#ifndef CLUSTERRECOGNITION_H
#define CLUSTERRECOGNITION_H
#include<logger.h>
#include <home.h>
#include <numeric>
#include <math.h>
#include<similarity-measure.h>
#include <feature-container.h>
#include <feature-processor.h>
#include <feature-reader.h>
#include <feature-writer.h>
#include <utilities.h>
#include<python-runner.h>
#include <boost/thread.hpp>

class ClusterRecognition{
public:
  ClusterRecognition(string hierarchal_clustering_path, string config_path, string cluster_rec_path,string within_day_cluster_path,string home_setup_file, string time_window_config,bool& success);
  ~ClusterRecognition();
  void destroy();
  void run();

private:
  void init_(string hierarchal_clustering_path, string config_path, string cluster_rec_path,string with_day_cluster_path,string home_setup_file, string time_window_config,bool& success);
  void computeClustersPurity_(FeatureContainer* featureContainers, float& purity);
  void computeClusterPurity_(int most_selected_activity_index,vector<int> activity_label, vector<int>& most_common_activity_count_cluster,vector<int>& all_activity_count_cluster);
  bool includeOtherActivityClass_(int activity_index);
  void computeFeatures_(FeatureContainer* featureContainers);
  void leaveOneDayOutStrategy_(vector<FeatureContainer*> sensor_data, FeatureContainer* copy_clustered_sensor_data);
  void prepareTrainingData_(string test_day, FeatureContainer* copy_clustered_sensor_data, FeatureContainer* train_sensor_data);
  void recognize_(FeatureContainer* test_sensor_data,
                  FeatureContainer* train_sensor_data,
                  vector<int>& actual_activity_labels,
                  vector<int>& predicted_activity_labels,
                  vector<int>& predicted_discovered_patterns);
  void printDurationTwoPatterns_(vector<float> sensor_durations1, vector<float> sensor_durations2);
  void checkPredictedActivityLabel_(int& predicted_activity,string& actual_activity_label,vector<int> activity_per_pattern);
  void writePredictions_(FeatureContainer *copy_clustered_sensor_data);
  void evaluate_(FeatureContainer* fc);
  void computeSubContainersClusters_(vector<FeatureContainer*>& sensor_data,vector<FeatureContainer*>& copy_clustered_sensor_data, FeatureContainer* merged_sub_containers,int& num_threads);
  void divideContainerToSubContainers_(vector<FeatureContainer *> &sensor_data,int& num_threads,vector<vector<FeatureContainer*> >& subFeatureContainers);
  void divideContainer_(int from, int to,vector<FeatureContainer *> &sensor_data, vector<FeatureContainer*>& subFeatureContainer);
  void mergeSubContainersToContainer_(vector<FeatureContainer *>& copy_clustered_sensor_data, FeatureContainer *mergedSubFeatureContainer);
private:
  string cluster_rec_path_;
  string within_day_cluster_path_;

  //python script info
  string script_name_;
  string function_name1_;
  int function_num_param_;

  //contains multiple copies of same data
  vector<FeatureContainer*> copy_clustered_sensor_data;
  vector<FeatureContainer*> sensor_data;
  FeatureContainer* merged_sub_containers;
  boost::thread_group thread_group_;

  FeatureProcessor* featureProcessor_;
  SimilarityMeasure* similarityMeasure_;
  FeatureReader* featureReader_;
  PythonRunner* pythonRunner_;
  FeatureWriter* featureWriter_;
  Home* home_;
  int num_threads_;

};
#endif // CLUSTERRECOGNITION_H
