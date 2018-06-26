#include<Recognition.h>

Recognition::Recognition()
{

}


void Recognition::run()
{
    if( remove( "../data/final_phase/results.txt" ) != 0 )
      perror( "Error deleting file" );
    else
      puts( "File successfully deleted" );

    vector<vector<string> > clusters=readDiscoveredFile();
    vector<vector<string> > features=readFeatureFile();
    readConfigFile();

    applyOneLeaveOut(features,clusters);

}
vector<vector<string> > Recognition::readDiscoveredFile()
{
    cout<<"readDiscoveredFile"<<endl;
    vector<vector<string> > file_content;

    string file_name="../data/final_phase/discovered_patterns.txt";
    std::ifstream infile(file_name);

    while (infile)
    {
        string line;
        vector<string> elem;
        if (!getline( infile, line )) break;

        Common::split(line,'\t',elem);

        //0 - cluster id
        //1 - cluster activity name


        vector<string> cluster_elem;
        cluster_elem.push_back(elem[0]);
        cluster_elem.push_back(elem[1]);
        cluster_elem.push_back(elem[elem.size()-1]);


        file_content.push_back(elem);
    }
    infile.close();

    return file_content;
}

vector<vector<string> > Recognition::readFeatureFile()
{
    cout<<"readFeatureFile"<<endl;
    vector<vector<string> > file_content;

    string file_name="../data/final_phase/recognition_step.txt";

    std::ifstream infile(file_name);


    while (infile)
    {
        string line;
        vector<string> line_content;
        vector<string> elem;
        if (!getline( infile, line )) break;

        Common::split(line,'\t',elem);

        //0 - cluster activity id
        //1 - cluster id
        //2 cluster start time

        for(int i =0;i<elem.size();i++)
            line_content.push_back(elem[i].c_str());

        file_content.push_back(line_content);
    }
    infile.close();

    return file_content;
}
int Recognition::getStartTime(int index)
{
    float start_time=(1.0*index)/1440.0;
    start_time=start_time-(long)start_time;

    int index_time=(start_time*1440)/60;

    //cout<<index<<"\t"<<start_time<<"\t"<<index_time<<endl;

    return index_time;

}

