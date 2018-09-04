#include "hierarchal-agglomerative-clustering.h"

/**
 * @brief HierarchalAgglomerativeClustering::HierarchalAgglomerativeClustering
 * @param feature_reader_path
 * @param hierarchal_clustering_path
 * @param home_setup_file
 * @param time_window_config
 */
HierarchalAgglomerativeClustering::HierarchalAgglomerativeClustering(string feature_reader_path,string hierarchal_clustering_path,string home_setup_file, string time_window_config,bool& success)
{
  logging::INFO("HierarchalAgglomerativeClustering");

  init_(feature_reader_path,hierarchal_clustering_path,home_setup_file, time_window_config,success);

  if(!success)
    {
      return;
    }
}

/**
 * @brief HierarchalAgglomerativeClustering::destroy
 */
void HierarchalAgglomerativeClustering::destroy()
{
  delete this;
}

/**
 * @brief HierarchalAgglomerativeClustering::init_
 * @param feature_reader_path
 * @param hierarchal_clustering_path
 * @param home_setup_file
 * @param time_window_config
 * @param success
 */
void HierarchalAgglomerativeClustering::init_(string feature_reader_path,string hierarchal_clustering_path,string home_setup_file, string time_window_config,bool& success)
{
  logging::INFO("init");

  home_ = new Home(home_setup_file,time_window_config);
  home_->readHomeSetup();

  hierarchal_clustering_path_=hierarchal_clustering_path;
  //set clustering threshold
  hierarchal_threshold= home_->getHierarchalClusteringThreshold();
  num_threads= boost::thread::hardware_concurrency();
  last_discovered_patterns = 0;

  logging::INFO("num_threads:"+std::to_string(num_threads));

  feature_reader_path=feature_reader_path+
      "_"+
      std::to_string(int(home_->getEmdSimilarityThreshold()))+
      "/";

  string tmp_ext=std::to_string(home_->getHierarchalClusteringThreshold());
  std::replace(tmp_ext.begin(), tmp_ext.end(), '.', '_');
  featureWriter_ = new FeatureWriter(this->hierarchal_clustering_path_,tmp_ext,home_,success);

  if(!success)
    {
      return;
    }

  featureReader_ = new FeatureReader(this->home_);
  featureProcessor_ = new FeatureProcessor(this->home_);

  similarityMeasure_ = new SimilarityMeasure(home_setup_file,time_window_config);

  //after processing all days using between day clustering,
  //the results are expected to be in a single feature container
  mergedSubFeatureContainers_ = featureReader_->readFeatures(feature_reader_path,Constants::between_day_cluster)[0];

  sorterProcessor_ = new SorterProcessor();
}


/**
 * @brief HierarchalAgglomerativeClustering::~HierarchalAgglomerativeClustering
 */
HierarchalAgglomerativeClustering::~HierarchalAgglomerativeClustering()
{
  logging::INFO("~HierarchalAgglomerativeClustering");

  if(featureProcessor_)
    {
      delete featureProcessor_;
    }
  if(featureReader_)
    {
      delete featureReader_;
    }
  if(mergedSubFeatureContainers_)
    {
      delete mergedSubFeatureContainers_;
    }
  if(featureWriter_)
    {
      delete featureWriter_;
    }

  if(similarityMeasure_)
    {
      delete similarityMeasure_;
    }
}

/**
 * @brief HierarchalAgglomerativeClustering::computeSubContainersClusters_
 * @param mergedFeatureContainers
 * @param subFeatureContainers
 */
void HierarchalAgglomerativeClustering::computeSubContainersClusters_(FeatureContainer *mergedFeatureContainers,int& num_threads)
{
  logging::INFO("computeSubContainersClusters_");

  logging::INFO("last_discovered_patterns:"+std::to_string(last_discovered_patterns)+
                "\t"+
                "merged_discovered_patterns:"+std::to_string(mergedFeatureContainers->getDiscoveredPatterns().size()));

  cout<<"last_discovered_patterns:"<<last_discovered_patterns<<"\t"<<"merged_discovered_patterns:"<<mergedFeatureContainers->getDiscoveredPatterns().size()<<endl;


  if(num_threads==0)
    {
      //computeContainerClusters_(mergedFeatureContainers);
      return;
    }

  //initialize sub containers
  vector<FeatureContainer*> subFeatureContainers;
  for(int i = 0 ; i<num_threads;i++)
    {
      FeatureContainer* fc = new FeatureContainer();
      subFeatureContainers.push_back(fc);
    }

  //divide problem to small problems
  divideContainerToSubContainers_(mergedFeatureContainers,subFeatureContainers,num_threads);

  //threading to computeContainerClusters
  vector<boost::thread*> threads;
  for(int i =0;i<num_threads;i++)
    {
      FeatureContainer* fc = subFeatureContainers[i];

      //g.add_thread(new boost::thread([fc, this] { computeContainerClusters_(fc); }));
      boost::thread* t = new boost::thread([fc, this] { optimizedComputeContainerClusters_(fc); });
      g.add_thread(t);
      threads.push_back(t);

    }

  g.join_all();



  //merge solutions to one solution
  mergeSubContainersToContainer_(subFeatureContainers,mergedFeatureContainers);


  last_discovered_patterns = mergedFeatureContainers->getDiscoveredPatterns().size();

  //free subcontainers

  for(int i = 0; i<num_threads;i++)
    {
      delete subFeatureContainers[i];
      delete threads[i];
    }

  //recursion
  computeSubContainersClusters_(mergedFeatureContainers,--num_threads);
}

