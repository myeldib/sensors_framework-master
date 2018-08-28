#include<evaluation.h>

/**
 * @brief Evaluation::Evaluation
 * @param evaluation_path
 * @param cluster_recognition_result_path
 * @param model_recognition_result_path
 * @param home_setup_file
 * @param time_window_config
 * @param success
 */

const int OTHER_ACTIVITY_LABEL_INDEX = 11;
Evaluation::Evaluation(string evaluation_path, string config_path, string cluster_recognition_result_path, string model_recognition_result_path, string home_setup_file, string time_window_config, bool &success)
{
  logging::INFO("Evaluation");
  evaluation_path_=evaluation_path;
  cluster_recognition_result_path_ = cluster_recognition_result_path;
  model_recognition_result_path_=model_recognition_result_path;
  config_path_ = config_path;

  init_(home_setup_file,time_window_config,success);

}

/**
 * @brief Evaluation::init_
 * @param home_setup_file
 * @param time_window_config
 * @param success
 */
void Evaluation::init_(string home_setup_file, string time_window_config, bool &success)
{
  logging::INFO("init_");

  home_ = new Home(home_setup_file,time_window_config);
  home_->readHomeSetup();

  string tmp_ext=std::to_string(home_->getHierarchalClusteringThreshold());
  std::replace(tmp_ext.begin(), tmp_ext.end(), '.', '_');


  featureWriter_ = new FeatureWriter(evaluation_path_,tmp_ext,this->home_,success);

  if(!success)
    {
      return;
    }

  featureReader_ = new FeatureReader(home_);

  pythonRunner_ = new PythonRunner(config_path_,home_setup_file,time_window_config);
  pythonRunner_->initPython();

  script_name_ ="evaluate";
  function_name1_ = "compute_accuracy";
  function_num_param1_ = 2;
  function_name2_ = "compute_confusion_matrix";
  function_num_param2_ = 4;

}

/**
 * @brief Evaluation::destroy
 */
void Evaluation::destroy()
{
  logging::INFO("destroy");
  delete this;
}
/**
 * @brief Evaluation::~Evaluation
 */
Evaluation::~Evaluation()
{
  logging::INFO("~Evaluation");

  if(featureReader_)
    {
      delete featureReader_;
    }

  if(featureWriter_)
    {
      delete featureWriter_;
    }

  if(pythonRunner_)
    {
      pythonRunner_->finalizePython();
      delete pythonRunner_;
    }

  if(home_)
    {
      delete home_;
    }
}
/**
 * @brief Evaluation::run
 */
void Evaluation::run()
{
  logging::INFO("run");
  evaluateClusterRecognition_(cluster_recognition_result_path_);
  evaluateModelRecognition_(model_recognition_result_path_);
}

/**
 * @brief Evaluation::removeOtherActivityLabels_
 * @param predicted_activity_labels
 * @param actual_activity_labels
 */
void Evaluation::removeOtherActivityLabels_(vector<int> &predicted_activity_labels, vector<int> &actual_activity_labels)
{
  logging::INFO("removeOtherActivityLabels_");

  vector<int> new_predicted_activity_labels;
  vector<int> new_actual_activity_labels;

  for(int i = 0; i <predicted_activity_labels.size();i++)
    {
      if(predicted_activity_labels[i] != OTHER_ACTIVITY_LABEL_INDEX && actual_activity_labels[i] != OTHER_ACTIVITY_LABEL_INDEX )
        {
          new_predicted_activity_labels.push_back(predicted_activity_labels[i]);
          new_actual_activity_labels.push_back(actual_activity_labels[i]);
        }
    }

  stringstream message;
  message<<"predicted_activity_labels_size:"<<predicted_activity_labels.size()<<"\t"<<"new_predicted_activity_labels_size:"<<new_predicted_activity_labels.size()<<endl;
  message<<"actual_activity_labels_size:"<<actual_activity_labels.size()<<"\t"<<"new_actual_activity_labels_size:"<<new_actual_activity_labels.size()<<endl;

  logging::INFO(message.str());

  predicted_activity_labels = new_predicted_activity_labels;
  actual_activity_labels = new_actual_activity_labels;

}
/**
 * @brief Evaluation::evaluateClusterRecognition_
 * @param result_path
 */
