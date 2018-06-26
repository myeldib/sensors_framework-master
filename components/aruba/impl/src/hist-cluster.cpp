/*
 * hist_cluster.cpp
 *
 *  Created on: 2017-04-14
 *      Author: Mohamed Eldib
 * This class provides a clustering algorithm to cluster short duration feature vectors
 * and long duration feature vector within a day (a single day instance).
 * - The long duration feature vectors (location histograms) and  the short duration feature
 *  vectors (sensor histograms) are loaded from files for a single day
 * - The location histograms are clustered by computing the similarity between each pair of histograms:
 *      * Compute the similarity between each pair of histograms
 *          # The distance between two histograms is based on the histogram difference (sum the differences between each bin)
 *      * Check if the histogram difference is below a threshold using an exponantial distribution:
 *          # The exponantial function relies on parameter lambda and histogram difference: 1-exp(-lamda*histogram_difference)
 *          # Two histograms are declared similar, if the similarity value computed from the exponantial function is less than 0.50 (threshold)
 * - The long duration cluster ids are assgined to the short duration feature vectors.
 * - Clustering is applied again on the short duration feature vectors that do not belong to any cluster from the previous step
 *
 */
#include<hist-cluster.h>

HistogramClustering::HistogramClustering(string hist_folder_path,Home* home)
{
    COUT<<"HistogramClustering"<<endl;
    logging::INFO("HistogramClustering");

    this->hist_folder_path=hist_folder_path;
    this->home=home;
    this->serialize_results = false;
    this->num_threads = boost::thread::hardware_concurrency();
}

/**
 * @brief HistogramClustering::~HistogramClustering
 */
HistogramClustering::~HistogramClustering()
{
    COUT<<"~HistogramClustering"<<endl;
    logging::INFO("~HistogramClustering");
}

/**
 * @brief HistogramClustering::destroy
 */
void HistogramClustering::destroy()
{
    COUT<<"destroy"<<endl;
    logging::INFO("destroy");
    delete this;
}

/**
 * @brief HistogramClustering::writeClusteredPatterns
 * @param sensor_clusters
 * @param folder_name
 */
void HistogramClustering::writeClusteredPatterns(vector<int> sensor_clusters, string folder_name)
{
    COUT<<"writeClusteredPatterns"<<endl;
    logging::INFO("writeClusteredPatterns");

    string sequence_patterns_filename = Common::buildOutputFile(this->hist_folder_path,Common::extractDayFromFileName(folder_name),"sequence_patterns");
    string discovered_patterns_filename = Common::buildOutputFile(this->hist_folder_path,Common::extractDayFromFileName(folder_name),"discovered_patterns");

    fstream  clustered_patterns_file(sequence_patterns_filename, std::fstream::out);
    fstream  discovered_patterns_file(discovered_patterns_filename, std::fstream::out);

    //write clustered patterns to file
    for(int j=0;j<sensor_clusters.size();j++)
        clustered_patterns_file<<sensor_clusters[j]<<endl;

    //erase duplicates to write unique discovered patterns to file
    Common::eraseDuplicatesFromVector(sensor_clusters);

    for(int j=0;j<sensor_clusters.size();j++)
        discovered_patterns_file<<sensor_clusters[j]<<endl;

}

/**
 * @brief HistogramClustering::generateClustersFromHistograms
 * The algorith starts with finding clusters using the long duration feature vectors,
 * then assign the short duration feature vectors to the discovered clusters from the long duration feature vectors.
 * Finally, the unassigned short duration feature vectors are clustered
 */