/**
 * @brief HierarchalAgglomerativeClustering::computeFeatures_
 * @param featureContainer
 */
void HierarchalAgglomerativeClustering::computeFeatures_(FeatureContainer* featureContainer)
{
  logging::INFO("computeFeatures_");

  featureProcessor_->computeAverageSensorDurationPerPattern(featureContainer);
  featureProcessor_->computeMostCommonActivityLabelPerPattern(featureContainer);
  featureProcessor_->computeActiveSensors(featureContainer,false);
  featureProcessor_->computeMostAssignedTimeIndex(featureContainer);
  //featureProcessor_->computePatternsLength(featureContainer);
}


/**
 * @brief HierarchalAgglomerativeClustering::optimizedComputeFeatures_
 * @param featureContainer
 */
void HierarchalAgglomerativeClustering::optimizedComputeFeatures_(FeatureContainer* featureContainer)
{
  logging::INFO("optimizedComputeFeatures_");

  sorterProcessor_->radixSort(featureContainer);

  featureProcessor_->computeOptimizedAverageSensorDurationPerPattern(featureContainer);
  featureProcessor_->computeOptimizedMostCommonActivityLabelPerPattern(featureContainer);
  featureProcessor_->computeActiveSensors(featureContainer,false);
  featureProcessor_->computeOptimizedMostAssignedTimeIndex(featureContainer);
}

/**
 * @brief HierarchalAgglomerativeClustering::getClusteredPatterns
 * @return
 */
FeatureContainer* HierarchalAgglomerativeClustering::getClusteredPatterns()
{
  return mergedSubFeatureContainers_;
}



/**
 * @brief HierarchalAgglomerativeClustering::divideContainerToSubContainers_
 * @param mergedSubFeatureContainer
 * @param subFeatureContainers
 */
void HierarchalAgglomerativeClustering::divideContainerToSubContainers_(FeatureContainer *mergedSubFeatureContainer, vector<FeatureContainer *>& subFeatureContainers,int& num_threads)
{
  logging::INFO("divideContainerToSubContainers_");

  vector<int> num_discovered_patterns_all_containers;
  Common::computeNumElementsPerThread(mergedSubFeatureContainer->getDiscoveredPatterns().size(),num_threads,num_discovered_patterns_all_containers);

  int from=0;
  int to = num_discovered_patterns_all_containers[0];

  for(int i =0; i<num_discovered_patterns_all_containers.size();i++)
    {
      logging::INFO("from:"+
                    std::to_string(from)+
                    "\t"+
                    "to:"+
                    std::to_string(to));


      divideContainer_(from,to,mergedSubFeatureContainer,subFeatureContainers[i]);

      //update ranges
      from = to;
      to+=num_discovered_patterns_all_containers[i+1];
    }
}

/**
 * @brief HierarchalAgglomerativeClustering::divideContainer_
 * @param from
 * @param to
 * @param mergedSubFeatureContainers
 * @param subFeatureContainer
 */
void HierarchalAgglomerativeClustering::divideContainer_(int from, int to,FeatureContainer *mergedSubFeatureContainers, FeatureContainer *subFeatureContainer)
{
  logging::INFO("divideContainer_");

  vector<int> discovered_patterns = mergedSubFeatureContainers->getDiscoveredPatterns();
  vector<int> sequence_patterns = mergedSubFeatureContainers->getSequencePatterns();
  vector<float> time_index_per_pattern = mergedSubFeatureContainers->getTimeIndexPerPattern();
  vector<vector<float> > sensor_durations_per_pattern = mergedSubFeatureContainers->getSensorDurations();
  vector<string> day_names_per_pattern = mergedSubFeatureContainers->getDayNamePerPattern();
  vector<string> activity_label_per_pattern = mergedSubFeatureContainers->getActivityLabel();

  //holders for sub container
  vector<int> sub_discovered_patterns;
  vector<int> sub_sequence_patterns;
  vector<float> sub_time_index;
  vector<vector<float> > sub_sensor_durations;
  vector<string> sub_day_names;
  vector<string> sub_activity_labels;


  //copy data to sub container
  for(int i = from; i< to; i++)
    {

      for(int j = 0; j <sequence_patterns.size();j++)
        {
          if( discovered_patterns[i] == sequence_patterns[j])
            {
              sub_sequence_patterns.push_back(sequence_patterns[j]);
              sub_sensor_durations.push_back(sensor_durations_per_pattern[j]);
              sub_time_index.push_back(time_index_per_pattern[j]);
              sub_day_names.push_back(day_names_per_pattern[j]);
              sub_activity_labels.push_back(activity_label_per_pattern[j]);
            }
        }

      sub_discovered_patterns.push_back(discovered_patterns[i] );
    }

  //set data to sub container
  subFeatureContainer->setDiscoveredPatterns(sub_discovered_patterns);
  subFeatureContainer->setSequencePatterns(sub_sequence_patterns);
  subFeatureContainer->setSensorDurations(sub_sensor_durations);
  subFeatureContainer->setTimeIndexPerPattern(sub_time_index);
  subFeatureContainer->setDayNamePerPattern(sub_day_names);
  subFeatureContainer->setActivityLabel(sub_activity_labels);

  logging::INFO("merged_containers_discovered_patterns:"+
                std::to_string(discovered_patterns.size())+
                "\t"+
                "sub_container_discovered_patterns:"+
                std::to_string(sub_discovered_patterns.size()));

}

