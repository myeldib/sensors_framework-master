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
  PythonRunner(string data_dir);
  ~PythonRunner();
  void predictUsingModel(string script_name,
                         string function_name,
                         int num_function_arg,
                         FeatureContainer* train_sensor_data,
                         FeatureContainer* test_sensor_data,
                         vector<float>& predicted_activity_labels);
private:
  void init_();
  void makeNumpyArray_(size_t rows,
                       size_t cols,
                       const float* data,
                       PyObject* array);
  void convert2DVectorToCvMat_(vector<vector<float> >& vec,
                               cv::Mat& mat);
  void predictUsingModel_(string script_name,
                          string function_name,
                          int num_function_arg,
                          FeatureContainer* train_sensor_data,
                          FeatureContainer* test_sensor_data,
                          vector<float>& predict_activity_labels);
  void prepareTrainData_(FeatureContainer* train_sensor_data,vector<vector<float> >& train_features);
  void prepareTestData_(FeatureContainer* test_sensor_data,vector<vector<float> >& test_features);
  void useModel_(string script_name,
                 string function_name,
                 int num_function_arg,
                 PyObject* train_features_py_obj,
                 PyObject* test_features_py_obj,
                 vector<float> &predict_activity_labels,
                 bool &success);

private:
  string data_dir;

};
#endif // PYTHONRUNNER_H
