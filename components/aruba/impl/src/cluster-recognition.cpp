#include "cluster-recognition.h"

/**
 * @brief ClusterRecognition::ClusterRecognition
 * @param hierarchal_clustering_path
 * @param cluster_rec_path
 * @param home_setup_file
 * @param time_window_config
 * @param success
 */
ClusterRecognition::ClusterRecognition(string hierarchal_clustering_path, string config_path, string cluster_rec_path,string with_day_cluster_path, string home_setup_file, string time_window_config, bool &success)
{

  logging::INFO("ClusterRecognition");

  init_(hierarchal_clustering_path, config_path, cluster_rec_path,with_day_cluster_path,home_setup_file,time_window_config,success);

  if(!success)
    {
      return;
    }
}

/**
 * @brief ClusterRecognition::init_
 * @param hierarchal_clustering_path
 * @param cluster_rec_path
 * @param home_setup_file
 * @param time_window_config
 * @param success
 */
void ClusterRecognition::init_(string hierarchal_clustering_path, string config_path, string cluster_rec_path, string within_day_cluster_path,string home_setup_file, string time_window_config, bool &success)
{
  logging::INFO("init_");

  num_threads_=boost::thread::hardware_concurrency();

  home_ = new Home(home_setup_file,time_window_config);
  home_->readHomeSetup();

  sorterProcessor_= new SorterProcessor();

  string tmp_ext=std::to_string(home_->getHierarchalClusteringThreshold());
  std::replace(tmp_ext.begin(), tmp_ext.end(), '.', '_');


  cluster_rec_path_=cluster_rec_path;
  featureWriter_ = new FeatureWriter(this->cluster_rec_path_,tmp_ext,home_,success);

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
      copy_clustered_sensor_data.push_back(featureReader_->readFeatures(hierarchal_clustering_path,Constants::between_day_cluster)[0]);
    }

  within_day_cluster_path.append("_").append(std::to_string(home_->getShortDurationLambda())).append("_").append(std::to_string(home_->getLongDurationLambda())).append("/");
  within_day_cluster_path_=within_day_cluster_path;

  sensor_data = featureReader_->readFeatures(within_day_cluster_path,Constants::within_day_cluster);
  merged_sub_containers = new FeatureContainer();

  pythonRunner_ = new PythonRunner(config_path,home_setup_file,time_window_config);
  pythonRunner_->initPython();

  script_name_="train_model";
  function_name1_ ="compute_accuracy";
  function_num_param_= 2;


  logging::INFO("features_discovered_patterns:"+std::to_string(copy_clustered_sensor_data[0]->getDiscoveredPatterns().size()));
  logging::INFO("sensor_data_size:"+std::to_string(sensor_data.size()));

}

/**
 * @brief ClusterRecognition::destroy
 */
void ClusterRecognition::destroy()
{
  delete this;
}

/**
 * @brief ClusterRecognition::~ClusterRecognition
 */
ClusterRecognition::~ClusterRecognition()
{
  logging::INFO("~ClusterRecognition");


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

  if(pythonRunner_)
    {
      pythonRunner_->finalizePython();
      delete pythonRunner_;
    }

}

/**
 * @brief ClusterRecognition::run
 */
void ClusterRecognition::run()
{
  logging::INFO("run");

  computeSubContainersClusters_(sensor_data,copy_clustered_sensor_data, merged_sub_containers,num_threads_);
  evaluate_(merged_sub_containers);
  writePredictions_(merged_sub_containers);
}

/**
 * @brief ClusterRecognition::computeSubContainersClusters_
 * @param sensor_data
 * @param clustered_sensor_data
 * @param num_threads
 */