/**
 * @brief HierarchalAgglomerativeClustering::mergeSubContainersToContainer_
 * @param subFeatureContainers
 * @param mergedSubFeatureContainer
 */
void HierarchalAgglomerativeClustering::mergeSubContainersToContainer_(vector<FeatureContainer *>& subFeatureContainers, FeatureContainer *mergedSubFeatureContainer)
{
  logging::INFO("mergeSubContainersToContainer_");

  //holders for sub container
  vector<int> merged_discovered_patterns;
  vector<int> merged_sequence_patterns;
  vector<float> merged_time_index;
  vector<vector<float> > merged_sensor_durations;
  vector<string> merged_day_names;
  vector<string> merged_activity_labels;

  for(int i =0; i<subFeatureContainers.size();i++)
    {
      FeatureContainer* subFeatureContainer = subFeatureContainers[i];

      vector<int> sub_discovered_patterns(subFeatureContainer->getDiscoveredPatterns());
      vector<int> sub_sequence_patterns(subFeatureContainer->getSequencePatterns());
      vector<float> sub_time_index(subFeatureContainer->getTimeIndexPerPattern());
      vector<vector<float> > sub_sensor_durations(subFeatureContainer->getSensorDurations());
      vector<string> sub_day_names(subFeatureContainer->getDayNamePerPattern());
      vector<string> sub_activity_labels(subFeatureContainer->getActivityLabel());

      //remove marked duplicate patterns with index -1
      sub_discovered_patterns.erase(std::remove(sub_discovered_patterns.begin(), sub_discovered_patterns.end(), -1), sub_discovered_patterns.end());
      //copy data from sub containers to one container
      merged_discovered_patterns.insert(merged_discovered_patterns.end(), sub_discovered_patterns.begin(), sub_discovered_patterns.end());
      merged_sequence_patterns.insert(merged_sequence_patterns.end(),sub_sequence_patterns.begin(),sub_sequence_patterns.end());
      merged_time_index.insert(merged_time_index.end(),sub_time_index.begin(),sub_time_index.end());
      merged_sensor_durations.insert(merged_sensor_durations.end(),sub_sensor_durations.begin(),sub_sensor_durations.end());
      merged_day_names.insert(merged_day_names.end(),sub_day_names.begin(),sub_day_names.end());
      merged_activity_labels.insert(merged_activity_labels.end(),sub_activity_labels.begin(),sub_activity_labels.end());

      logging::INFO("sub_container_discovered_patterns:"+
                    std::to_string(sub_discovered_patterns.size())+
                    "\t"+
                    "merged_containers_discovered_patterns:"+
                    std::to_string(merged_discovered_patterns.size()));

    }

  //set copied data to one container
  mergedSubFeatureContainer->setDiscoveredPatterns(merged_discovered_patterns);
  mergedSubFeatureContainer->setSequencePatterns(merged_sequence_patterns);
  mergedSubFeatureContainer->setSensorDurations(merged_sensor_durations);
  mergedSubFeatureContainer->setTimeIndexPerPattern(merged_time_index);
  mergedSubFeatureContainer->setDayNamePerPattern(merged_day_names);
  mergedSubFeatureContainer->setActivityLabel(merged_activity_labels);
}

/**
 * @brief HierarchalAgglomerativeClustering::checkFeatures
 * @param featureContainer
 * @param host_pattern_index
 * @param guest_pattern_index
 */
void HierarchalAgglomerativeClustering::checkFeatures(FeatureContainer *featureContainer, int &host_pattern_index, int &guest_pattern_index,vector<vector<float> >&proximity_matrix)
{

  logging::INFO("checkFeatures_");

  if(host_pattern_index!= -1 && guest_pattern_index !=-1)
    {
      logging::INFO("host_pattern_index:"+std::to_string(host_pattern_index)+
                    "\t"+
                    "guest_pattern_index:"+std::to_string(guest_pattern_index));

      logging::INFO("discovered_host_index:"+std::to_string(featureContainer->getDiscoveredPatterns()[host_pattern_index])+
                    "\t"+
                    "discovered_guest_index:"+std::to_string(featureContainer->getDiscoveredPatterns()[guest_pattern_index]));

      //      int host_pattern_length = featureContainer->getPatternsLength()[host_pattern_index];
      //      int guest_pattern_length = featureContainer->getPatternsLength()[guest_pattern_index];

      //      logging::INFO("host_pattern_length:"+std::to_string(host_pattern_length)+
      //                    "\t"+
      //                    "guest_pattern_length:"+std::to_string(guest_pattern_length));

      vector<vector<float> > avg_sensor_durations_per_pattern= featureContainer->getAverageSensorDurationsPerPattern();
      vector<vector<int> > active_sensors_per_pattern = featureContainer->getActiveSensorsPerPattern();
      vector<int> most_time_index_per_pattern = featureContainer->getMostAssignedTimeIndexPerPatternInHourIndex();



      vector<float> guest_avg_sensor_durations=avg_sensor_durations_per_pattern[guest_pattern_index];

      stringstream tmp_str;
      for(int i =0;i<guest_avg_sensor_durations.size();i++)
        {

          tmp_str<<guest_avg_sensor_durations[i]<<"\t";
        }


      logging::INFO("guest_avg_sensor_durations:"+(string)tmp_str.str().c_str());
      logging::INFO("#proximity[host_index][guest_index]:"+std::to_string(proximity_matrix[host_pattern_index][guest_pattern_index]));

    }
}

