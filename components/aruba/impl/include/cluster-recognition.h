#ifndef CLUSTERRECOGNITION_H
#define CLUSTERRECOGNITION_H
#include<logger.h>
#include <home.h>
#include <numeric>
#include <math.h>
#include <feature-container.h>
#include <feature-processor.h>
#include <feature-reader.h>
#include <feature-writer.h>
#include <utilities.h>

class ClusterRecognition{
public:
    ClusterRecognition(string hierarchal_clustering_path,string cluster_rec_path,string within_day_cluster_path,string home_setup_file, string time_window_config,bool& success);
    ~ClusterRecognition();
     void destroy();
     void run();

private:
    void init_(string hierarchal_clustering_path,string cluster_rec_path,string with_day_cluster_path,string home_setup_file, string time_window_config,bool& success);
    void computeClustersPurity_(FeatureContainer* featureContainers);
    void computeClusterPurity_(int most_selected_activity_index,vector<int> activity_label, vector<int>& most_common_activity_count_cluster,vector<int>& all_activity_count_cluster);
    bool includeOtherActivityClass_(int activity_index);
    void computeFeatures_(FeatureContainer* featureContainers);
    void leaveOneDayOutStrategy_(vector<FeatureContainer*> sensor_data, FeatureContainer* clustered_sensor_data);
    void prepareTrainingData_(string test_day, FeatureContainer* clustered_sensor_data, FeatureContainer* train_sensor_data);
    void recognize_(FeatureContainer* test_sensor_data,
                    FeatureContainer* train_sensor_data,
                    vector<string>& actual_activity_labels,
                    vector<string>& predicted_activity_labels,
                    vector<int>& predicted_discovered_patterns);
private:
    string cluster_rec_path_;
    string within_day_cluster_path_;
    FeatureContainer* clustered_sensor_data;
    vector<FeatureContainer*> sensor_data;

    FeatureProcessor* featureProcessor_;
    FeatureReader* featureReader_;
    FeatureWriter* featureWriter_;
    Home* home_;

};
#endif // CLUSTERRECOGNITION_H
