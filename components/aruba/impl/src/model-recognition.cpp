#include "model-recognition.h"

/**
 * @brief ModelRecognition::ModelRecognition
 * @param hierarchal_clustering_path
 * @param cluster_rec_path
 * @param home_setup_file
 * @param time_window_config
 * @param success
 */
ModelRecognition::ModelRecognition(string hierarchal_clustering_path, string config_path,string cluster_rec_path,string with_day_cluster_path, string home_setup_file, string time_window_config, bool &success)
{

  logging::INFO("ModelRecognition");

  init_(hierarchal_clustering_path,config_path,cluster_rec_path,with_day_cluster_path,home_setup_file,time_window_config,success);

  if(!success)
    {
      return;
    }
}

/**
 * @brief ModelRecognition::init_
 * @param hierarchal_clustering_path
 * @param cluster_rec_path
 * @param home_setup_file
 * @param time_window_config
 * @param success
 */
void ModelRecognition::init_(string hierarchal_clustering_path,string config_path,string cluster_rec_path, string within_day_cluster_path,string home_setup_file, string time_window_config, bool &success)
{

  logging::INFO("init_");

  num_threads_=1;// boost::thread::hardware_concurrency();

  home_ = new Home(home_setup_file,time_window_config);
  home_->readHomeSetup();

  string tmp_ext=std::to_string(home_->getHierarchalClusteringThreshold());
  std::replace(tmp_ext.begin(), tmp_ext.end(), '.', '_');


  cluster_rec_path_=cluster_rec_path;
  featureWriter_ = new FeatureWriter(this->cluster_rec_path_,tmp_ext,this->home_,success);

  if(!success)
    {
      return;
    }

  featureReader_ = new FeatureReader(this->home_);
  featureProcessor_ = new FeatureProcessor(this->home_);
  similarityMeasure_ = new SimilarityMeasure(home_setup_file,time_window_config);

  hierarchal_clustering_path=hierarchal_clustering_path+
      "_"+
      tmp_ext+
      "/";

  //after processing all days using between day clustering,
  //the results are expected to be in a single feature container
  for(int i =0; i<num_threads_;i++)
    {
      //set thread id, to be used to build train data for each thread
      FeatureContainer* tmp = featureReader_->readFeatures(hierarchal_clustering_path,Constants::between_day_cluster)[0];
      tmp->setThreadID(i);
      copy_clustered_sensor_data.push_back(tmp);
    }

  pythonRunner_ = new PythonRunner(config_path,home_setup_file,time_window_config);
  pythonRunner_->initPython();


  within_day_cluster_path.append("_").append(std::to_string(home_->getShortDurationLambda())).append("_").append(std::to_string(home_->getLongDurationLambda())).append("/");
  within_day_cluster_path_=within_day_cluster_path;

  sensor_data = featureReader_->readFeatures(within_day_cluster_path,Constants::within_day_cluster);
  merged_sub_containers = new FeatureContainer();

  sorterProcessor_ = new SorterProcessor();

  script_name_="train_model";
  function_name1_ ="train_and_predict";
  function_name2_ ="compute_accuracy";
  function_num_param_ = 2;

  logging::INFO("features_discovered_patterns:"+std::to_string(copy_clustered_sensor_data[0]->getDiscoveredPatterns().size()));
  logging::INFO("sensor_data_size:"+std::to_string(sensor_data.size()));

}

/**
 * @brief ModelRecognition::destroy
 */
void ModelRecognition::destroy()
{
  delete this;
}

/**
 * @brief ModelRecognition::~ModelRecognition
 */