/**
 * @brief HierarchalAgglomerativeClustering::computeContainerClusters
 * @param featureContainer
 */
void HierarchalAgglomerativeClustering::computeContainerClusters_(FeatureContainer *featureContainer)
{
  logging::INFO("computeContainerClusters_");
  vector<vector<float> > proximity_matrix;
  float sim=0.0;
  int host_pattern_index=-1;
  int guest_pattern_index=-1;
  bool is_first_to_compute = true;


  do
    {
      computeFeatures_(featureContainer);

      if(is_first_to_compute)
        {
          computeProximityMatrix_(featureContainer,proximity_matrix,host_pattern_index,guest_pattern_index,sim);
          is_first_to_compute = false;
        }
      else
        {
          updateProximityMatrix_(featureContainer,proximity_matrix,host_pattern_index,guest_pattern_index,sim);
        }

      if(sim >= hierarchal_threshold)
        {
          //checkFeatures(featureContainer,host_pattern_index,guest_pattern_index,proximity_matrix);
          mergePatterns_(featureContainer,host_pattern_index,guest_pattern_index);
        }

      printDebugInfo(featureContainer,sim);

    }while(sim >= hierarchal_threshold);
}

/**
 * @brief HierarchalAgglomerativeClustering::optimizedComputeContainerClusters_
 * @param featureContainer
 */
void HierarchalAgglomerativeClustering::optimizedComputeContainerClusters_(FeatureContainer *featureContainer)
{
  logging::INFO("optimizedComputeContainerClusters_");
  vector<vector<float> > proximity_matrix;
  float sim=0.0;
  int host_pattern_index=-1;
  int guest_pattern_index=-1;
  bool is_first_to_compute = true;


  do
    {
      optimizedComputeFeatures_(featureContainer);

      if(is_first_to_compute)
        {
          computeProximityMatrix_(featureContainer,proximity_matrix,host_pattern_index,guest_pattern_index,sim);
          is_first_to_compute = false;
        }
      else
        {
          optimizedUpdateProximityMatrixWithHostPattern_(host_pattern_index,featureContainer,proximity_matrix);
        }

      computePatternsToMerge_(proximity_matrix,host_pattern_index,guest_pattern_index,sim);

      if(sim >= hierarchal_threshold)
        {
          //checkFeatures(featureContainer,host_pattern_index,guest_pattern_index,proximity_matrix);

          optimizedUpdatePatterns_(featureContainer,proximity_matrix,host_pattern_index,guest_pattern_index);
        }

      printDebugInfo(featureContainer,sim);

    }while(sim >= hierarchal_threshold);
}


/**
 * @brief HierarchalAgglomerativeClustering::printDebugInfo
 * @param featureContainer
 * @param sim
 */
void HierarchalAgglomerativeClustering::printDebugInfo(FeatureContainer *featureContainer, float sim)
{

  cout<<"sim:"<<sim<<endl;
  logging::INFO("sim:"+std::to_string(sim));

  logging::INFO("=============================");
}

/**
 * @brief HierarchalAgglomerativeClustering::run
 */
void HierarchalAgglomerativeClustering::run()
{
  logging::INFO("run");

  //sub divide cluster problem in smaller problems to find final cluster
  computeSubContainersClusters_(mergedSubFeatureContainers_,num_threads);

  removeInvalidDiscoveredPatterns_(mergedSubFeatureContainers_);
  //write results to disk
  featureWriter_->writeFeatures(mergedSubFeatureContainers_,Constants::Cluster_Type::hierarchal_cluster);

}

/**
 * @brief HierarchalAgglomerativeClustering::removeInvalidDiscoveredPatterns_
 * @param featureContainer
 */
void HierarchalAgglomerativeClustering::removeInvalidDiscoveredPatterns_(FeatureContainer *featureContainer)
{
  logging::INFO("removeInvalidDiscoveredPatterns_");
  vector<int> discovered_patterns = featureContainer->getDiscoveredPatterns();
  //remove invalid patterns
  discovered_patterns.erase(std::remove(discovered_patterns.begin(), discovered_patterns.end(), -1), discovered_patterns.end());

  logging::INFO("discovered_patterns:"+std::to_string(discovered_patterns.size()));
  featureContainer->setDiscoveredPatterns(discovered_patterns);
}

/**
 * @brief HierarchalAgglomerativeClustering::computeProximityMatrix_
 * @param featureContainer
 */
