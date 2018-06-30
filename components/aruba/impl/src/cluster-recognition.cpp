#include "cluster-recognition.h"

/**
 * @brief ClusterRecognition::ClusterRecognition
 * @param hierarchal_clustering_path
 * @param cluster_rec_path
 * @param home_setup_file
 * @param time_window_config
 * @param success
 */
ClusterRecognition::ClusterRecognition(string hierarchal_clustering_path,string cluster_rec_path,string with_day_cluster_path, string home_setup_file, string time_window_config, bool &success)
{

  logging::INFO("ClusterRecognition");

  init_(hierarchal_clustering_path,cluster_rec_path,with_day_cluster_path,home_setup_file,time_window_config,success);

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
void ClusterRecognition::init_(string hierarchal_clustering_path,string cluster_rec_path, string within_day_cluster_path,string home_setup_file, string time_window_config, bool &success)
{
  logging::INFO("init_");

  home_ = new Home(home_setup_file,time_window_config);
  home_->readHomeSetup();

  cluster_rec_path_=cluster_rec_path;

  string tmp_ext=std::to_string(home_->getHierarchalClusteringThreshold());
  std::replace(tmp_ext.begin(), tmp_ext.end(), '.', '_');

  hierarchal_clustering_path=hierarchal_clustering_path+
      "_"+
      tmp_ext+
      "/";

  //  featureWriter_ = new FeatureWriter(this->cluster_rec_path_,tmp_ext,success);

  //  if(!success)
  //    {
  //      return;
  //    }

  featureReader_ = new FeatureReader(this->home_);
  featureProcessor_ = new FeatureProcessor(this->home_);
  similarityMeasure_ = new SimilarityMeasure(home_setup_file,time_window_config);

  //after processing all days using between day clustering,
  //the results are expected to be in a single feature container
  clustered_sensor_data = featureReader_->readFeatures(hierarchal_clustering_path,Constants::between_day_cluster)[0];

  within_day_cluster_path.append("_").append(std::to_string(home_->getShortDurationLambda())).append("_").append(std::to_string(home_->getLongDurationLambda())).append("/");
  within_day_cluster_path_=within_day_cluster_path;

  sensor_data = featureReader_->readFeatures(within_day_cluster_path,Constants::within_day_cluster);

  logging::INFO("features_discovered_patterns:"+std::to_string(clustered_sensor_data->getDiscoveredPatterns().size()));
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

  if(clustered_sensor_data)
    {
      delete clustered_sensor_data;
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

  //  if(featureWriter_)
  //    {
  //      delete featureWriter_;
  //    }

}

/**
 * @brief ClusterRecognition::run
 */
void ClusterRecognition::run()
{
  logging::INFO("run");

  //  computeClustersPurity_(clustered_sensor_data);
  leaveOneDayOutStrategy_(sensor_data,clustered_sensor_data);
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
 * @brief ClusterRecognition::computeClusterPurity_
 * @param featureContainers
 */
void ClusterRecognition::computeClustersPurity_(FeatureContainer *featureContainers)
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

  float purity=(sum_clusters*1.0)/(sum_labels*1.0);
  cout<<purity<<endl;


  cout<<activity_labels_per_pattern.size()<<"\t"<<sequence_patterns.size()<<"\t"<<discovered_patterns.size()<<endl;

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
void ClusterRecognition::printDurationTwoPatterns(vector<float> sensor_durations1, vector<float> sensor_durations2)
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
void ClusterRecognition::checkPredictedActivityLabel_(int &predicted_activity,string actual_activity_label, vector<int> activity_per_pattern)
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
 * @brief ClusterRecognition::recognize_
 * @param test_sensor_data
 * @param train_sensor_data
 * @param actual_activity_labels
 * @param predicted_activity_labels
 */
void ClusterRecognition::recognize_(FeatureContainer *test_sensor_data,
                                    FeatureContainer *train_sensor_data,
                                    vector<string> &actual_activity_labels,
                                    vector<string> &predicted_activity_labels,
                                    vector<int>& predicted_discovered_patterns)
{


  vector<vector<float> > test_sensor_duration=test_sensor_data->getSensorDurations();
  vector<vector<int> > test_active_sensors = test_sensor_data->getActiveSensorsPerPattern();
  vector<float> test_time_index = test_sensor_data->getTimeIndexPerPattern();
  vector<string> test_activity_label = test_sensor_data->getActivityLabel();

  vector<int> train_discovered_patterns = train_sensor_data->getDiscoveredPatterns();
  vector<int> train_most_select_activity_label = train_sensor_data->getMostCommonActivityLabelPerPattern();
  vector<int> train_sequence_patterns = train_sensor_data->getSequencePatterns();
  vector<vector<float> > train_sensor_avg_duration = train_sensor_data->getAverageSensorDurationsPerPattern();
  vector<vector<int> > train_active_sensors = train_sensor_data->getActiveSensorsPerPattern();
  vector<int> train_time_index = train_sensor_data->getMostAssignedTimeIndexPerPatternInHourIndex();
  vector<vector<int> > train_activity_label_per_pattern=train_sensor_data->getActivityLabelsPerPattern();



  for(int i =0; i<test_sensor_duration.size(); i++)
    {

      vector<float> sim_results;

      actual_activity_labels.push_back(test_activity_label[i]);

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

      vector<float> sensor_duration2 = train_sensor_avg_duration[max_similarity_index];
      printDurationTwoPatterns(sensor_duration1,sensor_duration2);

      int predicted_sequence_pattern = train_discovered_patterns[max_similarity_index];


      int predicted_activity_label =train_most_select_activity_label[max_similarity_index];

      checkPredictedActivityLabel_(predicted_activity_label,test_activity_label[i],train_activity_label_per_pattern[max_similarity_index]);

      stringstream message;
      message<<i<<" max_similarity_index:"<<max_similarity_index<<
               "\t sim_results:"<<sim_results[max_similarity_index]<<
               "\t predicted_sequence_pattern:"<<predicted_sequence_pattern<<
               "\t p_activity_label:"<<home_->getActivityLabelIntStringMap().at(predicted_activity_label)<<
               "\t a_activity_label:"<<test_activity_label[i]<<endl;

      logging::INFO(message.str());


      //      predicted_activity_labels.push_back(home_->getActivityLabelIntStringMap().at(predicted_activity_label));

      predicted_discovered_patterns.push_back(predicted_sequence_pattern);

    }


}
/**
 * @brief ClusterRecognition::leaveOneDayOutStrategy_
 * @param sensor_data
 * @param clustered_sensor_data
 */
void ClusterRecognition::leaveOneDayOutStrategy_(vector<FeatureContainer *>& sensors_data, FeatureContainer *clustered_sensor_data)
{
  logging::INFO("leaveOneDayOutStrategy_");

  vector<string> predicted_activity_label;
  vector<string> actual_activity_label;
  vector<int> predicted_discovered_patterns;

  for(int i=0; i<sensors_data.size();i++)
    {
      FeatureContainer* test_sensor_data =sensors_data[i];

      //compute active sensor using duration not avg duration
      test_sensor_data->setAverageSensorDurationsPerPattern(test_sensor_data->getSensorDurations());
      featureProcessor_->computeActiveSensors(test_sensor_data);


      FeatureContainer* train_sensor_data = new FeatureContainer();
      prepareTrainingData_(test_sensor_data->getDayNamePerPattern()[0],clustered_sensor_data,train_sensor_data);

      recognize_(test_sensor_data,train_sensor_data,actual_activity_label,predicted_activity_label,predicted_discovered_patterns);


      delete train_sensor_data;

    }

}
/**
 * @brief ClusterRecognition::computeFeatures_
 * @param featureContainers
 */
void ClusterRecognition::computeFeatures_(FeatureContainer *featureContainer)
{
  logging::INFO("computeFeatures_");

  featureProcessor_->computeAverageSensorDurationPerPattern(featureContainer);
  featureProcessor_->computeMostCommonActivityLabelPerPattern(featureContainer);
  featureProcessor_->computeActiveSensors(featureContainer);
  featureProcessor_->computeMostAssignedTimeIndex(featureContainer);
  featureProcessor_->computePatternsLength(featureContainer);
}
