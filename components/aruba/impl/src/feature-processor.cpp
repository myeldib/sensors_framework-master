#include<feature-processor.h>

/*
 * feature-reader.cpp
 *
 *  Created on: 2018-04-15
 *      Author: Mohamed Eldib
 * This class processes the sensor durations, sequence patterns and discovered patterns:
 * 1- It computes the average sensor duration per pattern.
 * 2- It finds the most common activity label per pattern.
 */

/**
 * @brief FeatureProcessor::FeatureProcessor
 * @param home
 */
FeatureProcessor::FeatureProcessor(Home* home)
{
  COUT<<"FeatureProcessor"<<endl;
  logging::INFO("FeatureProcessor");

  this->home_ = home;

  home_->readHomeSetup();

  sorterProcesser_ = new SorterProcessor();
}

/**
 * @brief FeatureProcessor::~FeatureProcessor
 */
FeatureProcessor::~FeatureProcessor()
{
  COUT<<"~FeatureProcessor"<<endl;
  logging::INFO("~FeatureProcessor");

  if(home_)
    {
      delete home_;
    }

  if(sorterProcesser_)
    {
      delete sorterProcesser_;
    }

}

/**
 * @brief FeatureProcessor::computeAverageSensorDurationPerPattern
 */
void FeatureProcessor::computeAverageSensorDurationPerPattern(FeatureContainer* featureContainer)
{
  COUT<<"computeAverageSensorDurationPerPattern"<<endl;
  logging::INFO("computeAverageSensorDurationPerPattern");

  vector<int>  discovered_patterns=featureContainer->getDiscoveredPatterns();
  vector<int>  sequence_patterns=featureContainer->getSequencePatterns();
  vector<vector<float> > all_sensor_durations=featureContainer->getSensorDurations();

  vector<vector<float> > average_sensor_durations_per_pattern;


  for (int i=0; i<discovered_patterns.size();i++)
    {
      int pattern_length_counter=0;

      vector<float> tmp_avg_sensor_duration_per_pattern(all_sensor_durations[0].size(),0);

      //pattern state holders
      vector<int> inside_pattern_state;
      vector<int> outside_pattern_state;

      for(int j=0;j<sequence_patterns.size();j++)
        {
          if(discovered_patterns[i]==sequence_patterns[j])
            {
              vector<float> sensor_durations=all_sensor_durations[j];

              //we exclude patten state from the calculations
              // size()-2 = inside pattern state
              // size()-1 = outside pattern state
              for(int c=0;c<sensor_durations.size()-2;c++)
                {
                  tmp_avg_sensor_duration_per_pattern[c]+=(1.0*sensor_durations[c]);
                }

              inside_pattern_state.push_back(sensor_durations[sensor_durations.size()-2]);
              outside_pattern_state.push_back(sensor_durations[sensor_durations.size()-1]);

              pattern_length_counter++;
            }

        }

      //get most repeated pattern state
      int max_repeated_inside_pattern_state =0;
      Common::getMostCommonElementInVector(inside_pattern_state,max_repeated_inside_pattern_state);

      int max_repeated_outside_pattern_state = 0;
      Common::getMostCommonElementInVector(outside_pattern_state,max_repeated_outside_pattern_state);

      //we exclude patten state from the calculations
      // size()-2 = inside pattern state
      // size()-1 = outside pattern state
      for(int c=0;c<tmp_avg_sensor_duration_per_pattern.size()-2;c++)
        {
          tmp_avg_sensor_duration_per_pattern[c]= (tmp_avg_sensor_duration_per_pattern[c]*1.0)/(pattern_length_counter*1.0);

        }

      //set most repeat pattern state
      tmp_avg_sensor_duration_per_pattern[tmp_avg_sensor_duration_per_pattern.size()-2] = max_repeated_inside_pattern_state;
      tmp_avg_sensor_duration_per_pattern[tmp_avg_sensor_duration_per_pattern.size()-1] = max_repeated_outside_pattern_state;


      average_sensor_durations_per_pattern.push_back(tmp_avg_sensor_duration_per_pattern);
    }


  featureContainer->setAverageSensorDurationsPerPattern(average_sensor_durations_per_pattern);
}

/**
 * @brief FeatureProcessor::computeOptimizedAverageSensorDurationPerPattern
 * @param featureContainer
 */