void HierarchalAgglomerativeClustering::computeProximityMatrix_(FeatureContainer* featureContainer,vector<vector<float> >& proximity_matrix,int& host_pattern, int& guest_pattern, float& similarity)
{
  logging::INFO("computeProximityMatrix_");

  vector<vector<float> > avg_sensor_durations_per_pattern= featureContainer->getAverageSensorDurationsPerPattern();
  vector<vector<int> > active_sensors_per_pattern = featureContainer->getActiveSensorsPerPattern();
  vector<int> most_time_index_per_pattern = featureContainer->getMostAssignedTimeIndexPerPatternInHourIndex();


  for(int i = 0 ; i<avg_sensor_durations_per_pattern.size();i++)
    {
      vector<float> avg_sensor_durations1=avg_sensor_durations_per_pattern[i];
      vector<int> active_sensors1 = active_sensors_per_pattern[i];
      int time_index1 = most_time_index_per_pattern[i];

      vector<float> proximity_row;

      for(int j =0; j<avg_sensor_durations_per_pattern.size();j++)
        {
          vector<float> avg_sensor_durations2=avg_sensor_durations_per_pattern[j];
          vector<int> active_sensors2 = active_sensors_per_pattern[j];
          int time_index2 = most_time_index_per_pattern[j];

          float duration_sim=0.0;
          float sensor_structure_sim = 0.0;
          float time_sim= 0.0;
          float total_sim= 0.0;

          //compute the similarity between different patterns
          //do not compute any similarity between the pattern and itself
          //give it the lowest similarity, since we are not interested in it
          if(i!=j)
            {
              similarityMeasure_->computeDurationSimilarity(avg_sensor_durations1,avg_sensor_durations2,duration_sim);
              similarityMeasure_->computeJaccardSimilarity(active_sensors1,active_sensors2,sensor_structure_sim);
              similarityMeasure_->computeTimeSimilarity(time_index1,time_index2,time_sim);

              total_sim= (duration_sim+sensor_structure_sim+time_sim)/3;
            }

          proximity_row.push_back(total_sim);

        }

      proximity_matrix.push_back(proximity_row);
    }
  //computePatternsToMerge_(proximity_matrix,host_pattern,guest_pattern,similarity);
}

/**
 * @brief HierarchalAgglomerativeClustering::updateProximityMatrix_
 * @param featureContainer
 * @param proximity_matrix
 * @param host_pattern_index
 * @param guest_pattern_index
 * @param similarity
 */
void HierarchalAgglomerativeClustering::updateProximityMatrix_(FeatureContainer *featureContainer, vector<vector<float> > &proximity_matrix, int &host_pattern_index, int &guest_pattern_index, float &similarity)
{
  logging::INFO("updateProximityMatrix_");
  logging::INFO("host_pattern_index:"+
                std::to_string(host_pattern_index)+
                "\t"+
                "guest_pattern_index:"+
                std::to_string(guest_pattern_index));

  updateProximityMatrixWithGuestPattern_(host_pattern_index,guest_pattern_index,proximity_matrix);

  logging::INFO("*proximity[host_index][guest_index]:"+std::to_string(proximity_matrix[host_pattern_index][guest_pattern_index]));

  updateProximityMatrixWithHostPattern_(host_pattern_index,guest_pattern_index,featureContainer,proximity_matrix);

  logging::INFO("#proximity[host_index][guest_index]:"+std::to_string(proximity_matrix[host_pattern_index][guest_pattern_index]));
  computePatternsToMerge_(proximity_matrix,host_pattern_index,guest_pattern_index,similarity);

}



/**
 * @brief HierarchalAgglomerativeClustering::updateProximityMatrixWithHostPattern_
 * @param host_pattern_index
 * @param featureContainer
 * @param proximity_matrix
 */