ModelRecognition::~ModelRecognition()
{
  logging::INFO("~ModelRecognition");


  for(int i =0; i<copy_clustered_sensor_data.size(); i++)
    {
      if(copy_clustered_sensor_data[i])
        {
          delete copy_clustered_sensor_data[i];
        }
    }

  for(int i = 0; i<sensor_data.size(); i++)
    {
      if(sensor_data[i])
        {
          delete sensor_data[i];
        }
    }

  if(merged_sub_containers)
    {
      delete merged_sub_containers;
    }

  if(featureProcessor_)
    {
      delete featureProcessor_;
    }

  if(featureReader_)
    {
      delete featureReader_;
    }

  if(similarityMeasure_)
    {
      delete similarityMeasure_;
    }

  if(featureWriter_)
    {
      delete featureWriter_;
    }

  for(int i =0; i<sub_interpreters_vec_.size();i++)
    {
      if(sub_interpreters_vec_[i])
        {
          delete sub_interpreters_vec_[i];
        }
    }

  if(pythonRunner_)
    {
      delete pythonRunner_;
    }

  if(sorterProcessor_)
    {
      delete sorterProcessor_;
    }

}

/**
 * @brief ModelRecognition::run
 */
void ModelRecognition::run()
{
  logging::INFO("run");

  computeSubContainersClusters_(sensor_data,copy_clustered_sensor_data, merged_sub_containers,num_threads_);
  evaluate_(merged_sub_containers);
  writePredictions_(merged_sub_containers);
}

/**
 * @brief ModelRecognition::computeSubContainersClusters_
 * @param sensor_data
 * @param clustered_sensor_data
 * @param num_threads
 */
void ModelRecognition::computeSubContainersClusters_(vector<FeatureContainer *> &sensor_data, vector<FeatureContainer*>& clustered_sensor_data, FeatureContainer* merged_sub_containers,int &num_threads)
{
  logging::INFO("computeSubContainersClusters_");

  boost::thread_group thread_group_;
  //initialize sub containers
  vector<vector<FeatureContainer*> > subFeatureContainers;

  for(int i = 0 ; i<num_threads;i++)
    {
      vector<FeatureContainer*> tmp;
      subFeatureContainers.push_back(tmp);

    }


  divideContainerToSubContainers_(sensor_data,num_threads,subFeatureContainers);

  //threading to computeContainerClusters


  for(int i =0; i<num_threads;i++)
    {
      sub_interpreter* s = new sub_interpreter();
      sub_interpreters_vec_.push_back(s);
    }

  for(int i =0;i<num_threads;i++)
    {
      vector<FeatureContainer*> testSubFeatureContainers = subFeatureContainers[i];
      FeatureContainer* copy_clustered_sensor_data= clustered_sensor_data[i];
      PyInterpreterState* interp= sub_interpreters_vec_[i]->interp();

      thread_group_.add_thread(new boost::thread(&ModelRecognition::leaveOneDayOutStrategy_,this,testSubFeatureContainers,copy_clustered_sensor_data,interp));
    }

  enable_threads enable_threads_scope;

  thread_group_.join_all();

  mergeSubContainersToContainer_(clustered_sensor_data,merged_sub_containers);

}

/**
 * @brief ModelRecognition::mergeSubContainersToContainer_
 * @param copy_clustered_sensor_data
 * @param mergedSubFeatureContainer
 */
void ModelRecognition::mergeSubContainersToContainer_(vector<FeatureContainer *> &copy_clustered_sensor_data, FeatureContainer *mergedSubFeatureContainer)
{
  logging::INFO("mergeSubContainersToContainer_");

  vector<int> predicted_activity_labels;
  vector<int> actual_activity_labels;


  for(int i =0 ; i<copy_clustered_sensor_data.size();i++)
    {
      FeatureContainer* fc = copy_clustered_sensor_data[i];

      for(int j =0; j<fc->getPredictedActivityLabels().size();j++)
        {
          predicted_activity_labels.push_back(fc->getPredictedActivityLabels()[j]);
          actual_activity_labels.push_back(fc->getActualActivityLabels()[j]);

        }
    }

  mergedSubFeatureContainer->setPredictedActivityLabels(predicted_activity_labels);
  mergedSubFeatureContainer->setActualActivityLabels(actual_activity_labels);

  stringstream message;
  message<<"predicted_activity_labels:"<<predicted_activity_labels.size()<<"\t"
        <<"actual_activity_labels:"<<actual_activity_labels.size()<<endl;

  logging::INFO(message.str());


}