void ClusterRecognition::computeSubContainersClusters_(vector<FeatureContainer *> &sensor_data, vector<FeatureContainer*>& clustered_sensor_data, FeatureContainer* merged_sub_containers,int &num_threads)
{
  logging::INFO("computeSubContainersClusters_");

  //initialize sub containers
  vector<vector<FeatureContainer*> > subFeatureContainers;

  for(int i = 0 ; i<num_threads;i++)
    {
      vector<FeatureContainer*> tmp;
      subFeatureContainers.push_back(tmp);

    }


  divideContainerToSubContainers_(sensor_data,num_threads,subFeatureContainers);

  //threading to computeContainerClusters
  for(int i =0;i<num_threads;i++)
    {
      vector<FeatureContainer*> testSubFeatureContainers = subFeatureContainers[i];
      FeatureContainer* copy_clustered_sensor_data= clustered_sensor_data[i];

      thread_group_.add_thread(new boost::thread([testSubFeatureContainers,copy_clustered_sensor_data, this] { leaveOneDayOutStrategy_(testSubFeatureContainers,copy_clustered_sensor_data); }));
    }

  thread_group_.join_all();

  mergeSubContainersToContainer_(clustered_sensor_data,merged_sub_containers);

}

/**
 * @brief ClusterRecognition::mergeSubContainersToContainer_
 * @param copy_clustered_sensor_data
 * @param mergedSubFeatureContainer
 */
void ClusterRecognition::mergeSubContainersToContainer_(vector<FeatureContainer *> &copy_clustered_sensor_data, FeatureContainer *mergedSubFeatureContainer)
{
  logging::INFO("mergeSubContainersToContainer_");

  vector<int> predicted_activity_labels;
  vector<int> actual_activity_labels;
  vector<int> predicted_discovered_patterns;

  for(int i =0 ; i<copy_clustered_sensor_data.size();i++)
    {
      FeatureContainer* fc = copy_clustered_sensor_data[i];

      for(int j =0; j<fc->getPredictedActivityLabels().size();j++)
        {
          predicted_activity_labels.push_back(fc->getPredictedActivityLabels()[j]);
          actual_activity_labels.push_back(fc->getActualActivityLabels()[j]);
          predicted_discovered_patterns.push_back(fc->getPredictedDiscoveredPatterns()[j]);

        }
    }

  mergedSubFeatureContainer->setPredictedActivityLabels(predicted_activity_labels);
  mergedSubFeatureContainer->setActualActivityLabels(actual_activity_labels);
  mergedSubFeatureContainer->setPredictedDiscoveredPatterns(predicted_discovered_patterns);

  stringstream message;
  message<<"predicted_activity_labels:"<<predicted_activity_labels.size()<<"\t"
        <<"actual_activity_labels:"<<actual_activity_labels.size()<<"\t"
       <<"predicted_discovered_patterns:"<<predicted_discovered_patterns.size()<<endl;

  logging::INFO(message.str());


}

/**
 * @brief ClusterRecognition::divideContainerToSubContainers_
 * @param sensor_data
 * @param clustered_sensor_data
 * @param num_threads
 * @param subFeatureContainers
 */
void ClusterRecognition::divideContainerToSubContainers_(vector<FeatureContainer *> &sensor_data,int& num_threads,vector<vector<FeatureContainer*> >& subFeatureContainers)
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


      divideContainer_(from,to,sensor_data,subFeatureContainers[i]);

      //update ranges
      from = to;
      to+=num_elements_per_containers[i+1];
    }
}

/**
 * @brief ClusterRecognition::divideContainer_
 * @param from
 * @param to
 * @param sensor_data
 * @param subFeatureContainer
 */
void ClusterRecognition::divideContainer_(int from, int to, vector<FeatureContainer *> &sensor_data, vector<FeatureContainer *> &subFeatureContainer)
{
  logging::INFO("divideContainer_");

  vector<FeatureContainer*> tmpSubFeatureContainer;
  //copy data to sub container
  for(int i = from; i< to; i++)
    {
      tmpSubFeatureContainer.push_back(sensor_data[i]);
    }

  subFeatureContainer = tmpSubFeatureContainer;

}

/**
 * @brief ClusterRecognition::computeClusterPurity_
 * @param activity_label
 * @param most_common_activity_count_cluster
 * @param all_activity_count_cluster
 */
void ClusterRecognition::computeClusterPurity_(int most_selected_activity_index, vector<int> activity_labels, vector<int> &most_common_activity_count_cluster, vector<int> &all_activity_count_cluster)
{

  for(int j = 0;j<activity_labels.size();j++)
    {
      if(j ==most_selected_activity_index &&  includeOtherActivityClass_(j))
        {
          most_common_activity_count_cluster.push_back(activity_labels[j]);
        }

      if(includeOtherActivityClass_(j))
        {
          all_activity_count_cluster.push_back(activity_labels[j]);
        }
      cout<<activity_labels[j]<<"\t";
    }


  cout<<endl;
}