void HierarchalAgglomerativeClustering::updateProximityMatrixWithHostPattern_(int &host_pattern_index,int& guest_pattern_index, FeatureContainer *featureContainer, vector<vector<float> > &proximity_matrix)
{

  logging::INFO("updateProximityMatrixWithHostPattern_");
  logging::INFO("host_pattern_index:"+std::to_string(host_pattern_index));

  vector<vector<float> > avg_sensor_durations_per_pattern= featureContainer->getAverageSensorDurationsPerPattern();
  vector<vector<int> > active_sensors_per_pattern = featureContainer->getActiveSensorsPerPattern();
  vector<int> most_time_index_per_pattern = featureContainer->getMostAssignedTimeIndexPerPatternInHourIndex();


  //select host pattern features
  vector<float> avg_sensor_durations1 = avg_sensor_durations_per_pattern[host_pattern_index];
  vector<int> active_sensors1 = active_sensors_per_pattern[host_pattern_index];
  int time_index1 = most_time_index_per_pattern[host_pattern_index];

  for(int i =0;i<avg_sensor_durations_per_pattern.size();i++)
    {
      float duration_sim=0.0;
      float sensor_structure_sim = 0.0;
      float time_sim= 0.0;
      float total_sim= 0.0;


      //compare host pattern features with row patterns on right symetric part
      if(i !=guest_pattern_index && i != host_pattern_index)
        {
          //          logging::INFO("*row:"+std::to_string(i)+
          //                        "\t"+
          //                        "col:"+std::to_string(host_pattern_index));

          //make sure it is a valid entry
          vector<float> avg_sensor_durations2=avg_sensor_durations_per_pattern[i];
          vector<int> active_sensors2 = active_sensors_per_pattern[i];
          int time_index2 = most_time_index_per_pattern[i];


          similarityMeasure_->computeDurationSimilarity(avg_sensor_durations1,avg_sensor_durations2,duration_sim);
          similarityMeasure_->computeJaccardSimilarity(active_sensors1,active_sensors2,sensor_structure_sim);
          similarityMeasure_->computeTimeSimilarity(time_index1,time_index2,time_sim);

          total_sim= (duration_sim+sensor_structure_sim+time_sim)/3;

          proximity_matrix[i][host_pattern_index]=total_sim;

        }
    }

  vector<float> host_row = proximity_matrix[host_pattern_index];

  for(int i =0;i<host_row.size();i++)
    {
      float duration_sim=0.0;
      float sensor_structure_sim = 0.0;
      float time_sim= 0.0;
      float total_sim= 0.0;

      //last row does not have any entries, avoid it, accessing it causes crash
      if(i != guest_pattern_index && i != host_pattern_index)
        {

          //          logging::INFO("#row:"+std::to_string(host_pattern_index)+
          //                        "\t"+
          //                        "col:"+std::to_string(i));

          vector<float> avg_sensor_durations2=avg_sensor_durations_per_pattern[i];
          vector<int> active_sensors2 = active_sensors_per_pattern[i];
          int time_index2 = most_time_index_per_pattern[i];

          similarityMeasure_->computeDurationSimilarity(avg_sensor_durations1,avg_sensor_durations2,duration_sim);
          similarityMeasure_->computeJaccardSimilarity(active_sensors1,active_sensors2,sensor_structure_sim);
          similarityMeasure_->computeTimeSimilarity(time_index1,time_index2,time_sim);

          total_sim= (duration_sim+sensor_structure_sim+time_sim)/3;

        }
      else if(i==guest_pattern_index || i==host_pattern_index)
        {
          //          logging::INFO("*#row:"+std::to_string(host_pattern_index)+
          //                        "\t"+
          //                        "col:"+std::to_string(i));
        }

      host_row[i] = total_sim;
    }

  proximity_matrix[host_pattern_index]=host_row;

}

/**
 * @brief HierarchalAgglomerativeClustering::optimzedUpdateProximityMatrixWithHostPattern_
 * @param host_pattern_index
 * @param featureContainer
 * @param proximity_matrix
 */
void HierarchalAgglomerativeClustering::optimizedUpdateProximityMatrixWithHostPattern_(int &host_pattern_index, FeatureContainer *featureContainer, vector<vector<float> > &proximity_matrix)
{

  logging::INFO("optimizedUpdateProximityMatrixWithHostPattern_");
  logging::INFO("host_pattern_index:"+std::to_string(host_pattern_index));

  vector<vector<float> > avg_sensor_durations_per_pattern= featureContainer->getAverageSensorDurationsPerPattern();
  vector<vector<int> > active_sensors_per_pattern = featureContainer->getActiveSensorsPerPattern();
  vector<int> most_time_index_per_pattern = featureContainer->getMostAssignedTimeIndexPerPatternInHourIndex();


  //select host pattern features
  vector<float> avg_sensor_durations1 = avg_sensor_durations_per_pattern[host_pattern_index];
  vector<int> active_sensors1 = active_sensors_per_pattern[host_pattern_index];
  int time_index1 = most_time_index_per_pattern[host_pattern_index];

  for(int i =0;i<avg_sensor_durations_per_pattern.size();i++)
    {
      float duration_sim=0.0;
      float sensor_structure_sim = 0.0;
      float time_sim= 0.0;
      float total_sim= 0.0;



      if(i != host_pattern_index)
        {
          //          logging::INFO("*row:"+std::to_string(i)+
          //                        "\t"+
          //                        "col:"+std::to_string(host_pattern_index));

          //make sure it is a valid entry
          vector<float> avg_sensor_durations2=avg_sensor_durations_per_pattern[i];
          vector<int> active_sensors2 = active_sensors_per_pattern[i];
          int time_index2 = most_time_index_per_pattern[i];


          similarityMeasure_->computeDurationSimilarity(avg_sensor_durations1,avg_sensor_durations2,duration_sim);
          similarityMeasure_->computeJaccardSimilarity(active_sensors1,active_sensors2,sensor_structure_sim);
          similarityMeasure_->computeTimeSimilarity(time_index1,time_index2,time_sim);

          total_sim= (duration_sim+sensor_structure_sim+time_sim)/3;

          proximity_matrix[i][host_pattern_index]=total_sim;

        }
    }

  vector<float> host_row = proximity_matrix[host_pattern_index];

  for(int i =0;i<host_row.size();i++)
    {
      float duration_sim=0.0;
      float sensor_structure_sim = 0.0;
      float time_sim= 0.0;
      float total_sim= 0.0;


      if(i != host_pattern_index)
        {

          //          logging::INFO("#row:"+std::to_string(host_pattern_index)+
          //                        "\t"+
          //                        "col:"+std::to_string(i));

          vector<float> avg_sensor_durations2=avg_sensor_durations_per_pattern[i];
          vector<int> active_sensors2 = active_sensors_per_pattern[i];
          int time_index2 = most_time_index_per_pattern[i];

          similarityMeasure_->computeDurationSimilarity(avg_sensor_durations1,avg_sensor_durations2,duration_sim);
          similarityMeasure_->computeJaccardSimilarity(active_sensors1,active_sensors2,sensor_structure_sim);
          similarityMeasure_->computeTimeSimilarity(time_index1,time_index2,time_sim);

          total_sim= (duration_sim+sensor_structure_sim+time_sim)/3;

        }

      host_row[i] = total_sim;
    }

  proximity_matrix[host_pattern_index]=host_row;

}