void HistogramClustering::run()
{
    COUT<<"generateClustersFromHistograms"<<endl;
    logging::INFO("generateClustersFromHistograms");

    this->home->readHomeSetup();

    //get folder names
    vector<string> data_files=Common::getFolderFileNames(this->hist_folder_path,"");
    //load long duration feature vectors from files (location histograms)
    vector<string> location_hist_files=Common::getFolderFileNames(this->hist_folder_path,"locations_duration");

    //load short duration feature vectors from files (sensor histograms)
    vector<string> sensor_hist_files=Common::getFolderFileNames(this->hist_folder_path,"sensors_duration");


    vector<vector<string> > sensor_hist_files_per_thread,location_hist_files_per_thread,data_files_per_thread;

    divideElementsBetweenThreads_(location_hist_files,
                                  sensor_hist_files,
                                  data_files,
                                  location_hist_files_per_thread,
                                  sensor_hist_files_per_thread,
                                  data_files_per_thread);

    //threading to generateClusters_
    for(int i =0;i<sensor_hist_files_per_thread.size();i++)
    {
        vector<string> sub_sensor_hist_files= sensor_hist_files_per_thread[i];
        vector<string> sub_location_hist_files = location_hist_files_per_thread[i];
        vector<string> sub_data_files = data_files_per_thread[i];

        for(int j=0;j<sub_sensor_hist_files.size();j++)
        {
            logging::INFO(sub_sensor_hist_files[j]);
        }

        g.add_thread(new boost::thread([sub_location_hist_files,sub_sensor_hist_files,sub_data_files,this] { generateClusters_(sub_location_hist_files,sub_sensor_hist_files,sub_data_files); }));
    }

    g.join_all();

}

/**
 * @brief HistogramClustering::divideElementsBetweenThreads_
 * @param location_hist_files
 * @param sensor_hist_files
 * @param data_files
 * @param location_hist_files_per_thread
 * @param sensor_hist_files_per_thread
 * @param data_files_per_thread
 */
void HistogramClustering::divideElementsBetweenThreads_(vector<string> location_hist_files,
                                                        vector<string> sensor_hist_files,
                                                        vector<string> data_files,
                                                        vector<vector<string> > &location_hist_files_per_thread,
                                                        vector<vector<string> > &sensor_hist_files_per_thread,
                                                        vector<vector<string> > &data_files_per_thread)
{
    logging::INFO("divideElementsBetweenThreads_");

    //number of assigned elements to run in each thread
    vector<int> num_elements_per_thread;

    Common::computeNumElementsPerThread(sensor_hist_files.size(),num_threads,num_elements_per_thread);

    int from=0;
    int to = num_elements_per_thread[0];

    for(int i =0; i<num_elements_per_thread.size();i++)
    {
        logging::INFO("from:"+
                      std::to_string(from)+
                      "\t"+
                      "to:"+
                      std::to_string(to));


        divideElements_(from,
                        to,
                        location_hist_files,
                        sensor_hist_files,
                        data_files,
                        location_hist_files_per_thread,
                        sensor_hist_files_per_thread,
                        data_files_per_thread);

        //update ranges
        from = to;
        to+=num_elements_per_thread[i+1];
    }
}

/**
 * @brief HistogramClustering::divideElementsBetweenThreads_
 * @param from
 * @param to
 * @param location_hist_files
 * @param sensor_hist_files
 * @param location_hist_files_per_thread
 * @param sensor_hist_files_per_thread
 */
void HistogramClustering::divideElements_(int from,
                                          int to,
                                          vector<string> location_hist_files,
                                          vector<string> sensor_hist_files,
                                          vector<string> data_files,
                                          vector<vector<string> > &location_hist_files_per_thread,
                                          vector<vector<string> > &sensor_hist_files_per_thread,
                                          vector<vector<string> >& data_files_per_thread)
{
    logging::INFO("divideElements_");

    vector<string> sub_sensor_hist_files;
    vector<string> sub_location_hist_files;
    vector<string> sub_data_files;

    //copy data to sub container
    for(int i = from; i< to; i++)
    {
        sub_sensor_hist_files.push_back(sensor_hist_files[i]);
        sub_location_hist_files.push_back(location_hist_files[i]);
        sub_data_files.push_back(data_files[i]);

    }

    location_hist_files_per_thread.push_back(sub_location_hist_files);
    sensor_hist_files_per_thread.push_back(sub_sensor_hist_files);
    data_files_per_thread.push_back(sub_data_files);
}

/**
 * @brief HistogramClustering::generateClusters
 * @param location_hist_files
 * @param sensor_hist_files
 */