/**
 * @brief ModelRecognition::divideContainerToSubContainers_
 * @param sensor_data
 * @param clustered_sensor_data
 * @param num_threads
 * @param subFeatureContainers
 */
void ModelRecognition::divideContainerToSubContainers_(vector<FeatureContainer *> &sensor_data,int& num_threads,vector<vector<FeatureContainer*> >& subFeatureContainers)
{
  logging::INFO("divideContainerToSubContainers_");

  vector<int> num_elements_per_containers;
  Common::computeNumElementsPerThread(sensor_data.size(),num_threads,num_elements_per_containers);

  int from=0;
  int to = num_elements_per_containers[0];

  for(int i =0; i<num_elements_per_containers.size();i++)
    {
      logging::INFO("from:"+
                    std::to_string(from)+
                    "\t"+
                    "to:"+
                    std::to_string(to));


      divideContainer_(i,from,to,sensor_data,subFeatureContainers[i]);

      //update ranges
      from = to;
      to+=num_elements_per_containers[i+1];
    }
}

/**
 * @brief ModelRecognition::divideContainer_
 * @param from
 * @param to
 * @param sensor_data
 * @param subFeatureContainer
 */
void ModelRecognition::divideContainer_(int thread_id, int from, int to, vector<FeatureContainer *> &sensor_data, vector<FeatureContainer *> &subFeatureContainer)
{
  logging::INFO("divideContainer_");

  vector<FeatureContainer*> tmpSubFeatureContainer;
  //copy data to sub container
  for(int i = from; i< to; i++)
    {
      //set thread id, to be used to build test data for each thread
      sensor_data[i]->setThreadID(thread_id);
      tmpSubFeatureContainer.push_back(sensor_data[i]);
    }

  subFeatureContainer = tmpSubFeatureContainer;

}

/**
 * @brief ModelRecognition::prepareTrainingData_
 * @param test_day
 * @param clustered_sensor_data
 * @param train_sensor_data
 */
void ModelRecognition::prepareTrainingData_(string test_day, FeatureContainer *clustered_sensor_data, FeatureContainer *train_sensor_data)
{
  logging::INFO("prepareTrainingData_");

  vector<string> clustered_day_names = clustered_sensor_data->getDayNamePerPattern();
  vector<int> clustered_sequence_patterns = clustered_sensor_data->getSequencePatterns();
  vector<vector<float> > clustered_sensor_duration = clustered_sensor_data->getSensorDurations();
  vector<float> clustered_time_index = clustered_sensor_data->getTimeIndexPerPattern();
  vector<string> clustered_activity_labels = clustered_sensor_data->getActivityLabel();


  //include all data except the data related to test day
  vector<int> train_sequence_patterns;
  vector<vector<float> > train_sensor_durations;
  vector<float> train_time_index;
  vector<string> train_days_names;
  vector<string> train_activity_labels;
  vector<int> train_discovered_patterns;

  cout<<test_day<<endl;

  for(int i =0;i<clustered_sequence_patterns.size();i++)
    {
      if(clustered_day_names[i]!=test_day)
        {

          train_sequence_patterns.push_back(clustered_sequence_patterns[i]);
          train_sensor_durations.push_back(clustered_sensor_duration[i]);
          train_time_index.push_back(clustered_time_index[i]);
          train_days_names.push_back(clustered_day_names[i]);
          train_activity_labels.push_back(clustered_activity_labels[i]);
          //include all sequence patterns in discovered patterns
          train_discovered_patterns.push_back(clustered_sequence_patterns[i]);
        }
    }

  //remove duplicates from discovered patterns
  set<int> train_discovered_patterns_set( train_discovered_patterns.begin(), train_discovered_patterns.end() );
  train_discovered_patterns.assign( train_discovered_patterns_set.begin(), train_discovered_patterns_set.end() );


  train_sensor_data->setSequencePatterns(train_sequence_patterns);
  train_sensor_data->setSensorDurations(train_sensor_durations);
  train_sensor_data->setTimeIndexPerPattern(train_time_index);
  train_sensor_data->setDayNamePerPattern(train_days_names);
  train_sensor_data->setActivityLabel(train_activity_labels);
  train_sensor_data->setDiscoveredPatterns(train_discovered_patterns);
  train_sensor_data->setThreadID(clustered_sensor_data->getThreadID());

  computeFeatures_(train_sensor_data);

}