void Recognition::recongize(vector<vector<string> > train_days, vector<vector<string> > test_day, vector<vector<string> > clusters)
{

    vector<vector<int> > clusters_sensor_duration;
    vector<vector<int> > clusters_sensor_structure;
    vector<int> clusters_start_time;
    vector<float> clusters_weight;


    cout<<"Computing Clusters Centroid"<<endl;
    for( int i=0;i<clusters.size();i++)
    {

        //cluster id position in index 0
        vector<string> cluster_elem=clusters[i];
        vector<vector<string> > tmp_cluster_features;
        float count=0.0;

        for(int j=0;j<train_days.size();j++)
        {
            //cluster_id position in  index 1
            vector<string> train_day_elem=train_days[j];




            if(cluster_elem[0]==train_day_elem[1])
            {

                int start_time=getStartTime(j);
                train_day_elem.push_back(std::to_string(start_time));
                tmp_cluster_features.push_back(train_day_elem);
                count++;

            }

        }


        vector<int> durations;
        vector<int> sensor_structure;
        int start_time;

        clusters_weight.push_back(count);



        computeClusterCentroid(tmp_cluster_features,start_time,durations,sensor_structure);



        clusters_sensor_duration.push_back(durations);
        clusters_sensor_structure.push_back(sensor_structure);
        clusters_start_time.push_back(start_time);

    }




    vector<vector<float> > test_day_sim= computeSimilarity(test_day,clusters_sensor_duration,clusters_sensor_structure,clusters_start_time,clusters_weight);
    vector<int> predicted_labels_index;

    std::ofstream outfile;
    outfile.open("../data/final_phase/results.txt", std::ios_base::app);

    cout<<"Write Output"<<endl;
    for(int i=0;i<test_day_sim.size();i++)
    {


        vector<float> test_items_sim=test_day_sim[i];
        vector<string> test_day_line=test_day[i];
        int start_time=getStartTime(i);

        auto biggest = std::max_element(std::begin(test_items_sim), std::end(test_items_sim));
        int pos=std::distance(std::begin(test_items_sim), biggest);

        vector<string> cluster_item=clusters[pos];


        //ground_truth,semantic_label,predicted_label
        //cout<<start_time<<"\t"<<this->activity_names[atoi(test_day_line[0].c_str())]<<"\t"<<this->activity_names[atoi(test_day_line[2].c_str())]<<"\t"<<cluster_item[1]<<endl;
        //if(atoi(test_day_line[0].c_str())!=11)
            outfile<<test_day_line[0]<<"\t"<<cluster_item[5]<<endl;
        //cout<<cluster_item[1]<<"\t"<<this->activity_names[atoi(test_day_line[0].c_str())]<<endl;
        //predicted_labels_index.push_back(pos);
    }
    outfile.close();


}
vector<vector<float> > Recognition::computeSimilarity(vector<vector<string> > test_day, vector<vector<int> >& clusters_sensor_duration,vector<vector<int> >& clusters_sensor_structure,vector<int>& clusters_start_time,vector<float>&clusters_weight)
{


    cout<<"Compute Test Day Features"<<endl;
    vector<vector<float> > test_day_sim;
    for(int i=0;i<test_day.size();i++)
    {
        int start_time1=getStartTime(i);
        vector<float> test_item_sim;

        vector<int> dur1;
        vector<int> structure_sensor1;

        vector<string> test_item=test_day[i];

        for(int k=3;k<test_item.size();k++)
        {
            dur1.push_back(atoi(test_item[k].c_str()));
            if(atoi(test_item[k].c_str())>0)
                structure_sensor1.push_back(k-3);

        }



        for(int j=0;j<clusters_sensor_duration.size();j++)
        {
            int start_time2=clusters_start_time[j];
            vector<int> dur2=clusters_sensor_duration[j];
            vector<int> structure_sensor2=clusters_sensor_structure[j];


            int sum_of_elems = std::accumulate(dur2.begin(), dur2.end(), 0);
            float final_similarity=0.0;


            if(sum_of_elems!=0)
            {
                float tmp_time_similarity=computeTimeSimilarity(start_time1,start_time2);
                float tmp_duration_similarity=computeDurationSimilarity(dur1,dur2);
                float tmp_structure_sensor_similarity=0.0;



                if(structure_sensor1.size()!=0 && structure_sensor2.size()!=0)
                {

                    tmp_structure_sensor_similarity=computeStructureSimilarity(structure_sensor1,structure_sensor2);
                    final_similarity=(tmp_time_similarity+tmp_duration_similarity+tmp_structure_sensor_similarity)/3;
                }
                else
                    final_similarity=(tmp_time_similarity+tmp_duration_similarity)/2;

            }

            test_item_sim.push_back(final_similarity);

        }
        test_day_sim.push_back(test_item_sim);
    }

    return test_day_sim;
}


void Recognition::computeClusterCentroid(vector<vector<string> > cluster_feature, int &start_time, vector<int> &duration, vector<int> &sensor_structure)
{
    vector<int> tmp_start_times(24,0);

    for(int i=0;i<cluster_feature.size();i++)
    {
        vector<string> cluster_items=cluster_feature[i];
        tmp_start_times[atoi(cluster_items[cluster_items.size()-1].c_str())]++;


        for(int j=3;j<cluster_items.size()-1;j++)
        {
            if(i==0)
                duration.push_back(atoi(cluster_items[j].c_str()));
            else
            {
                duration[j-3]+=atoi(cluster_items[j].c_str());
            }
        }
    }


    for(int i=0;i<duration.size();i++)
    {


        duration[i]=ceil((duration[i]*1.0)/(cluster_feature.size()*1.0));


        if(duration[i]>0)
            sensor_structure.push_back(i);
    }


    auto biggest = std::max_element(std::begin(tmp_start_times), std::end(tmp_start_times));
    start_time=std::distance(std::begin(tmp_start_times), biggest);

}


