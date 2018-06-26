#include "HeatMap.h"
HeatMap::HeatMap(Mat groundmap)
{
    this->groundmap=groundmap;
    this->home_occ_freq=Mat::zeros(this->groundmap.size(),CV_8UC1);
    this->home_occ_time=Mat::zeros(this->groundmap.size(),CV_32FC1);

}
void HeatMap::sortHeatMapScores(vector<double>& scores,vector<Sensor>& positions)
{
    int i, j, flag = 1;    // set flag to 1 to start first pass
    double temp_score;
    Point temp_position;
    int numLength = scores.size();
    for(i = 1; (i <= numLength) && flag; i++)
    {
        flag = 0;
        for (j=0; j < (numLength -1); j++)
        {
            if (scores[j+1] < scores[j])      // ascending order simply changes to <
            {

                temp_score = scores[j];             // swap elements
                scores[j] = scores[j+1];
                scores[j+1] = temp_score;

                temp_position = positions[j].getSensorPosition();             // swap elements
                positions[j].setSensorPosition(positions[j+1].getSensorPosition());
                positions[j+1].setSensorPosition(temp_position);

                flag = 1;               // indicates that a swap occurred.
            }
        }
    }
    return;   //arrays are passed to functions by address; nothing is returned
}



void HeatMap::printHomeGrid()
{

    for(int i=0;i<home_grid_positions.size();i++)
    {
        if(home_occ_freq.at<int>(home_grid_positions[i])!=0)
            cout<<home_occ_freq.at<int>(home_grid_positions[i])<<endl;
    }
}

void HeatMap::computeOccupancyFrequency(vector<Sensor>& sensor_position,bool& is_valid_heatmap, set<string>& room_ids)
{

    if(sensor_position.size()>0)
    {
        is_valid_heatmap=true;

        for(int i=0;i<sensor_position.size();i++)
        {
            home_occ_freq.at<int>(sensor_position[i].getSensorPosition())+=1;
            room_ids.insert(sensor_position[i].getSensorRoomId());

        }
    }
    else
        is_valid_heatmap=false;

}

void HeatMap::computeOccupancyTime(vector<Sensor>& sensor_position,bool& is_valid_heatmap,set<string>& room_ids)
{

    if(sensor_position.size()>0)
    {
        is_valid_heatmap=true;
        time_t prev=sensor_position[0].getSensorTimestamp();

        for(int i=1; i<sensor_position.size();i++)
        {
            time_t curr=sensor_position[i].getSensorTimestamp();
            double time_in_sec=curr-prev;
            //cout<<time_in_sec<<endl;
            home_occ_time.at<float>(sensor_position[i-1].getSensorPosition())+=time_in_sec;
            prev=curr;
            room_ids.insert(sensor_position[i].getSensorRoomId());
        }

    }
    else
        is_valid_heatmap=false;

}

Mat HeatMap::createOccupancyTimeHeatMap(vector<Sensor>& sensor_position)
{

    vector<double> sortedHomeGridScores;

    Mat activityMap=Mat::zeros(home_occ_time.size(),CV_32F);
    Mat activityMap2=Mat::zeros(activityMap.size(),CV_32F);
    Mat activityMap3=Mat::zeros(activityMap.size(),CV_32FC3);

    applyColorMap(activityMap2,activityMap3,2);

    for(int i=0;i< sensor_position.size();i++)
    {
       // cout<<home_occ_time.at<float>(sensor_position[i].getSensorPosition())<<endl;
        sortedHomeGridScores.push_back(home_occ_time.at<float>(sensor_position[i].getSensorPosition()));
    }


    sortHeatMapScores(sortedHomeGridScores,sensor_position);

    for(int i=0; i<sortedHomeGridScores.size();i++)
    {
        activityMap.at<float>(sensor_position[i].getSensorPosition())=sortedHomeGridScores[i];
        cv::GaussianBlur(activityMap,activityMap2,cv::Size(0,0),2.0);
        cv::normalize(activityMap2,activityMap2,0,255,cv::NORM_MINMAX, CV_8UC1);
        cv::applyColorMap(activityMap2, activityMap3, 2);

    }

     return activityMap3;

}

Mat HeatMap::createOccupancyFrequencyHeatMap(vector<Sensor>& sensor_position)
{

    vector<double> sortedHomeGridScores;

    Mat activityMap=Mat::zeros(home_occ_freq.size(),CV_32F);
    Mat activityMap2=Mat::zeros(activityMap.size(),CV_32F);
    Mat activityMap3=Mat::zeros(activityMap.size(),CV_32FC3);

    applyColorMap(activityMap2,activityMap3,2);

    for(int i=0;i< sensor_position.size();i++)
        sortedHomeGridScores.push_back(home_occ_freq.at<int>(sensor_position[i].getSensorPosition()));


    sortHeatMapScores(sortedHomeGridScores,sensor_position);

    for(int i=0; i<sortedHomeGridScores.size();i++)
    {
        activityMap.at<float>(sensor_position[i].getSensorPosition())=sortedHomeGridScores[i];
        cv::GaussianBlur(activityMap,activityMap2,cv::Size(0,0),2.0);
        cv::normalize(activityMap2,activityMap2,0,255,cv::NORM_MINMAX, CV_8UC1);
        cv::applyColorMap(activityMap2, activityMap3, 2);
    }


     return activityMap3;

}


//Mat HeatMap::createHeatMap()
//{
//    vector<int> sortedHomeGridScores;
//    int previous=0;

//    Mat activityMap=Mat::zeros(groundmap.size(),CV_8UC1);
//    Mat activityMap2=Mat::zeros(activityMap.size(),CV_8UC1);
//    Mat activityMap3=Mat::zeros(activityMap.size(),CV_8UC3);

//    applyColorMap(activityMap2,activityMap3,2);

//    for(int i=0;i< home_grid_positions.size();i++)
//        sortedHomeGridScores.push_back(home_occ_freq.at<int>(home_grid_positions[i]));

//    sortHeatMapScores(sortedHomeGridScores,home_grid_positions);

//    for(int x=0; x<sortedHomeGridScores.size();x++)
//    {
//        if(previous==0 || previous!=sortedHomeGridScores[x])
//        {

//            for(int z=0; z<home_grid_positions.size(); z++)
//            {
//                for(int k=0; k<sortedHomeGridScores[x];k++)
//                    circle(activityMap, home_grid_positions[z], 7, cv::Scalar(255,0,0), -1);

//                //no longer active spot after drawing red dot
//                home_occ_freq.at<int>(home_grid_positions[z])=0;
//                home_grid_positions.erase(home_grid_positions.begin()+z);
//            }
//            //not to normalize the most active spot
//            cv::normalize(activityMap,activityMap,0,200,cv::NORM_MINMAX, CV_8UC1);
//        }
//        //remember previous activity spot element
//        previous=sortedHomeGridScores[x];
//        cv::GaussianBlur(activityMap,activityMap2,cv::Size(0,0),3.0);
//        cv::normalize(activityMap2,activityMap2,0,255,cv::NORM_MINMAX, CV_8UC1);
//        cv::applyColorMap(activityMap2, activityMap3, 2);

//    }

//    return activityMap3;
//}
