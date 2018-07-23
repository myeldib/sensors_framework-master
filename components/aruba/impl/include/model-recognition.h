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
#include <confusion.h>
#include<evaluation.h>
#include <python-runner.h>
#include <boost/thread.hpp>

// initialize and clean up python
struct initialize_python
{
  initialize_python()
  {
    logging::INFO("initialize_python");
    Py_InitializeEx(1);
    PyEval_InitThreads();
  }

  ~initialize_python()
  {
    logging::INFO("~initialize_python");
    Py_Finalize();
  }
};

class enable_threads
{
public:
  enable_threads()
  {
     logging::INFO("enable_threads");
    _state = PyEval_SaveThread();
  }

  ~enable_threads()
  {
    logging::INFO("~enable_threads");
    PyEval_RestoreThread(_state);
  }

private:
  PyThreadState* _state;
};

class ModelRecognition{
public:
  ModelRecognition(string hierarchal_clustering_path,string cluster_rec_path,string within_day_cluster_path,string home_setup_file, string time_window_config,bool& success);
  ~ModelRecognition();
  void destroy();
  void run();

private:
  void init_(string hierarchal_clustering_path,string cluster_rec_path,string with_day_cluster_path,string home_setup_file, string time_window_config,bool& success);
  bool includeOtherActivityClass_(int activity_index);
  void computeFeatures_(FeatureContainer* featureContainers);
  void leaveOneDayOutStrategy_(vector<FeatureContainer*> sensor_data, FeatureContainer* copy_clustered_sensor_data,PyInterpreterState* interp);
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
  vector<PythonRunner*> pythonRunnerContainer_;
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
