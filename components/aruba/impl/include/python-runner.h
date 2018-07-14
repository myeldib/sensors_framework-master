#ifndef PYTHONRUNNER_H
#define PYTHONRUNNER_H
#include <feature-container.h>
#include <home.h>
#include <Python.h>
#include <numpy/arrayobject.h>
#include <opencv2/opencv.hpp>
#include<logger.h>

class PythonRunner{

public:
  PythonRunner(string data_dir,string home_setup_file, string time_window_config);
  ~PythonRunner();
  void predictUsingModel(string script_name,
                         string function_name,
                         int num_function_arg,
                         FeatureContainer* train_sensor_data,
                         FeatureContainer* test_sensor_data,
                         vector<int>& actual_activity_label,
                         vector<int>& predicted_activity_labels);
  void computeAccuracy(string script_name,
                       string function_name,
                       int num_function_arg,
                       vector<int>& actual_activity_labels,
                       vector<int>& predicted_activity_labels,
                       string& accuracy_info);
  void initPython();
  void finalizePython();
private:
  void init_(string home_setup_file, string time_window_config);
  void makeNumpyArray_(size_t rows,
                       size_t cols,
                       const float* data,
                       PyObject*& array);
  void convert2DVectorToCvMat_(vector<vector<float> >& vec,
                               cv::Mat& mat);
  void convert1DVectorToCvMat_(vector<int>& vec,
                               cv::Mat& mat);
  void predictUsingModel_(string script_name,
                          string function_name,
                          int num_function_arg,
                          FeatureContainer* train_sensor_data,
                          FeatureContainer* test_sensor_data,
                          vector<int>& actual_activity_label,
                          vector<int>& predict_activity_labels);
  void prepareTrainData_(FeatureContainer* train_sensor_data,vector<vector<float> >& train_features);
  void prepareTestData_(FeatureContainer* test_sensor_data,vector<int>& actual_activity_label,vector<vector<float> >& test_features);
  void useModelFunction_(string script_name,
                 string function_name,
                 int num_function_arg,
                 PyObject* train_features_py_obj,
                 PyObject* test_features_py_obj,
                 vector<int> &predict_activity_labels,
                 int thread_id,
                 bool &success);
  void UseAccuracyFucntion_(string script_name,
                            string function_name,
                            int num_function_arg,
                            PyObject* actual_labels_py_obj,
                            PyObject* predicted_activity_py_obj,
                            string& accuracy_info,
                            bool &success);
  void computeAccuracy_(string script_name,
                        string function_name,
                        int num_function_arg,
                        vector<int>& actual_activity_labels,
                        vector<int>& predicted_activity_labels,
                        string& accuracy_info,
                        bool& success);

private:
  string data_dir;
  Home* home_;

};
#endif // PYTHONRUNNER_H
