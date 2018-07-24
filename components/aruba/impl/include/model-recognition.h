#ifndef MODELRECOGNITION_H
#define MODELRECOGNITION_H
#include<logger.h>
#include <home.h>
#include <numeric>
#include <math.h>
#include<thread>
#include<similarity-measure.h>
#include <feature-container.h>
#include <feature-processor.h>
#include <feature-reader.h>
#include <feature-writer.h>
#include <utilities.h>
#include<evaluation.h>
#include <python-runner.h>
#include <boost/thread.hpp>

class ModelRecognition{
public:
  ModelRecognition(string hierarchal_clustering_path,string config_path,string cluster_rec_path,string within_day_cluster_path,string home_setup_file, string time_window_config,bool& success);
  ~ModelRecognition();
  void destroy();
  void run();

private:
  void init_(string hierarchal_clustering_path,string config_path,string cluster_rec_path,string with_day_cluster_path,string home_setup_file, string time_window_config,bool& success);
  bool includeOtherActivityClass_(int activity_index);
  void computeFeatures_(FeatureContainer* featureContainers);
  void leaveOneDayOutStrategy_(vector<FeatureContainer*> sensor_data, FeatureContainer* copy_clustered_sensor_data,PyInterpreterState* interp);
  void prepareTrainingData_(string test_day, FeatureContainer* copy_clustered_sensor_data, FeatureContainer* train_sensor_data);
  void writePredictions_(FeatureContainer *copy_clustered_sensor_data);
  void evaluate_(FeatureContainer* fc);
  void computeSubContainersClusters_(vector<FeatureContainer*>& sensor_data,vector<FeatureContainer*>& copy_clustered_sensor_data, FeatureContainer* merged_sub_containers,int& num_threads);
  void divideContainerToSubContainers_(vector<FeatureContainer *> &sensor_data,int& num_threads,vector<vector<FeatureContainer*> >& subFeatureContainers);
  void divideContainer_(int thread_id, int from, int to,vector<FeatureContainer *> &sensor_data, vector<FeatureContainer*>& subFeatureContainer);
  void mergeSubContainersToContainer_(vector<FeatureContainer *>& copy_clustered_sensor_data, FeatureContainer *mergedSubFeatureContainer);
private:
  initialize_python init_python_;
  string cluster_rec_path_;
  string within_day_cluster_path_;
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
  string script_name_;
  string function_name1_;
  string function_name2_;
  int function_num_param_;

  vector<sub_interpreter*> sub_interpreters_vec_;

};
#endif // CLUSTERRECOGNITION_H