void HistogramClustering::generateClusters_(vector<string> location_hist_files, vector<string> sensor_hist_files,vector<string> data_files)
{
    logging::INFO("generateClusters_");

    //for each day, discover duration clusters using the long and short duration feature vectors
    for(int i=0;i<location_hist_files.size();i++)
    {

        COUT<<"Processing file:"<<location_hist_files[i]<<endl;
        logging::INFO("Processing file:"+location_hist_files[i]);

        //read long duration feature vectors from file
        vector<vector<int> > locations_hist=readHistogramFile(location_hist_files[i]);
        //read short duration feature vectors from file
        vector<vector<int> > sensors_hist=readHistogramFile(sensor_hist_files[i]);

        //declare vairables to represent the found clusters from the long and short durations
        //all feature vectors (short or long) are assigned to cluster 0 by default
        vector<int> locations_clusters(locations_hist.size(),0);
        vector<int> sensor_clusters(sensors_hist.size(),0);

        //find clusters using long duration feature vectors (location histograms)
        clusterHistograms(locations_hist,home->getLocationTimeInterval(),home->getTimeConstraint(),home->getLongDurationLambda(),locations_clusters);


        for(int j=0;j<locations_clusters.size();j++)
        {
            COUT<<j<<"\t"<<locations_clusters[j]<<endl;
        }

        //assign short duration feature vectors to the discovered clusters
        mapLocationClustersToSensorClusters(locations_clusters,sensor_clusters,home->getSensorTimeInterval(),home->getLocationTimeInterval());

        for(int j=0;j<sensor_clusters.size();j++)
        {
            COUT<<j<<"\t"<<sensor_clusters[j]<<endl;
        }

        //discover clusters for the unassigned short duration feature vectors from the previous step
        clusterHistograms(sensors_hist,home->getSensorTimeInterval(),home->getTimeConstraint(),home->getShortDurationLambda(),sensor_clusters);

        //write clustered sensors
        writeClusteredPatterns(sensor_clusters,data_files[i]);

        //write location abs duration difference to file
        writeAbsDifferenceToFile(locations_hist,sensors_hist);

    }
}

/**
 * @brief HistogramClustering::writeAbsDifferenceToFile
 * @param location_durations
 * @param sensor_durations
 */\
void HistogramClustering::writeAbsDifferenceToFile(vector<vector<int> > locations_hist, vector<vector<int> >sensors_hist)
{

    COUT<<"writeAbsDifferenceToFile"<<endl;
    logging::INFO("writeAbsDifferenceToFile");

    if(this->serialize_results)
    {
        string location_output= this->hist_folder_path;
        location_output.append("location_abs_diff.txt");
        int rc= serializeAbsDurationDifference(locations_hist,home->getLocationTimeInterval(),home->getTimeConstraint(),location_output);

        if(!rc)
        {
            cerr<<"ERROR serializeAbsDurationDifference"<<endl;
            logging::ERROR("serializeAbsDurationDifference");
        }

    }

    //write location abs duration difference to file
    if(this->serialize_results)
    {
        string sensor_output= this->hist_folder_path;
        sensor_output.append("sensor_abs_diff.txt");
        int rc= serializeAbsDurationDifference(sensors_hist,home->getSensorTimeInterval(),home->getTimeConstraint(),sensor_output);

        if(!rc)
        {
            cerr<<"ERROR serializeAbsDurationDifference"<<endl;
            logging::ERROR("serializeAbsDurationDifference");
        }

    }
}

/**
 * @brief HistogramClustering::computeDaySegment
 * @param window_id: the id of the feature vector
 * @param time_constraint: the size of segment of the day (seconds)
 * @param time_interval: the size of feature vector in unit of time (seconds)
 * @return the day segment id of the duration feature vector given its id
 */
int HistogramClustering::computeDaySegment(int window_id, int time_constraint, int time_interval)
{
    COUT<<"computeDaySegment"<<endl;
    logging::INFO("computeDaySegment");

    return ceil((window_id*time_interval*1.0)/time_constraint*1.0);
}
/**
 * @brief HistogramClustering::mapLocationClustersToSensorClusters
 * @param location_clusters: the discovered clusters from the long duration feature vectors (location histograms)
 * @param sensor_clusters: the long duration cluster ids are assgined to the short duration feature vectors.
 * @param short_time_interval: the size of the short duration feature vector in unit of time (seconds)
 * @param long_time_interval: the size of the long duration feature vector in unit of time (seconds)
 */