/**
 * @brief ClusterRecognition::includeOtherActivityClass_
 * @param activity_index
 * @return
 */
bool ClusterRecognition::includeOtherActivityClass_(int activity_index)
{
  if(home_->getIncludeOtherActivityClass())
    {
      return true;
    }
  else
    {
      if(activity_index !=home_->getActivityLabelStringIntMap().at(""))
        {
          return true;
        }
      else
        {
          return false;
        }
    }

}
/**
 * @brief ClusterRecognition::computeClustersPurity_
 * @param featureContainers
 * @param purity
 */
void ClusterRecognition::computeClustersPurity_(FeatureContainer *featureContainers, float& purity)
{

  logging::INFO("computeClustersPurity_");

  vector<int> discovered_patterns = featureContainers->getDiscoveredPatterns();
  vector<int> sequence_patterns = featureContainers->getSequencePatterns();
  vector<vector<int> > activity_labels_per_pattern = featureContainers->getActivityLabelsPerPattern();
  vector<int> most_select_activity = featureContainers->getMostCommonActivityLabelPerPattern();


  vector<int> most_common_activity_count_cluster;
  vector<int> all_activity_count_cluster;

  for(int i =0 ;i<activity_labels_per_pattern.size();i++)
    {
      cout<<discovered_patterns[i]<<"\t"<<most_select_activity[i]<<"\t";
      computeClusterPurity_(most_select_activity[i],activity_labels_per_pattern[i],most_common_activity_count_cluster,all_activity_count_cluster);
    }

  int sum_labels=  std::accumulate(all_activity_count_cluster.begin(), all_activity_count_cluster.end(), 0);
  int sum_clusters=  std::accumulate(most_common_activity_count_cluster.begin(), most_common_activity_count_cluster.end(), 0);

  logging::INFO("activity_labels_per_pattern_size:"+std::to_string(activity_labels_per_pattern.size())+"\t"+
                 "sequence_patterns_size:"+std::to_string(sequence_patterns.size())+"\t"+
                 "discovered_patterns_size:"+std::to_string(discovered_patterns.size()));

  purity=(sum_clusters*1.0)/(sum_labels*1.0);
  logging::INFO("purity:"+std::to_string(purity));

}

/**
 * @brief ClusterRecognition::prepareTrainingData_
 * @param test_day
 * @param clustered_sensor_data
 * @param train_sensor_data
 */
void ClusterRecognition::prepareTrainingData_(string test_day, FeatureContainer *clustered_sensor_data, FeatureContainer *train_sensor_data)
{
  logging::INFO("prepareTrainingData_");

  vector<string> clustered_day_names = clustered_sensor_data->getDayNamePerPattern();
  vector<int> clustered_sequence_patterns = clustered_sensor_data->getSequencePatterns();
  vector<int> clustered_discovered_patterns = clustered_sensor_data->getDiscoveredPatterns();
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

  computeFeatures_(train_sensor_data);

}

/**
 * @brief ClusterRecognition::printDurationTwoPatterns
 * @param sensor_durations1
 * @param sensor_durations2
 */
void ClusterRecognition::printDurationTwoPatterns_(vector<float> sensor_durations1, vector<float> sensor_durations2)
{
  for(int i =0; i<sensor_durations1.size();i++)
    {
      stringstream message;
      message<<sensor_durations1[i]<<":"<<sensor_durations2[i]<<endl;

      logging::INFO(message.str());
    }
}

/**
 * @brief ClusterRecognition::checkPredictedActivityLabel_
 * @param predicted_activity
 * @param activity_per_pattern
 */