/**
 * @brief ModelRecognition::evaluate_
 * @param fc
 */
void ModelRecognition::evaluate_(FeatureContainer *fc)
{
  logging::INFO("evaluate_");
  vector<int> targets = fc->getActualActivityLabels();
  vector<int> outputs = fc->getPredictedActivityLabels();
  string accuracy_info;

  cout<<targets.size()<<"\t"<<outputs.size()<<endl;

  pythonRunner_->computeAccuracy(script_name_,function_name2_,function_num_param_,targets,outputs,accuracy_info);

  fc->setAccuracyResultsMessage(accuracy_info);

}


/**
 * @brief ModelRecognition::writePredictions_
 * @param clustered_sensor_data
 */
void ModelRecognition::writePredictions_(FeatureContainer *clustered_sensor_data)
{

  logging::INFO("writePredictions_");
  featureWriter_->writeFeatures(clustered_sensor_data,Constants::Cluster_Type::model_recognition);
}

/**
 * @brief ModelRecognition::leaveOneDayOutStrategy_
 * @param sensor_data
 * @param clustered_sensor_data
 */
void ModelRecognition::leaveOneDayOutStrategy_(vector<FeatureContainer *> sensors_data, FeatureContainer *clustered_sensor_data,PyInterpreterState* interp)
{
  logging::INFO("leaveOneDayOutStrategy_");

  vector<int> predicted_activity_label;
  vector<int> actual_activity_label;

  for(int i=0; i<sensors_data.size();i++)
    {
      FeatureContainer* test_sensor_data =sensors_data[i];

      //compute active sensor using duration not avg duration
      test_sensor_data->setAverageSensorDurationsPerPattern(test_sensor_data->getSensorDurations());
      featureProcessor_->computeActiveSensors(test_sensor_data,true);


      FeatureContainer* train_sensor_data = new FeatureContainer();
      prepareTrainingData_(test_sensor_data->getDayNamePerPattern()[0],clustered_sensor_data,train_sensor_data);

      string tmp_script_name=script_name_;
      string tmp_function_name1=function_name1_;
      int tmp_function_num_param =function_num_param_;

      pythonRunner_->predictUsingModel(tmp_script_name,tmp_function_name1,tmp_function_num_param,train_sensor_data,test_sensor_data,actual_activity_label,predicted_activity_label,interp);

      delete train_sensor_data;

    }

  clustered_sensor_data->setPredictedActivityLabels(predicted_activity_label);
  clustered_sensor_data->setActualActivityLabels(actual_activity_label);

}
/**
 * @brief ModelRecognition::computeFeatures_
 * @param featureContainers
 */
void ModelRecognition::computeFeatures_(FeatureContainer *featureContainer)
{
  logging::INFO("computeFeatures_");

  sorterProcessor_->radixSort(featureContainer);

  featureProcessor_->computeOptimizedAverageSensorDurationPerPattern(featureContainer);
  featureProcessor_->computeOptimizedMostCommonActivityLabelPerPattern(featureContainer);
  featureProcessor_->computeActiveSensors(featureContainer,true);
  featureProcessor_->computeOptimizedMostAssignedTimeIndex(featureContainer);

}