void HistogramClustering:: mapLocationClustersToSensorClusters(vector<int> location_clusters,vector<int>& sensor_clusters, int short_time_interval, int long_time_interval)
{
    COUT<<"mapLocationClustersToSensorClusters"<<endl;
    logging::INFO("mapLocationClustersToSensorClusters");

    map<int,int> discovered_patterns=Common::countVectorElements(location_clusters);


    for( auto l : discovered_patterns )
    {
        //get cluster id
        int cluster_id=l.first;

        for(int i=0; i<location_clusters.size();i++)
        {
            if(cluster_id==location_clusters[i])
            {
                //start and end of short time interval window
                int start_short_window_index=(i*long_time_interval)/short_time_interval;
                int end_short_window_index=(i*long_time_interval+long_time_interval)/short_time_interval;

                for(int j=start_short_window_index;j<end_short_window_index;j++)
                {
                    sensor_clusters[j]=cluster_id;
                }
            }

        }

    }

}

/**
 * @brief HistogramClustering::clusterHistograms
 * - By default all feature vectors are assinged to cluster 0
 * - Each feature vector inside a day segment is compared to all feature vectors in the same day segment
 * - A cluster is formed if two or more feature vectors are similar to each other (similar histograms)
 * - Once a feature vector is assigned to a cluster then, this feature vector is not evaluated again
 * @param histograms: duration feature vectors (miliseconds)
 * @param time_interval: the size of the duration feature vector in unit of time (seconds)
 * @param time_constraint: the size of segment of the day (seconds)
 * @param duration_lambda: a threshold value for the exponential function to determine the distance between two histograms (seconds)
 * @param clusters: list of cluster ids where the duration feature vectors are assigned to
 */
void HistogramClustering::clusterHistograms(vector<vector<int> > histograms,int time_interval,int time_constraint,int duration_lambda,vector<int>& clusters)
{

    COUT<<"clusterHistograms"<<endl;
    logging::INFO("clusterHistograms");

    //compute number of steps inside a single day segment
    int step=ceil((time_constraint*1.0)/(time_interval*1.0));
    //incrementer for the count
    int step_increment=ceil((time_constraint*1.0)/(time_interval*1.0));
    //if the list has already cluster ids, then pick up the max id + 1
    int cluster_id=*max_element(clusters.begin(), clusters.end())+1;

    for (int i = 0; i < histograms.size();i++ )
    {
        //feature vector (histogram)
        vector<int> hist1=histograms[i];
        bool update_cluster_id=false;

        //if feature vector does not belong to any cluster yet, then evaluate it against other feature vectors
        if(!clusters[i])
        {
            //compare current feature vector to others in the same day segment
            for (int j = i; j < step_increment && j<histograms.size(); j++)
            {
                //feature vector in same day segment
                vector<int> hist2=histograms[j];

                COUT<<"round:"<<i<<"\t"<<j<<"\t"<<cluster_id<<"\t"<<step_increment<<endl;

                //check the similarity between two feature vectors, and the second feature vector has not been assigned to a cluster
                if(isSimilarHistograms(hist1,hist2,duration_lambda) && !clusters[j])
                {
                    //assign feature vector to a cluster
                    clusters[j]=cluster_id;
                    //update cluster ids if no feature vectors are similar for the next comparison
                    update_cluster_id=true;
                }

            }
        }

        //update cluster id
        if(update_cluster_id)
            cluster_id++;

        //update day segment
        if(i+1==step_increment)
            step_increment+=step;
    }

}
/**
 * @brief HistogramClustering::serializeAbsDurationDifference
 * @param histograms
 * @param time_interval
 * @param time_constraint
 * @param file_name
 * @return
 */
