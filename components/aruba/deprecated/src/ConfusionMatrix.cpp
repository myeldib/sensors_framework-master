#include<ConfusionMatrix.h>

ConfusionMatrix::ConfusionMatrix()
{
    base_dir="../data/days/";
    //    f_sr = new SensorReader();
    //    f_sr->run();
    //    num_of_days=f_sr->getNumOfDays();
    //    f_day_parts=f_sr->getNumberOfDayParts();
    //    f_timeslot=f_sr->getTimeSlot();
    //    f_time_interval=f_sr->getTimeInterval();
    //    room_sybmols=f_sr->getRoomSymbols();
    //    days=f_sr->getDays();
    //    activity_names=f_sr->getActivityNames();
    //    activity_indexes=f_sr->getActivityIndexes();
    //    sensor_to_rooms=f_sr->getSensorToRooms();
    days=Common::getFolderNames(Common::getFolderFileNames("../data/days/"));
    //    delete f_sr;
    day_len=1440;


}
vector<vector<string> > ConfusionMatrix::readDiscoveredFile()
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
        cluster_elem.push_back(elem[1]);
        cluster_elem.push_back(elem[0]);


        file_content.push_back(elem);
    }
    infile.close();
    return file_content;
}
void ConfusionMatrix::readConfigFile()
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

float ConfusionMatrix::computeClusterPurity()
{
    cout<<"computeClusterPurity"<<endl;

    vector<int> all_labels_count;
    vector<int> cluster_count;

    string file_name="../data/final_phase/activity_patterns.txt";
    std::ifstream infile(file_name);

    while (infile)
    {
        string line;
        vector<string> elem;
        if (!getline( infile, line )) break;



        Common::split(line,'\t',elem);

        //0 - cluster id
        //1 - cluster activity name
        for(int i=0;i<elem.size();i++)
        {

                 vector<string> elem2;
            cout<<elem[i]<<endl;

            Common::split(elem[i].c_str(),':',elem2);
             if (elem2[0].find("-") == std::string::npos)
            all_labels_count.push_back(atoi(elem2[1].c_str()));
            cout<<elem2.size()<<"\t"<<elem2[0]<<"\t"<<elem2[1]<<endl;
            if (elem2[0].find("-") == std::string::npos && elem2[0].find("*") != std::string::npos)
            {
                cout<<"found\t"<<elem2[0]<<endl;
                cluster_count.push_back(atoi(elem2[1].c_str()));
            }
        }

    }

    int sum_labels=  std::accumulate(all_labels_count.begin(), all_labels_count.end(), 0);
    int sum_clusters=  std::accumulate(cluster_count.begin(), cluster_count.end(), 0);

    float purity=(sum_clusters*1.0)/(sum_labels*1.0);
    cout<<purity<<endl;
    infile.close();

    return purity;
}

void ConfusionMatrix::generateModifiedLabels(vector<vector<string> > &discovered_clusters, vector<vector<int> > &feature_labels)
{

    std::ofstream new_features_file("../data/final_phase/new_features.txt");
    std::ofstream recognition_step_file("../data/final_phase/recognition_step.txt");

    int count_match=0;
    int count_all=0;
    for(int i=0;i<feature_labels.size();i++)
    {
        vector<int> f_label=feature_labels[i];
        for(int j=0;j<discovered_clusters.size();j++)
        {
            vector<string> cluster = discovered_clusters[j];
            if(f_label[1]==atoi(cluster[0].c_str()))
            {
             // if(f_label[0]!=11)
                {
                    int pos = find(activity_names.begin(), activity_names.end(), cluster[1]) - activity_names.begin();


                    new_features_file<<f_label[0]<<"\t"<<activity_indexes[pos]<<"\t";
                    recognition_step_file<<f_label[0]<<"\t"<<f_label[1]<<"\t"<<activity_indexes[pos]<<"\t";

                    if(f_label[0]==activity_indexes[pos])
                        count_match++;
                    count_all++;
                    for(int c=2;c<f_label.size();c++)
                    {
                        if(c==f_label.size()-1)
                            new_features_file<<f_label[c]<<endl;
                        else
                            new_features_file<<f_label[c]<<"\t";
                    }

                    for(int c=3;c<f_label.size();c++)
                    {
                        if(c==f_label.size()-1)
                            recognition_step_file<<f_label[c]<<endl;
                        else
                            recognition_step_file<<f_label[c]<<"\t";
                    }
                }
            }

        }

    }
    recognition_step_file.close();
    new_features_file.close();
    float ratio=(1.0*count_match)/(1.0*count_all);
    cout<<count_all<<"\t"<<count_match<<"\t"<<ratio<<endl;

}

int ConfusionMatrix::computeDifference(vector<vector<int> > &source_features, vector<vector<int> > &processed_features)
{

    // difference between processed and raw features in seconds for each sensor
    int diff=0;
    for(int i=0;i<source_features.size();i++)
    {
        vector<int> source_f=source_features[i];
        vector<int> processed_f=processed_features[i];

        for(int j=0; j<source_f.size();j++)
        {
            cout<<source_f[j]<<",";

            if(processed_f[j]==1)
                processed_f[j]=0;
            if(processed_f[j]==2)
                processed_f[j]=1;

            int tmp_diff=abs(source_f[j]-processed_f[j]);
            diff=diff+tmp_diff;

        }
        cout<<endl;

        for(int j=0; j<processed_f.size();j++)
            cout<<processed_f[j]<<",";
        cout<<endl;
        cout<<"==========================="<<endl;



    }
    return diff;
}

void ConfusionMatrix::run()
{

    computeClusterPurity();
    num_of_days=days.size();

    readConfigFile();
    //    vector<vector<int> > source_features=readSensorEllapsedTimeFile();
    vector<vector<int> > cluster_features=readFeatureFile();
    vector<vector<string> > clusters_centroids=readDiscoveredFile();
    generateModifiedLabels(clusters_centroids,cluster_features);


    // int diff=computeDifference(source_features,cluster_features);



}
vector<vector<int> > ConfusionMatrix::readFeatureFile()
{
    cout<<"readFeatureFile"<<endl;
    vector<vector<int> > file_content;

    string file_name="../data/final_phase/features.txt";

    std::ifstream infile(file_name);


    while (infile)
    {
        string line;
        vector<int> line_content;
        vector<string> elem;
        if (!getline( infile, line )) break;

        Common::split(line,'\t',elem);

        //0 - cluster activity id
        //1 - cluster id
        //2 cluster start time

        for(int i =0;i<elem.size();i++)
            line_content.push_back(atoi(elem[i].c_str()));

        file_content.push_back(line_content);
    }
    infile.close();

    return file_content;
}

vector<vector<int> > ConfusionMatrix::readSensorEllapsedTimeFile()
{
    cout<<"readSensorEllapsedTimeFile"<<endl;
    vector<vector<int> > file_content;


    for(int i=0;i<num_of_days; i++)
    {

        string day=days[i];
        string file_name="../data/days/"+day+"/features/sensor_ellapsed_time.txt";

        std::ifstream infile(file_name);


        while (infile)
        {
            string line;
            vector<int> line_content;
            vector<string> elem;
            if (!getline( infile, line )) break;

            Common::split(line,',',elem);


            for(int i =0;i<elem.size();i++)
                line_content.push_back(atoi(elem[i].c_str()));


            file_content.push_back(line_content);
        }
        infile.close();

    }

    return file_content;

}