void ClusterRecognition::checkPredictedActivityLabel_(int &predicted_activity,string& actual_activity_label, vector<int> activity_per_pattern)
{
  if(actual_activity_label=="-")   actual_activity_label="";

  int other_activity_class_index =home_->getActivityLabelStringIntMap().at("");
  int actual_activity_index = home_->getActivityLabelStringIntMap().at(actual_activity_label);

  if(predicted_activity ==other_activity_class_index && actual_activity_index!=other_activity_class_index)
    {
      //we do not want to select other activity class if it is the max
      activity_per_pattern[other_activity_class_index] =0;

      //find max similarity value
      auto max_activity_value = std::max_element(std::begin(activity_per_pattern), std::end(activity_per_pattern));
      int max_activity_index=std::distance(std::begin(activity_per_pattern), max_activity_value);

      predicted_activity = max_activity_index;
    }
}

/**
 * @brief ClusterRecognition::evaluate_
 * @param fc
 */
void ClusterRecognition::evaluate_(FeatureContainer *fc)
{
  logging::INFO("evaluate_");
  vector<int> targets = fc->getActualActivityLabels();
  vector<int> outputs = fc->getPredictedActivityLabels();
  string accuracy_info="";
  float purity = 0.0;


  computeClustersPurity_(copy_clustered_sensor_data[0],purity);

  logging::INFO("purity:"+std::to_string(purity));
  accuracy_info.append("Cluster Purity:"+std::to_string(purity));
  accuracy_info.append("\n\n");

  logging::INFO("targets_size:"+std::to_string(targets.size())+"\t"+
                "outputs_size:"+std::to_string(outputs.size()));

  pythonRunner_->computeAccuracy(script_name_,function_name1_,function_num_param_,targets,outputs,accuracy_info);

  fc->setAccuracyResultsMessage(accuracy_info);
}
/**
 * @brief ClusterRecognition::recognize_
 * @param test_sensor_data
 * @param train_sensor_data
 * @param actual_activity_labels
 * @param predicted_activity_labels
 */
void ClusterRecognition::recognize_(FeatureContainer *test_sensor_data,
                                    FeatureContainer *train_sensor_data,
                                    vector<int> &actual_activity_labels,
                                    vector<int> &predicted_activity_labels,
                                    vector<int>& predicted_discovered_patterns)
{

  logging::INFO("recognize_");

  vector<vector<float> > test_sensor_duration=test_sensor_data->getSensorDurations();
  vector<vector<int> > test_active_sensors = test_sensor_data->getActiveSensorsPerPattern();
  vector<float> test_time_index = test_sensor_data->getTimeIndexPerPattern();
  vector<string> test_activity_label = test_sensor_data->getActivityLabel();

  vector<int> train_discovered_patterns = train_sensor_data->getDiscoveredPatterns();
  vector<int> train_most_select_activity_label = train_sensor_data->getMostCommonActivityLabelPerPattern();
  vector<vector<float> > train_sensor_avg_duration = train_sensor_data->getAverageSensorDurationsPerPattern();
  vector<vector<int> > train_active_sensors = train_sensor_data->getActiveSensorsPerPattern();
  vector<int> train_time_index = train_sensor_data->getMostAssignedTimeIndexPerPatternInHourIndex();
  vector<vector<int> > train_activity_label_per_pattern=train_sensor_data->getActivityLabelsPerPattern();

  for(int i =0; i<test_sensor_duration.size(); i++)
    {

      vector<float> sim_results;


      vector<float> sensor_duration1 = test_sensor_duration[i];
      vector<int> active_sensors1 = test_active_sensors[i];
      float time_index1=test_time_index[i];

      for(int j =0;j<train_sensor_avg_duration.size();j++)
        {
          vector<float> sensor_duration2 = train_sensor_avg_duration[j];
          vector<int> active_sensors2 = train_active_sensors[j];
          float time_index2=train_time_index[j];

          float duration_sim=0.0;
          float sensor_structure_sim = 0.0;
          float time_sim= 0.0;
          float total_sim= 0.0;

          similarityMeasure_->computeJaccardSimilarity(active_sensors1,active_sensors2,sensor_structure_sim);
          similarityMeasure_->computeTimeSimilarity(time_index1,time_index2,time_sim);
          similarityMeasure_->computeDurationSimilarity(sensor_duration1,sensor_duration2,duration_sim);

          total_sim= (duration_sim+sensor_structure_sim+time_sim)/3;

          sim_results.push_back(total_sim);
        }

      //find max similarity value in col (host pattern)
      auto max_host_similarity_value = std::max_element(std::begin(sim_results), std::end(sim_results));
      int max_similarity_index=std::distance(std::begin(sim_results), max_host_similarity_value);

      int predicted_sequence_pattern = train_discovered_patterns[max_similarity_index];
      int predicted_activity_label_index =train_most_select_activity_label[max_similarity_index];

      checkPredictedActivityLabel_(predicted_activity_label_index,test_activity_label[i],train_activity_label_per_pattern[max_similarity_index]);

      //      vector<float> sensor_duration2 = train_sensor_avg_duration[max_similarity_index];
      //      printDurationTwoPatterns_(sensor_duration1,sensor_duration2);

      stringstream message;
      message<<i<<" max_similarity_index:"<<max_similarity_index<<
               "\t sim_results:"<<sim_results[max_similarity_index]<<
               "\t predicted_sequence_pattern:"<<predicted_sequence_pattern<<
               "\t p_activity_label:"<<home_->getActivityLabelIntStringMap().at(predicted_activity_label_index)<<
               "\t a_activity_label:"<<test_activity_label[i]<<endl;

      //logging::INFO(message.str());


      int actual_activity_label_index = home_->getActivityLabelStringIntMap().at(test_activity_label[i]);

      if(includeOtherActivityClass_(actual_activity_label_index) && includeOtherActivityClass_(predicted_activity_label_index))
        {
          actual_activity_labels.push_back(actual_activity_label_index);
          predicted_activity_labels.push_back(predicted_activity_label_index);
          predicted_discovered_patterns.push_back(predicted_sequence_pattern);
        }

    }
}