void FeatureProcessor::computeOptimizedAverageSensorDurationPerPattern(FeatureContainer* featureContainer)
{
  COUT<<"computeOptimizedAverageSensorDurationPerPattern"<<endl;
  logging::INFO("computeOptimizedAverageSensorDurationPerPattern");



  vector<int>  sequence_patterns=featureContainer->getSequencePatterns();
  vector<vector<float> > all_sensor_durations=featureContainer->getSensorDurations();

  vector<vector<float> > average_sensor_durations_per_pattern;
  //tmp holders, to be reset after each iterations
  int pattern_length_counter=0;
  vector<float> tmp_avg_sensor_duration_per_pattern(all_sensor_durations[0].size(),0);
  //pattern state holders
  vector<int> inside_pattern_state;
  vector<int> outside_pattern_state;

  int tmp_sequence_pattern = sequence_patterns[0];
  for(int i=0;i<sequence_patterns.size();i++)
    {
      if(tmp_sequence_pattern==sequence_patterns[i])
        {
          vector<float> sensor_durations=all_sensor_durations[i];

          //we exclude patten state from the calculations
          // size()-2 = inside pattern state
          // size()-1 = outside pattern state
          for(int c=0;c<sensor_durations.size()-2;c++)
            tmp_avg_sensor_duration_per_pattern[c]+=(1.0*sensor_durations[c]);

          inside_pattern_state.push_back(sensor_durations[sensor_durations.size()-2]);
          outside_pattern_state.push_back(sensor_durations[sensor_durations.size()-1]);

          pattern_length_counter++;
        }
      else
        {
          //get most repeated pattern state
          int max_repeated_inside_pattern_state =0;
          Common::getMostCommonElementInVector(inside_pattern_state,max_repeated_inside_pattern_state);

          int max_repeated_outside_pattern_state = 0;
          Common::getMostCommonElementInVector(outside_pattern_state,max_repeated_outside_pattern_state);

          //we exclude patten state from the calculations
          // size()-2 = inside pattern state
          // size()-1 = outside pattern state
          for(int c=0;c<tmp_avg_sensor_duration_per_pattern.size()-2;c++)
            tmp_avg_sensor_duration_per_pattern[c]= (tmp_avg_sensor_duration_per_pattern[c]*1.0)/(pattern_length_counter*1.0);

          //set most repeat pattern state
          tmp_avg_sensor_duration_per_pattern[tmp_avg_sensor_duration_per_pattern.size()-2] = max_repeated_inside_pattern_state;
          tmp_avg_sensor_duration_per_pattern[tmp_avg_sensor_duration_per_pattern.size()-1] = max_repeated_outside_pattern_state;

          average_sensor_durations_per_pattern.push_back(tmp_avg_sensor_duration_per_pattern);

          //reset tmp holders
          pattern_length_counter = 0;
          inside_pattern_state.clear();
          outside_pattern_state.clear();
          std::vector<float>(all_sensor_durations[0].size()).swap(tmp_avg_sensor_duration_per_pattern);

          //update sequence pattern
          tmp_sequence_pattern = sequence_patterns[i];

          //redo computation for different sequence pattern
          vector<float> sensor_durations=all_sensor_durations[i];

          for(int c=0;c<sensor_durations.size()-2;c++)
            tmp_avg_sensor_duration_per_pattern[c]+=(1.0*sensor_durations[c]);

          inside_pattern_state.push_back(sensor_durations[sensor_durations.size()-2]);
          outside_pattern_state.push_back(sensor_durations[sensor_durations.size()-1]);

          pattern_length_counter++;

        }

    }

  //redo computation for last pattern
  int max_repeated_inside_pattern_state =0;
  Common::getMostCommonElementInVector(inside_pattern_state,max_repeated_inside_pattern_state);

  int max_repeated_outside_pattern_state = 0;
  Common::getMostCommonElementInVector(outside_pattern_state,max_repeated_outside_pattern_state);


  for(int c=0;c<tmp_avg_sensor_duration_per_pattern.size()-2;c++)
    tmp_avg_sensor_duration_per_pattern[c]= (tmp_avg_sensor_duration_per_pattern[c]*1.0)/(pattern_length_counter*1.0);

  tmp_avg_sensor_duration_per_pattern[tmp_avg_sensor_duration_per_pattern.size()-2] = max_repeated_inside_pattern_state;
  tmp_avg_sensor_duration_per_pattern[tmp_avg_sensor_duration_per_pattern.size()-1] = max_repeated_outside_pattern_state;

  average_sensor_durations_per_pattern.push_back(tmp_avg_sensor_duration_per_pattern);

  featureContainer->setAverageSensorDurationsPerPattern(average_sensor_durations_per_pattern);
}
/**
 * @brief FeatureProcessor::computePatternsLength
 * @param featureContainer
 */
void FeatureProcessor::computePatternsLength(FeatureContainer *featureContainer)
{

  logging::INFO("computePatternsLength");

  vector<int> discovered_patterns = featureContainer->getDiscoveredPatterns();
  vector<int> sequence_patterns = featureContainer->getSequencePatterns();
  vector<int> patterns_length;

  for(int i=0; i<discovered_patterns.size();i++)
    {
      int pattern_length = 0;


      for(int j = 0 ;j<sequence_patterns.size();j++)
        {
          if(discovered_patterns[i] == sequence_patterns[j])
            {
              pattern_length++;
            }
        }


      patterns_length.push_back(pattern_length);
    }

  featureContainer->setPatternsLength(patterns_length);

}