void Recognition::applyOneLeaveOut(vector<vector<string> > features, vector<vector<string> > clusters)
{

    int num_days=features.size()/1440;


    int lower=0;
    int upper=1440;

    for(int i=0;i<num_days;i++)
    {

        vector<vector<string> >::const_iterator test_first = features.begin() + lower;
        vector<vector<string> >::const_iterator test_last = features.begin() + upper;
        vector<vector<string> > test_day(test_first, test_last);

        vector<vector<string> >::const_iterator train_day_after_first = features.begin() + upper;
        vector<vector<string> >::const_iterator train_day_after_last = features.begin() + features.size() ;
        vector<vector<string> > train_day_after(train_day_after_first, train_day_after_last);

        vector<vector<string> >::const_iterator train_day_before_first = features.begin();
        vector<vector<string> >::const_iterator train_day_before_last = features.begin() + lower ;
        vector<vector<string> > train_days(train_day_before_first, train_day_before_last);

        train_days.insert(train_days.end(),train_day_after.begin(),train_day_after.end());

         cout<<train_days.size()<<"\t"<<test_day.size()<<"\t"<<train_day_after.size()<<"\t"<<features.size()<<endl;
        recongize(train_days,test_day,clusters);



        lower=upper;
        upper=upper+1440;


    }

}

float Recognition::computeStructureSimilarity(vector<int>& xvec, vector<int>& yvec)
{

    float result=0.0;
    //    if(verbose)
    //        cout<<"computeStructureSimilarity"<<endl;


    vector<int> intersection_sensors,union_sensors;

    std::set_intersection (xvec.begin(), xvec.end(), yvec.begin(), yvec.end(), back_inserter(intersection_sensors));
    std::set_union(xvec.begin(), xvec.end(), yvec.begin(), yvec.end(), back_inserter(union_sensors));


    result=(1.0*intersection_sensors.size())/(1.0*union_sensors.size());

    return result;

}

float Recognition::computeDurationSimilarity(vector<int> &dur1,vector<int>& dur2)
{
    //    if(verbose)
    //        cout<<"computeDurationSimilarity"<<endl;
    float sum1=0,sum2=0,max=0;
    int sum=0;

    //    if(dur1.size()==0 && dur2.size()==0)
    //    {
    //        max=1;

    //    }
    //    else
    // {
    sum1=  std::accumulate(dur1.begin(), dur1.end(), 0);
    sum2=  std::accumulate(dur2.begin(), dur2.end(), 0);
    max=sum1+sum2;

    for(int i=0;i<dur1.size();i++)
        sum+=abs(dur1[i]-dur2[i]);
    // }

    float dur_sim=1-((1.0*sum)/(max*1.0));

    return dur_sim;
}

float Recognition::computeTimeSimilarity(int& start_time1,int& start_time2)
{
    //    if(verbose)
    //        cout<<"computeTimeSimilarity"<<endl;

    float radian1=(((1.0*start_time1/(1.0*24))*360*1.0)*M_PI)/(1.0*180);
    float radian2=(((1.0*start_time2/(1.0*24))*360*1.0)*M_PI)/(1.0*180);

    float time_sim=1-(abs(radian2-radian1)/(2*M_PI));

    return time_sim;
}

void Recognition::readConfigFile()
{
    string file_name="../data/config";

    std::ifstream infile( file_name.c_str());
    int line_index=0;


    while (infile)
    {
        string line;

        if (!getline( infile, line )) break;

        vector<string> elem;
        Common::split(line,'=',elem);


        if(line_index>2)
        {
            if(elem[0]!="Enter_Home")
            {
                activity_names.push_back(elem[0]);
                activity_indexes.push_back(atoi(elem[1].c_str()));
            }
        }

        line_index++;
    }
    infile.close();

}