int HistogramClustering::serializeAbsDurationDifference(vector<vector<int> > histograms, int time_interval, int time_constraint,string file_name)
{

    COUT<<"serializeAbsDurationDifference"<<endl;
    logging::INFO("serializeAbsDurationDifference");

    FileSeparator* fs = new FileSeparator(file_name,this->hist_folder_path);
    int rc = 1;

    //compute number of steps inside a single day segment
    int step=ceil((time_constraint*1.0)/(time_interval*1.0));
    //incrementer for the count
    int step_increment=ceil((time_constraint*1.0)/(time_interval*1.0));


    for (int i = 0; i < histograms.size();i++ )
    {
        //feature vector (histogram)
        vector<int> hist1=histograms[i];

        //compare current feature vector to others in the same day segment
        for (int j = i; j < step_increment && j<histograms.size(); j++)
        {
            //feature vector in same day segment
            vector<int> hist2=histograms[j];

            //compute the feature vectors difference (histogram difference)
            int hist_diff=computeHistogramsDifference(hist1,hist2);

            COUT<<hist_diff<<endl;
            logging::INFO(std::to_string(hist_diff));

            rc =fs->writeValueToFile(hist_diff);

            if(!rc)
            {
                return rc;
            }

        }

        //update day segment
        if(i+1==step_increment)
            step_increment+=step;
    }

    if(fs)
    {
        delete fs;
    }
    return rc;

}

/**
 * @brief HistogramClustering::isSimilarHistograms
 * - Use an exponential fucntion to determine if two feature vectors are similar
 * @param hist1: first duration feature vector (histogram) in miliseconds
 * @param hist2: second duration feature vector (histogram) in miliseconds
 * @param duration_lambda: a threshold value for the exponential function to determine the distance between two histograms (seconds)
 * @return: true if two feature vectors are similar, otherwise false
 */
bool HistogramClustering::isSimilarHistograms(vector<int> hist1, vector<int> hist2,int duration_lambda)
{
    COUT<<"isSimilarHistograms"<<endl;
    //logging::INFO("isSimilarHistograms");

    //compute the feature vectors difference (histogram difference)
    int hist_diff=computeHistogramsDifference(hist1,hist2);

    //compute degree of similarity using an exponential function, where the duration lambda determine the degree of similarity
    //the function computers the degree of similairty using durations in miliseconds, and lambda in miliseconds as well
    float degree_similarity_histograms=1-std::exp(-1*pow(2,-1*(duration_lambda+Constants::EXP_ONE_MILI_SECOND))*hist_diff);

    COUT<<hist_diff<<"\t"<<degree_similarity_histograms<<"\t"<<duration_lambda<<endl;


    //if the degree of similarity is less than a threshold (area under the curve)
    if(degree_similarity_histograms<=home->getHistogramSimilarityThreshold())
        return true;

    return false;
}
/**
 * @brief HistogramClustering::computeHistogramsDifference
 * - The distance between two histograms is based on the histogram difference (sum the differences between each bin)
 * @param hist1: duration feature vector (histogram) in miliseconds
 * @param hist2: duration feature vector (histogram) in miliseconds
 * @return distance between two histograms in miliseconds
 */
float HistogramClustering::computeHistogramsDifference(vector<int> hist1, vector<int> hist2)
{
    COUT<<"computeHistogramsDifference"<<endl;
    //logging::INFO("computeHistogramsDifference");

    int hist_diff=0;
    for(int i=0;i<hist1.size();i++)
    {
        COUT<<hist1[i]<<"\t"<<hist2[i]<<"\t"<<abs(hist1[i]-hist2[i])<<endl;

        hist_diff+=std::abs(hist1[i]-hist2[i]);
    }

    return hist_diff;
}
/**
 * @brief HistogramClustering::readHistogramFile
 * @param file_path: histogram file
 * @return content of the histogram file as 2d matrix where each row represent a histogram (feature vector)
 */
vector<vector<int> > HistogramClustering::readHistogramFile(string file_path)
{
    COUT<<"readHistogramFile"<<endl;
    logging::INFO("readHistogramFile");

    //read histograms of a day instance from file
    ifstream in_file(file_path);
    vector<vector<int> > duration_histograms;

    while (in_file)
    {
        string line;
        vector<string> elem;
        vector<int> duration_histogram;

        if (!getline( in_file, line )) break;

        Common::split(line,Constants::COMMA_SEPARATOR,elem);

        for(int i=0;i<elem.size();i++)
            duration_histogram.push_back(atoi(elem[i].c_str()));

        duration_histograms.push_back(duration_histogram);
    }
    return duration_histograms;
}