/**
 * @brief FeatureProcessor::computeMostCommonActivityLabelPerPattern
 * @param featureContainer
 */
void FeatureProcessor::computeMostCommonActivityLabelPerPattern(FeatureContainer *featureContainer)
{
  COUT<<"computeMostCommonActivityLabelPerPattern"<<endl;
  logging::INFO("computeMostCommonActivityLabelPerPattern");

  vector<string> activity_labels=featureContainer->getActivityLabel();
  vector<int>  discovered_patterns=featureContainer->getDiscoveredPatterns();
  vector<int>  sequence_patterns=featureContainer->getSequencePatterns();

  //default is "-", no activity assigned or labeled
  vector<int> most_common_activity_label(discovered_patterns.size(),11);
  vector<vector<int> > activity_labels_per_pattern;

  for (int i=0; i<discovered_patterns.size();i++)
    {
      vector<int> tmp_activity_label(home_->getActivityLabelIntStringMap().size(),0);

      for(int j=0; j<sequence_patterns.size();j++)
        {
          if(discovered_patterns[i]==sequence_patterns[j])
            {
              string activity = "";

              if(activity_labels[j] !="-")
                {
                  activity= activity_labels[j];
                }

              tmp_activity_label[home_->getActivityLabelStringIntMap().at(activity)]++;
            }
        }

      auto mostCommonActivityLabelPerPattern = std::max_element(std::begin(tmp_activity_label), std::end(tmp_activity_label));
      int mostCommonActivityLabelPerPatternIndex=std::distance(std::begin(tmp_activity_label), mostCommonActivityLabelPerPattern);
      most_common_activity_label[i]=mostCommonActivityLabelPerPatternIndex;


      activity_labels_per_pattern.push_back(tmp_activity_label);

    }

  featureContainer->setActivityLabelsPerPattern(activity_labels_per_pattern);
  featureContainer->setMostCommonActivityLabelPerPattern(most_common_activity_label);
}

/**
 * @brief FeatureProcessor::computeOptimizedMostCommonActivityLabelPerPattern
 * @param featureContainer
 */
void FeatureProcessor::computeOptimizedMostCommonActivityLabelPerPattern(FeatureContainer *featureContainer)
{
  COUT<<"computeOptimizedMostCommonActivityLabelPerPattern"<<endl;
  logging::INFO("computeOptimizedMostCommonActivityLabelPerPattern");



  vector<string> activity_labels=featureContainer->getActivityLabel();
  vector<int>  discovered_patterns=featureContainer->getDiscoveredPatterns();
  vector<int>  sequence_patterns=featureContainer->getSequencePatterns();

  //default is "-", no activity assigned or labeled
  vector<int> most_common_activity_label(discovered_patterns.size(),11);
  vector<vector<int> > activity_labels_per_pattern;

  //tmp holder, needs to be reset after each iteration
  vector<int> tmp_activity_label(home_->getActivityLabelIntStringMap().size(),0);
  int most_common_activity_index = 0;

  int tmp_sequence_pattern = sequence_patterns[0];
  for(int i=0; i<sequence_patterns.size();i++)
    {
      if(tmp_sequence_pattern==sequence_patterns[i])
        {
          string activity = "";

          if(activity_labels[i] !="-")
            activity= activity_labels[i];


          tmp_activity_label[home_->getActivityLabelStringIntMap().at(activity)]++;
        }
      else
        {
          auto mostCommonActivityLabelPerPattern = std::max_element(std::begin(tmp_activity_label), std::end(tmp_activity_label));
          int mostCommonActivityLabelPerPatternIndex=std::distance(std::begin(tmp_activity_label), mostCommonActivityLabelPerPattern);
          most_common_activity_label[most_common_activity_index]=mostCommonActivityLabelPerPatternIndex;

          activity_labels_per_pattern.push_back(tmp_activity_label);

          //reset
          std::vector<int>(home_->getActivityLabelIntStringMap().size()).swap(tmp_activity_label);

          //update sequence pattern
          tmp_sequence_pattern = sequence_patterns[i];
          most_common_activity_index++;

          //redo computation for different sequence pattern
          string activity = "";

          if(activity_labels[i] !="-")
            activity= activity_labels[i];

          tmp_activity_label[home_->getActivityLabelStringIntMap().at(activity)]++;

        }
    }

  //redo computation for last pattern
  auto mostCommonActivityLabelPerPattern = std::max_element(std::begin(tmp_activity_label), std::end(tmp_activity_label));
  int mostCommonActivityLabelPerPatternIndex=std::distance(std::begin(tmp_activity_label), mostCommonActivityLabelPerPattern);
  most_common_activity_label[most_common_activity_index]=mostCommonActivityLabelPerPatternIndex;

  activity_labels_per_pattern.push_back(tmp_activity_label);


  featureContainer->setActivityLabelsPerPattern(activity_labels_per_pattern);
  featureContainer->setMostCommonActivityLabelPerPattern(most_common_activity_label);
}