/**
 * @brief HierarchalAgglomerativeClustering::updateProximityMatrixWithGuestPattern_
 * @param guest_pattern_index
 * @param proximity_matrix
 */
void HierarchalAgglomerativeClustering::updateProximityMatrixWithGuestPattern_(int &host_pattern_index, int& guest_pattern_index, vector<vector<float> > &proximity_matrix)
{
  logging::INFO("updateProximityMatrixWithGuestPattern_");

  logging::INFO("guest_pattern_index:"+std::to_string(guest_pattern_index));
  //update rows in proximity matrix to have zero similarity since it is merged with the host pattern

  for(int i =0; i<proximity_matrix.size();i++)
    {
      //make sure we access the right symetric part of the martix
      //there is no left symetric part in the matrix, accessing it will result in crash

      //      logging::INFO("^row:"+std::to_string(i)+
      //                    "\t"+
      //                    "col:"+std::to_string(guest_pattern_index));

      //update guest pattern in each row
      proximity_matrix[i][guest_pattern_index] = 0.0;
    }

  //update columns proximity matrix to have zeor similarity
  vector<float> guest_row=proximity_matrix[guest_pattern_index];

  for(int i=0;i<guest_row.size();i++)
    {
      //      logging::INFO("-row:"+std::to_string(guest_pattern_index)+
      //                    "\t"+
      //                    "col:"+std::to_string(i));

      guest_row[i]=0.0;
    }
  proximity_matrix[guest_pattern_index]=guest_row;
}

/**
 * @brief HierarchalAgglomerativeClustering::mergePatterns_
 * @param featureContainer
 * @param host_pattern_index
 * @param guest_pattern_index
 */
void HierarchalAgglomerativeClustering::mergePatterns_(FeatureContainer *featureContainer, int& host_pattern_index, int& guest_pattern_index)
{
  logging::INFO("mergePatterns_");


  logging::INFO("*host_pattern_index:"+std::to_string(host_pattern_index)+
                "\t"+
                "*guest_pattern_index:"+std::to_string(guest_pattern_index));


  logging::INFO("*discovered_host_index:"+std::to_string(featureContainer->getDiscoveredPatterns()[host_pattern_index])+
                "\t"+
                "*discovered_guest_index:"+std::to_string(featureContainer->getDiscoveredPatterns()[guest_pattern_index]));

  //for debugging purpose
  if(featureContainer->getDiscoveredPatterns()[host_pattern_index] ==featureContainer->getDiscoveredPatterns()[guest_pattern_index] )
    {
      cout<<"similar patterns with different index:"<<endl;
      cout<<"num_threads"<<num_threads<<endl;
      exit(0);
    }

  //  int host_pattern_length = featureContainer->getPatternsLength()[host_pattern_index];
  //  int guest_pattern_length = featureContainer->getPatternsLength()[guest_pattern_index];

  //  logging::INFO("host_pattern_length:"+std::to_string(host_pattern_length)+
  //                "\t"+
  //                "guest_pattern_length:"+std::to_string(guest_pattern_length));


  updatePatterns_(featureContainer,host_pattern_index,guest_pattern_index);
}

/**
 * @brief HierarchalAgglomerativeClustering::updatePatterns_
 * @param featureContainer
 * @param host_pattern_index
 * @param guest_pattern_index
 */
void HierarchalAgglomerativeClustering::updatePatterns_(FeatureContainer *featureContainer,int host_pattern_index, int guest_pattern_index)
{
  logging::INFO("updatePatterns_");

  //prepare host pattern info
  vector<int> discovered_patterns = featureContainer->getDiscoveredPatterns();
  vector<int> sequence_patterns = featureContainer->getSequencePatterns();

  //update guest sequence pattern index to host sequence pattern index
  for(int i =0; i<sequence_patterns.size(); i++)
    {
      if(sequence_patterns[i] == discovered_patterns[guest_pattern_index])
        {
          sequence_patterns[i] = discovered_patterns[host_pattern_index];
        }
    }


  //marked guest pattern as invalid (-1) since it is merge with host pattern index
  discovered_patterns[guest_pattern_index] = -1;
  //set updated discovered pattern back
  featureContainer->setDiscoveredPatterns(discovered_patterns);
  //set updated sequence patterns back
  featureContainer->setSequencePatterns(sequence_patterns);
}

/**
 * @brief HierarchalAgglomerativeClustering::optimizedUpdatePatterns_
 * @param featureContainer
 * @param proximity_matrix
 * @param host_pattern_index
 * @param guest_pattern_index
 */
