#ifndef EVALUATION_H
#define EVALUATION_H
#include<utilities.h>
#include<home.h>
#include<feature-writer.h>
#include<feature-reader.h>
#include<python-runner.h>
#include<logger.h>
class Evaluation {
public:
  Evaluation(string evaluation_path, string config_path,string cluster_recognition_result_path, string model_recognition_result_path, string home_setup_file, string time_window_config,bool& success);
  ~Evaluation();
  void run();
  void destroy();
private:
  void init_(string home_setup_file, string time_window_config, bool &success);
  void evaluateModelRecognition_(string result_path);
  void evaluateClusterRecognition_(string result_path);
  void writeEvaluationResult_(string result_path);
  void removeOtherActivityLabels_(vector<int>& predicted_activity_labels, vector<int>& actual_activity_labels);
private:
  string cluster_recognition_result_path_;
  string model_recognition_result_path_;
  string evaluation_path_;
  string config_path_;

  //python script info
  string script_name_;
  string function_name1_;
  string function_name2_;
  int function_num_param1_;
  int function_num_param2_;

  Home* home_;
  PythonRunner* pythonRunner_;
  FeatureWriter* featureWriter_;
  FeatureReader* featureReader_;

};
#endif // EVALUATION_H