/**
 * @brief FeatureProcessor::sortPatternsUsingSimilarityScores
 * @param featureContainer
 */
void FeatureProcessor::sortPatternsUsingSimilarityScores(FeatureContainer *featureContainer)
{
  COUT<<"sortPatternsUsingSimilarityScores"<<endl;
  logging::INFO("sortPatternsUsingSimilarityScores");

  vector<vector<int> > patterns_to_merge =featureContainer->getPatternsToMerge();
  vector<int> discovered_patterns = featureContainer->getDiscoveredPatterns();
  vector<vector<float> > similarity_score_per_pattern = featureContainer->getSimilarityScorePerPattern();
  vector<float> avg_similarity_score_per_pattern;


  computeAverageSimilarityScorePerPattern_(similarity_score_per_pattern,avg_similarity_score_per_pattern);

  //printPatterns_(patterns_to_merge,discovered_patterns,avg_similarity_score_per_pattern);

  sortPatterns_(patterns_to_merge, discovered_patterns,avg_similarity_score_per_pattern);

  removeDuplicatePatternsToMerge_(patterns_to_merge);

  //printPatterns_(patterns_to_merge,discovered_patterns,avg_similarity_score_per_pattern);

  featureContainer->setSortedPatternsToMerge(patterns_to_merge);
  featureContainer->setSortedDiscoveredPatterns(discovered_patterns);

}

/**
 * @brief FeatureProcessor::computeAverageSimilarityScorePerPattern_
 * @param similarity_score_per_pattern
 * @param avg_similarity_score_per_pattern
 */
void FeatureProcessor::computeAverageSimilarityScorePerPattern_(vector<vector<float> > &similarity_score_per_pattern, vector<float> &avg_similarity_score_per_pattern)
{
  COUT<<"computeAverageSimilarityScorePerPattern_"<<endl;
  logging::INFO("computeAverageSimilarityScorePerPattern_");

  for(int i = 0 ; i<similarity_score_per_pattern.size();i++)
    {
      vector<float> similarity_score = similarity_score_per_pattern[i];
      float avg_similarity_score = 0.0;

      for(int j = 0; j<similarity_score.size();j++)
        {
          avg_similarity_score+=similarity_score[j];
        }

      avg_similarity_score = avg_similarity_score/(similarity_score.size()*1.0);

      avg_similarity_score_per_pattern.push_back(avg_similarity_score);
    }
}

/**
 * @brief FeatureProcessor::sortPatterns
 * @param patterns_to_merge
 * @param avg_similarity_score
 */
void FeatureProcessor::sortPatterns_(vector<vector<int> > &patterns_to_merge, vector<int>& discovered_patterns, vector<float> &avg_similarity_score)
{
  COUT<<"sortPatterns_"<<endl;
  logging::INFO("sortPatterns_");

  int i, j, flag = 1;    // set flag to 1 to start first pass

  float tm_avg_similarity_score;
  int tmp_discovered_pattern;
  vector<int> temp_pattern_to_merge;

  int numLength = avg_similarity_score.size();

  for(i = 0; (i < numLength) && flag; i++)
    {
      flag = 0;

      for (j=1; j <numLength; j++)
        {
          if (avg_similarity_score[j] < avg_similarity_score[j-1])      // ascending order simply changes to <
            {
              tm_avg_similarity_score = avg_similarity_score[j-1];             // swap elements
              avg_similarity_score[j-1] = avg_similarity_score[j];
              avg_similarity_score[j] = tm_avg_similarity_score;

              tmp_discovered_pattern = discovered_patterns[j-1];             // swap elements
              discovered_patterns[j-1] = discovered_patterns[j];
              discovered_patterns[j] = tmp_discovered_pattern;


              temp_pattern_to_merge = patterns_to_merge[j-1];             // swap elements
              patterns_to_merge[j-1] = patterns_to_merge[j];
              patterns_to_merge[j]= temp_pattern_to_merge;

              flag = 1;               // indicates that a swap occurred.
            }
        }
    }
}

/**
 * @brief FeatureProcessor::printPatterns_
 * @param patterns_to_merge
 * @param avg_similarity_score_per_pattern
 */
void FeatureProcessor::printPatterns_(vector<vector<int> > &patterns_to_merge, vector<int>& discovered_patterns, vector<float> &avg_similarity_score_per_pattern)
{
  COUT<<"printPatterns_"<<endl;
  logging::INFO("printPatterns_");

  COUT<<"Size:"<<patterns_to_merge.size()<<"\t"<<avg_similarity_score_per_pattern.size()<<"\t"<<discovered_patterns.size()<<endl;

  for(int i = 0; i<patterns_to_merge.size();i++)
    {
      vector<int> pattern_index = patterns_to_merge[i];
      float avg_similarity_score = avg_similarity_score_per_pattern[i];

      for(int j = 0; j<pattern_index.size();j++)
        {
          COUT<<discovered_patterns[i]<<"\t"<<pattern_index[j]<<"\t"<<avg_similarity_score<<endl;
          logging::INFO(std::to_string(discovered_patterns[i])+"\t"+
                        std::to_string(pattern_index[j])+"\t"+
                        std::to_string(avg_similarity_score));
        }
    }
}