void HierarchalAgglomerativeClustering::optimizedUpdatePatterns_(FeatureContainer *featureContainer,vector<vector<float> >& proximity_matrix,int& host_pattern_index, int& guest_pattern_index)
{
  logging::INFO("optimizedUpdatePatterns_");

  //prepare host pattern info
  vector<int> discovered_patterns = featureContainer->getDiscoveredPatterns();
  vector<int> sequence_patterns = featureContainer->getSequencePatterns();

  stringstream message;
  message<<"before erase->discovered_pattern_size:"<<discovered_patterns.size()<<"\t"<<"proximity_matrix_size:"<<proximity_matrix.size()<<endl;
  logging::INFO(message.str());

  //update guest sequence pattern index to host sequence pattern index
  for(int i =0; i<sequence_patterns.size(); i++)
    {
      if(sequence_patterns[i] == discovered_patterns[guest_pattern_index])
        {
          sequence_patterns[i] = discovered_patterns[host_pattern_index];
        }
    }

  //update host pattern index if needed
  if(guest_pattern_index<host_pattern_index)
    {
      --host_pattern_index;
    }

  //delete guest discovered pattern
  discovered_patterns.erase(discovered_patterns.begin()+guest_pattern_index);

  //delete row from matrix
  proximity_matrix.erase(proximity_matrix.begin()+guest_pattern_index);

  for(int i =0; i<proximity_matrix.size();i++)
    {
      vector<float> row = proximity_matrix[i];
      //delete colum from row
      row.erase(row.begin()+guest_pattern_index);
      //assign updated row to the matrix
      proximity_matrix[i] =row;
    }

  message.clear();
  message<<"after erase->discovered_pattern_size:"<<discovered_patterns.size()<<"\t"<<"proximity_matrix_size:"<<proximity_matrix.size()<<endl;
  logging::INFO(message.str());

  //save discovered pattern of the host pattern index
  int host_discovered_pattern = discovered_patterns[host_pattern_index];
  //sort discovered patterns and proximity matrix
  sorterProcessor_->radixSortForProximityMatrix(discovered_patterns,proximity_matrix);
  //retrive index of the host discovered pattern using the saved discovered pattern
  host_pattern_index =  find(discovered_patterns.begin(), discovered_patterns.end(), host_discovered_pattern) - discovered_patterns.begin();

  message.clear();
  message<<"after s->discovered_pattern_size:"<<discovered_patterns.size()<<"\t"<<"proximity_matrix_size:"<<proximity_matrix.size()<<endl;
  logging::INFO(message.str());

  if(discovered_patterns.size()!=proximity_matrix.size())
    {
      stringstream message;
      message<<"proximity_matrix is not equal to discovered pattern size"<<endl;
      logging::ERROR(message.str());
      std::exit(0);
    }
  //set updated discovered pattern back
  featureContainer->setDiscoveredPatterns(discovered_patterns);
  //set updated sequence patterns back
  featureContainer->setSequencePatterns(sequence_patterns);
}

/**
 * @brief HierarchalAgglomerativeClustering::computePatternsToMerge
 * @param proximity_matrix
 * @param host_pattern
 * @param guest_pattern
 * @param similarity
 */
void HierarchalAgglomerativeClustering::computePatternsToMerge_(vector<vector<float> >& proximity_matrix, int &host_pattern_index, int &guest_pattern_index, float &similarity)
{
  logging::INFO("computePatternsToMerge_");
  vector<float> max_similarity_values_per_row;
  vector<int>  host_pattern_col,guest_pattern_row;

  for(int i =0; i<proximity_matrix.size();i++)
    {
      vector<float> proximity_row=proximity_matrix[i];

      //find max similarity value in row (guest pattern)
      auto max_guest_pattern_similarity_value = std::max_element(std::begin(proximity_row), std::end(proximity_row));
      int max_guest_pattern_similarity_index=std::distance(std::begin(proximity_row), max_guest_pattern_similarity_value);

      //save the max similarity value from row (guest pattern)
      max_similarity_values_per_row.push_back(proximity_row[max_guest_pattern_similarity_index]);
      //save index of row (guest pattern index)
      guest_pattern_row.push_back(max_guest_pattern_similarity_index);
      //save index of column (host pattern index)
      host_pattern_col.push_back(i);
    }

  //find max similarity value in col (host pattern)
  auto max_host_similarity_value = std::max_element(std::begin(max_similarity_values_per_row), std::end(max_similarity_values_per_row));
  int max_host_similarity_index=std::distance(std::begin(max_similarity_values_per_row), max_host_similarity_value);


  //save index of host pattern that has max similarity value in proximity matrix
  host_pattern_index = host_pattern_col[max_host_similarity_index];
  //save index of guest pattern that has max similarity value in proximity matrix
  //(+host_pattern_index) term to get to the right side of matrix
  guest_pattern_index = guest_pattern_row[max_host_similarity_index];
  //save max similarity value from proximity matrix
  similarity = max_similarity_values_per_row[max_host_similarity_index];

  logging::INFO("host_pattern_index:"+std::to_string(host_pattern_index)+
                "\t"+
                "guest_pattern_index:"+std::to_string(guest_pattern_index)+
                "\t"+
                "similarity:"+std::to_string(similarity));

  logging::INFO("1-proximity[host_index][guest_index]:"+std::to_string(proximity_matrix[host_pattern_index][guest_pattern_index]));

}