/**
 * @brief ClusterRecognition::writePredictions_
 * @param clustered_sensor_data
 */
void ClusterRecognition::writePredictions_(FeatureContainer *clustered_sensor_data)
{

  logging::INFO("writePredictions_");
  featureWriter_->writeFeatures(clustered_sensor_data,Constants::Cluster_Type::cluster_recognition);
}

/**
 * @brief ClusterRecognition::leaveOneDayOutStrategy_
 * @param sensor_data
 * @param clustered_sensor_data
 */
void ClusterRecognition::leaveOneDayOutStrategy_(vector<FeatureContainer *> sensors_data, FeatureContainer *clustered_sensor_data)
{
  logging::INFO("leaveOneDayOutStrategy_");

  vector<int> predicted_activity_label;
  vector<int> actual_activity_label;
  vector<int> predicted_discovered_patterns;


  for(int i=0; i<sensors_data.size();i++)
    {
      FeatureContainer* test_sensor_data =sensors_data[i];

      //compute active sensor using duration not avg duration
      test_sensor_data->setAverageSensorDurationsPerPattern(test_sensor_data->getSensorDurations());
      featureProcessor_->computeActiveSensors(test_sensor_data,false);


      FeatureContainer* train_sensor_data = new FeatureContainer();
      prepareTrainingData_(test_sensor_data->getDayNamePerPattern()[0],clustered_sensor_data,train_sensor_data);

      recognize_(test_sensor_data,train_sensor_data,actual_activity_label,predicted_activity_label,predicted_discovered_patterns);


      delete train_sensor_data;

    }

  clustered_sensor_data->setPredictedActivityLabels(predicted_activity_label);
  clustered_sensor_data->setActualActivityLabels(actual_activity_label);
  clustered_sensor_data->setPredictedDiscoveredPatterns(predicted_discovered_patterns);

}
/**
 * @brief ClusterRecognition::computeFeatures_
 * @param featureContainers
 */
void ClusterRecognition::computeFeatures_(FeatureContainer *featureContainer)
{
  logging::INFO("computeFeatures_");

  sorterProcessor_->radixSort(featureContainer);

  featureProcessor_->computeOptimizedAverageSensorDurationPerPattern(featureContainer);
  featureProcessor_->computeOptimizedMostCommonActivityLabelPerPattern(featureContainer);
  featureProcessor_->computeActiveSensors(featureContainer,false);
  featureProcessor_->computeOptimizedMostAssignedTimeIndex(featureContainer);
}