/**
 * @brief FeatureProcessor::removeDuplicatePatternsToMerge_
 * @param patterns_to_merge
 */
void FeatureProcessor::removeDuplicatePatternsToMerge_(vector<vector<int> > &patterns_to_merge)
{
  COUT<<"removeDuplicatePatternsToMerge_"<<endl;
  logging::INFO("removeDuplicatePatternsToMerge_");

  vector<int> is_merged_patterns;

  for(int i = 0; i<patterns_to_merge.size();i++)
    {
      vector<int> sequence_patterns = patterns_to_merge[i];

      for(int j = 0; j<sequence_patterns.size();j++)
        {
          //initalize
          if(is_merged_patterns.size() == 0)
            {
              is_merged_patterns.push_back(sequence_patterns[j]);
            }
          else
            {
              bool is_merged = false;
              //mark duplicate patterns with -1 index
              for(int k = 0; k<is_merged_patterns.size();k++)
                {

                  if(is_merged_patterns[k] == sequence_patterns[j] && is_merged_patterns[k] != -1 )
                    {
                      is_merged= true;
                      sequence_patterns[j] = -1;
                    }
                }
              //add it to unique patterns if has not seen before
              if(!is_merged)
                {
                  is_merged_patterns.push_back(sequence_patterns[j]);
                }

            }
        }

      //remove marked duplicate patterns with index -1
      sequence_patterns.erase(std::remove(sequence_patterns.begin(), sequence_patterns.end(), -1), sequence_patterns.end());

      patterns_to_merge[i] =  sequence_patterns;

    }
}

/**
 * @brief FeatureProcessor::computeActiveSensors
 * @param featureContainer
 */
void FeatureProcessor::computeActiveSensors(FeatureContainer *featureContainer,bool include_non_active_sensors)
{
  COUT<<"computeActiveSensors"<<endl;
  logging::INFO("computeActiveSensors");

  vector<vector<float> > avg_sensor_durations_per_pattern = featureContainer->getAverageSensorDurationsPerPattern();

  vector<vector<int> > active_sensors_per_pattern;


  for(int i =0; i <avg_sensor_durations_per_pattern.size(); i++)
    {
      vector<float> avg_sensor_durations = avg_sensor_durations_per_pattern[i];

      vector<int> active_sensors;

      for(int j = 0; j<avg_sensor_durations.size(); j++)
        {
          //if sensor j has avg duration for pattern i more than 0 seconds
          //then indicate sensor j as an active sensor, and add it to list of active sensors

          if(avg_sensor_durations[j]>0)
            {
              active_sensors.push_back(j);
            }
          else
            {
              if(include_non_active_sensors)
                {
                  //model recognition requires to have equla size active sensor length (features)
                  //we set here non active sensor with -1 to indicate that these sensors are off
                  active_sensors.push_back(-1);
                }
            }
        }

      active_sensors_per_pattern.push_back(active_sensors);
    }

  featureContainer->setActiveSensorsPerPattern(active_sensors_per_pattern);
}

/**
 * @brief FeatureProcessor::computeMostAssignedTimeIndex
 * @param featureContainer
 */
void FeatureProcessor::computeMostAssignedTimeIndex(FeatureContainer *featureContainer)
{
  COUT<<"computeMostAssignedTimeIndex"<<endl;
  logging::INFO("computeMostAssignedTimeIndex");

  vector<int> sequence_patterns = featureContainer->getSequencePatterns();
  vector<int> discovered_patterns = featureContainer->getDiscoveredPatterns();
  vector<float> time_index_per_pattern = featureContainer->getTimeIndexPerPattern();

  vector<int> most_assigned_time_index(discovered_patterns.size(),0);

  for(int i=0;i<discovered_patterns.size();i++)
    {

      vector<int> assigned_time_index;
      for(int j=0;j<sequence_patterns.size();j++)
        {

          if(discovered_patterns[i]==sequence_patterns[j])
            {
              assigned_time_index.push_back(time_index_per_pattern[j]);
            }
        }

      if(assigned_time_index.size()>0)
        {
          auto mostAssignedTimeIndexPerPattern = std::max_element(std::begin(assigned_time_index), std::end(assigned_time_index));
          int mostAssignedTimeIndexPerPatternInex=std::distance(std::begin(assigned_time_index), mostAssignedTimeIndexPerPattern);

          most_assigned_time_index.push_back(assigned_time_index[mostAssignedTimeIndexPerPatternInex]);
        }

    }

  featureContainer->setMostAssginedTimeIndexPerPatternInHourIndex(most_assigned_time_index);
}