void Evaluation::evaluateClusterRecognition_(string path_to_labels)
{
  logging::INFO("evaluateClusterRecognition_");

  string tmp_ext=std::to_string(home_->getHierarchalClusteringThreshold());
  std::replace(tmp_ext.begin(), tmp_ext.end(), '.', '_');
  path_to_labels = path_to_labels+"_"+tmp_ext+"/";

  logging::INFO("path:"+path_to_labels);

  FeatureContainer* fc =featureReader_->readFeatures(path_to_labels,Constants::Cluster_Type::cluster_recognition)[0];
  string accuracy_info_with_other_activity= "";
  string accuracy_info_without_other_activity= "";
  string path_to_results="";

  vector<int> actual_cluster_activity_labels= fc->getActualActivityLabels();
  vector<int> predicted_cluster_activity_labels = fc->getPredictedActivityLabels();

  //computer accuracy with other activity labels
  pythonRunner_->computeAccuracy(script_name_,function_name1_,function_num_param1_,actual_cluster_activity_labels,predicted_cluster_activity_labels,accuracy_info_with_other_activity);

  //write info to file
  fc->setAccuracyResultsMessage(accuracy_info_with_other_activity);
  featureWriter_->writeEvaluationResults(fc,Constants::Cluster_Type::cluster_recognition,Constants::Result_Type::with_other_activity_label,path_to_results);

  //compute confusion matrix with other activity labels
  pythonRunner_->generateConfusionMatrix(script_name_,function_name2_,function_num_param2_,actual_cluster_activity_labels,predicted_cluster_activity_labels,path_to_results,Constants::Result_Type::with_other_activity_label);

  //remove other activity labels
  removeOtherActivityLabels_(predicted_cluster_activity_labels,actual_cluster_activity_labels);

  //compute accuracy without other activity labels
  pythonRunner_->computeAccuracy(script_name_,function_name1_,function_num_param1_,actual_cluster_activity_labels,predicted_cluster_activity_labels,accuracy_info_without_other_activity);

  //write info to file
  fc->setAccuracyResultsMessage(accuracy_info_without_other_activity);
  featureWriter_->writeEvaluationResults(fc,Constants::Cluster_Type::cluster_recognition,Constants::Result_Type::without_other_activity_label,path_to_results);

  //compute confusion matrix without other activity labels
  pythonRunner_->generateConfusionMatrix(script_name_,function_name2_,function_num_param2_,actual_cluster_activity_labels,predicted_cluster_activity_labels,path_to_results,Constants::Result_Type::without_other_activity_label);
}

/**
 * @brief Evaluation::evaluateModelRecognition_
 * @param result_path
 */
void Evaluation::evaluateModelRecognition_(string path_to_labels)
{
  logging::INFO("evaluateModelRecognition_");

  string tmp_ext=std::to_string(home_->getHierarchalClusteringThreshold());
  std::replace(tmp_ext.begin(), tmp_ext.end(), '.', '_');
  path_to_labels = path_to_labels+"_"+tmp_ext+"/";

  logging::INFO("path:"+path_to_labels);

  FeatureContainer* fc =featureReader_->readFeatures(path_to_labels,Constants::Cluster_Type::model_recognition)[0];
  string accuracy_info_with_other_activity= "";
  string accuracy_info_without_other_activity= "";
  string path_to_results="";

  vector<int> actual_cluster_activity_labels= fc->getActualActivityLabels();
  vector<int> predicted_cluster_activity_labels = fc->getPredictedActivityLabels();

  //compute accuracy with other activity labels
  pythonRunner_->computeAccuracy(script_name_,function_name1_,function_num_param1_,actual_cluster_activity_labels,predicted_cluster_activity_labels,accuracy_info_with_other_activity);

  //write info to file
  fc->setAccuracyResultsMessage(accuracy_info_with_other_activity);
  featureWriter_->writeEvaluationResults(fc,Constants::Cluster_Type::model_recognition,Constants::Result_Type::with_other_activity_label,path_to_results);

  //compute confusion matrix with other activity labels
  pythonRunner_->generateConfusionMatrix(script_name_,function_name2_,function_num_param2_,actual_cluster_activity_labels,predicted_cluster_activity_labels,path_to_results,Constants::Result_Type::with_other_activity_label);

  //remove other activity labels
  removeOtherActivityLabels_(predicted_cluster_activity_labels,actual_cluster_activity_labels);

  //compute accuracy without other activity labels
  pythonRunner_->computeAccuracy(script_name_,function_name1_,function_num_param1_,actual_cluster_activity_labels,predicted_cluster_activity_labels,accuracy_info_without_other_activity);

  //write info to file
  fc->setAccuracyResultsMessage(accuracy_info_without_other_activity);
  featureWriter_->writeEvaluationResults(fc,Constants::Cluster_Type::model_recognition,Constants::Result_Type::without_other_activity_label,path_to_results);

  //compute confusion matrix without other activity labels
  pythonRunner_->generateConfusionMatrix(script_name_,function_name2_,function_num_param2_,actual_cluster_activity_labels,predicted_cluster_activity_labels,path_to_results,Constants::Result_Type::without_other_activity_label);

}