/**
 * @brief FeatureProcessor::computeOptimizedMostAssignedTimeIndex
 * @param featureContainer
 */
void FeatureProcessor::computeOptimizedMostAssignedTimeIndex(FeatureContainer *featureContainer)
{
  COUT<<"computeOptimizedMostAssignedTimeIndex"<<endl;
  logging::INFO("computeOptimizedMostAssignedTimeIndex");



  vector<int> sequence_patterns = featureContainer->getSequencePatterns();
  vector<int> discovered_patterns = featureContainer->getDiscoveredPatterns();
  vector<float> time_index_per_pattern = featureContainer->getTimeIndexPerPattern();

  vector<int> most_assigned_time_index(discovered_patterns.size(),0);

  int tmp_sequence_pattern = sequence_patterns[0];
  //tmp holders to be reset after each iterations
  vector<int> assigned_time_index;
  for(int i=0;i<sequence_patterns.size();i++)
    {

      if(tmp_sequence_pattern==sequence_patterns[i])
        {
          assigned_time_index.push_back(time_index_per_pattern[i]);
        }
      else
        {

          if(assigned_time_index.size()>0)
            {
              auto mostAssignedTimeIndexPerPattern = std::max_element(std::begin(assigned_time_index), std::end(assigned_time_index));
              int mostAssignedTimeIndexPerPatternInex=std::distance(std::begin(assigned_time_index), mostAssignedTimeIndexPerPattern);

              most_assigned_time_index.push_back(assigned_time_index[mostAssignedTimeIndexPerPatternInex]);
            }

          //reset
          assigned_time_index.clear();
          //update sequence pattern
          tmp_sequence_pattern = sequence_patterns[i];

          assigned_time_index.push_back(time_index_per_pattern[i]);
        }
    }

  //redo computations for the last pattern
  if(assigned_time_index.size()>0)
    {
      auto mostAssignedTimeIndexPerPattern = std::max_element(std::begin(assigned_time_index), std::end(assigned_time_index));
      int mostAssignedTimeIndexPerPatternInex=std::distance(std::begin(assigned_time_index), mostAssignedTimeIndexPerPattern);

      most_assigned_time_index.push_back(assigned_time_index[mostAssignedTimeIndexPerPatternInex]);
    }

  featureContainer->setMostAssginedTimeIndexPerPatternInHourIndex(most_assigned_time_index);
}

/**
 * @brief FeatureProcessor::mergePatterns
 * @param day_index
 * @param new_initial_pattern_index
 * @param merged_fc
 * @param fc
 */
void FeatureProcessor::mergePatterns(int day_index,int new_initial_pattern_index, FeatureContainer *merged_fc, FeatureContainer *fc)
{

  COUT<<"mergePatterns"<<endl;
  logging::INFO("mergePatterns");

  if(day_index==0)
    {
      day_index+=1;
    }

  int new_pattern_index = new_initial_pattern_index*(day_index);


  //holders for the host and guest after merging
  vector<int> new_sequence_patterns;
  vector<int> new_discovered_patterns;
  vector<vector<float> > new_sensors_duration;
  vector<float> new_time_index_per_pattern;
  vector<string> new_day_name_per_pattern;
  vector<string> new_activity_labels;

  updateOptimizedHostPatterns_(new_pattern_index,merged_fc,
                      new_sequence_patterns,
                      new_discovered_patterns,
                      new_sensors_duration,
                      new_time_index_per_pattern,
                      new_day_name_per_pattern,
                      new_activity_labels);

  vector<vector<int> >guest_patterns_to_merge_indexes = merged_fc->getSortedPatternsToMerge();

  updateGuestPatterns_(fc,
                       guest_patterns_to_merge_indexes,
                       new_sequence_patterns,
                       new_discovered_patterns,
                       new_sensors_duration,
                       new_time_index_per_pattern,
                       new_day_name_per_pattern,
                       new_activity_labels);



  COUT<<new_discovered_patterns.size()<<
        "\t"<<new_sequence_patterns.size()<<
        "\t"<<new_sensors_duration.size()<<
        "\t"<<new_time_index_per_pattern.size()<<
        "\t"<<new_day_name_per_pattern.size()<<
        "\t"<<new_activity_labels.size()<<endl;

  logging::INFO(std::to_string(new_discovered_patterns.size())+"\t"+
                std::to_string(new_sequence_patterns.size())+"\t"+
                std::to_string(new_sensors_duration.size())+"\t"+
                std::to_string(new_time_index_per_pattern.size())+"\t"+
                std::to_string(new_day_name_per_pattern.size())+"\t"+
                std::to_string(new_activity_labels.size())+"\t");

  merged_fc->setDiscoveredPatterns(new_discovered_patterns);
  merged_fc->setSequencePatterns(new_sequence_patterns);
  merged_fc->setSensorDurations(new_sensors_duration);
  merged_fc->setTimeIndexPerPattern(new_time_index_per_pattern);
  merged_fc->setDayNamePerPattern(new_day_name_per_pattern);
  merged_fc->setActivityLabel(new_activity_labels);
}

/**
 * @brief FeatureProcessor::updateHostPatterns_
 * @param new_pattern_index
 * @param merged_fc
 * @param new_sequence_patterns
 * @param new_discovered_patterns
 * @param new_sensor_durations
 * @param new_time_index_per_pattern
 * @param new_activity_labels
 */
void FeatureProcessor::updateHostPatterns_(int new_pattern_index,
                                           FeatureContainer *merged_fc,
                                           vector<int>& new_sequence_patterns,
                                           vector<int>& new_discovered_patterns,
                                           vector<vector<float> >& new_sensor_durations,
                                           vector<float>& new_time_index_per_pattern,
                                           vector<string>& new_day_name_per_pattern,
                                           vector<string>& new_activity_labels)
{

  COUT<<"updateHostPatterns_"<<endl;

  logging::INFO("updateHostPatterns_");

  vector<int> sorted_discovered_patterns = merged_fc->getSortedDiscoveredPatterns();
  vector<int> sequence_patterns = merged_fc->getSequencePatterns();
  vector<vector<float> > sensor_durations = merged_fc->getSensorDurations();
  vector<float> time_index_per_pattern = merged_fc->getTimeIndexPerPattern();
  vector<string> day_name_per_pattern = merged_fc->getDayNamePerPattern();
  vector<string> activitly_label = merged_fc->getActivityLabel();

  for(int i =0; i<sorted_discovered_patterns.size();i++)
    {
      //add new pattern index to discovered pattern
      new_discovered_patterns.push_back(new_pattern_index);

      for(int j = 0; j<sequence_patterns.size();j++)
        {
          if(sorted_discovered_patterns[i]==sequence_patterns[j])
            {

              //add new pattern index to each sequence pattern
              new_sequence_patterns.push_back(new_pattern_index);
              //assign sequence pattern information to new holders
              new_sensor_durations.push_back(sensor_durations[j]);
              new_time_index_per_pattern.push_back(time_index_per_pattern[j]);
              new_day_name_per_pattern.push_back(day_name_per_pattern[j]);
              new_activity_labels.push_back(activitly_label[j]);
            }
        }

      new_pattern_index++;
    }

}

/**
 * @brief FeatureProcessor::updateOptimizedHostPatterns_
 * @param new_pattern_index
 * @param merged_fc
 * @param new_sequence_patterns
 * @param new_discovered_patterns
 * @param new_sensor_durations
 * @param new_time_index_per_pattern
 * @param new_activity_labels
 */
void FeatureProcessor::updateOptimizedHostPatterns_(int new_pattern_index,
                                                    FeatureContainer *merged_fc,
                                                    vector<int>& new_sequence_patterns,
                                                    vector<int>& new_discovered_patterns,
                                                    vector<vector<float> >& new_sensor_durations,
                                                    vector<float>& new_time_index_per_pattern,
                                                    vector<string>& new_day_name_per_pattern,
                                                    vector<string>& new_activity_labels)
{

  COUT<<"updateOptimizedHostPatterns_"<<endl;

  logging::INFO("updateOptimizedHostPatterns_");

  sorterProcesser_->radixSort(merged_fc);

  vector<int> sequence_patterns = merged_fc->getSequencePatterns();
  vector<vector<float> > sensor_durations = merged_fc->getSensorDurations();
  vector<float> time_index_per_pattern = merged_fc->getTimeIndexPerPattern();
  vector<string> day_name_per_pattern = merged_fc->getDayNamePerPattern();
  vector<string> activitly_label = merged_fc->getActivityLabel();


  int tmp_sequence_pattern = sequence_patterns[0];

  for(int i = 0; i<sequence_patterns.size();i++)
    {
      if(tmp_sequence_pattern==sequence_patterns[i])
        {

          //add new pattern index to each sequence pattern
          new_sequence_patterns.push_back(new_pattern_index);
          //assign sequence pattern information to new holders
          new_sensor_durations.push_back(sensor_durations[i]);
          new_time_index_per_pattern.push_back(time_index_per_pattern[i]);
          new_day_name_per_pattern.push_back(day_name_per_pattern[i]);
          new_activity_labels.push_back(activitly_label[i]);
        }
      else
        {
          //add new pattern index to discovered pattern
          new_discovered_patterns.push_back(new_pattern_index);
          ++new_pattern_index;

          //update sequence pattern
          tmp_sequence_pattern = sequence_patterns[i];

          //add new pattern index to each sequence pattern
          new_sequence_patterns.push_back(new_pattern_index);
          //assign sequence pattern information to new holders
          new_sensor_durations.push_back(sensor_durations[i]);
          new_time_index_per_pattern.push_back(time_index_per_pattern[i]);
          new_day_name_per_pattern.push_back(day_name_per_pattern[i]);
          new_activity_labels.push_back(activitly_label[i]);

        }
    }

  //redo computations for the last pattern
  new_discovered_patterns.push_back(new_pattern_index);
  ++new_pattern_index;

}

/**
 * @brief FeatureProcessor::updateGuestPatterns_
 * @param fc
 * @param sorted_patterns_to_merge
 * @param new_sequence_patterns
 * @param new_discovered_patterns
 * @param new_sensors_duration
 * @param new_time_index_per_pattern
 * @param new_activity_labels
 */
void FeatureProcessor::updateGuestPatterns_(FeatureContainer *fc,
                                            vector<vector<int> > sorted_patterns_to_merge,
                                            vector<int> &new_sequence_patterns,
                                            vector<int> &new_discovered_patterns,
                                            vector<vector<float> > &new_sensor_durations,
                                            vector<float>& new_time_index_per_pattern,
                                            vector<string>& new_day_name_per_pattern,
                                            vector<string> &new_activity_labels)
{

  COUT<<"updateGuestPatterns_"<<endl;

  logging::INFO("updateGuestPatterns_");

  vector<int> discovered_patterns = fc->getDiscoveredPatterns();
  vector<int> sequence_patterns = fc->getSequencePatterns();
  vector<vector<float> > sensor_durations = fc->getSensorDurations();
  vector<float> time_index_per_pattern = fc->getTimeIndexPerPattern();
  vector<string> day_name_per_pattern = fc->getDayNamePerPattern();
  vector<string> activitly_labels = fc->getActivityLabel();



  //first map patterns to merge
  for(int i =0;i<sorted_patterns_to_merge.size();i++)
    {
      //get patterns to merge
      vector<int> patterns_to_map = sorted_patterns_to_merge[i];

      //loop through elements of patterns
      for(int j=0; j<patterns_to_map.size();j++)
        {
          bool is_mapped = false;

          COUT<<j<<"\t"<<patterns_to_map[j]<<endl;

          logging::INFO(std::to_string(j)+"\t"+
                        std::to_string(patterns_to_map[j]));

          //loop through guest patterns
          for(int k = 0 ;k <sequence_patterns.size();k++)
            {
              //pattern to map is not duplicate (marked with index -1)
              //pattern to map is the index of the discovered pattern
              if(patterns_to_map[j] !=-1 && discovered_patterns[patterns_to_map[j]]==sequence_patterns[k])
                {
                  is_mapped = true;
                  //map guest patterns data to merged patterns
                  new_sequence_patterns.push_back(new_discovered_patterns[i]);
                  new_sensor_durations.push_back(sensor_durations[k]);
                  new_time_index_per_pattern.push_back((time_index_per_pattern[k]));
                  new_day_name_per_pattern.push_back(day_name_per_pattern[k]);
                  new_activity_labels.push_back(activitly_labels[k]);

                }
            }
          //mark the guest pattern as updated
          if(is_mapped)
            {
              COUT<<"mapped:"<<discovered_patterns[patterns_to_map[j]]<<endl;
              logging::INFO("mapped:"+std::to_string(discovered_patterns[patterns_to_map[j]]));

              discovered_patterns[patterns_to_map[j]]= -1;
            }
        }
    }

  //get largest pattern index to use to update unmerged patterns
  int new_pattern_index = new_discovered_patterns[new_discovered_patterns.size()-1]+1;

  //then add unmerged patterns (unmapped patterns)
  for(int i =0; i<discovered_patterns.size();i++)
    {
      //pattern has not been mapped yet, then add
      if(discovered_patterns[i] != -1)
        {
          COUT<<"unmapped:"<<discovered_patterns[i]<<"\t"<<discovered_patterns.size()<<"\t"<<sequence_patterns.size()<<endl;

          //logging::INFO("unmapped:"+std::to_string(discovered_patterns[i]));
          bool include_pattern = false;

          for(int j = 0;j<sequence_patterns.size();j++)
            {
              if(discovered_patterns[i] == sequence_patterns[j])
                {
                  include_pattern = true;
                  //map guest patterns data to merged patterns
                  new_sequence_patterns.push_back(new_pattern_index);
                  new_sensor_durations.push_back(sensor_durations[j]);
                  new_time_index_per_pattern.push_back(time_index_per_pattern[j]);
                  new_day_name_per_pattern.push_back(day_name_per_pattern[j]);
                  new_activity_labels.push_back(activitly_labels[j]);
                }
            }

          if(include_pattern)
            {
              //assign new pattern index
              new_discovered_patterns.push_back(new_pattern_index);
              //update pattern index
              new_pattern_index++;
            }
        }
    }

}
