#ifndef FEATURECONTAINER_H
#define FEATURECONTAINER_H
#include<utilities.h>

class FeatureContainer {
public:
  FeatureContainer();
  //read features
  void setActivityLabel(vector<string> activity_labels);
  void setSequencePatterns(vector<int> sequence_patterns);
  void setDiscoveredPatterns(vector<int> discovered_patterns);
  void setSensorDurations(vector<vector<float> > sensor_durations);
  void setMostAssginedTimeIndexPerPatternInHourIndex(vector<int> most_assigned_time_index_per_pattern);
  void setTimeIndexPerPattern(vector<float> time_index_per_pattern);
  void setDayNamePerPattern(vector<string> day_name_per_pattern);
  vector<string> getActivityLabel();
  vector<int> getDiscoveredPatterns();
  vector<int> getSequencePatterns();
  vector<int> getMostAssignedTimeIndexPerPatternInHourIndex();
  vector<float> getTimeIndexPerPattern();
  vector<vector<float> > getSensorDurations();
  vector<string> getDayNamePerPattern();

  //processed features per pattern
  void setAverageSensorDurationsPerPattern(vector<vector<float> > average_sensor_duration_per_pattern);
  vector<vector<float> > getAverageSensorDurationsPerPattern();
  void setActivityLabelsPerPattern(vector<vector<int> > activity_labels_per_pattern);
  vector<vector<int> > getActivityLabelsPerPattern();
  void setMostCommonActivityLabelPerPattern(vector<int> most_common_activity_label_per_pattern);
  vector<int> getMostCommonActivityLabelPerPattern();
  void setActiveSensorsPerPattern(vector<vector<int> > active_sensors_per_pattern);
  vector<vector<int> > getActiveSensorsPerPattern();
  void setPatternsLength(vector<int> patterns_length);
  vector<int> getPatternsLength();

  //information about patterns to merge
  void setPatternsToMerge(vector<vector<int> > patterns_to_merge);
  vector<vector<int> > getPatternsToMerge();
  void setSimilarityScorePerPattern(vector<vector<float> > similarity_score_per_pattern);
  vector<vector<float> > getSimilarityScorePerPattern();
  void setSortedPatternsToMerge(vector<vector<int> > sorted_patterns_to_merge);
  vector<vector<int> > getSortedPatternsToMerge();
  void setSortedDiscoveredPatterns(vector<int> sorted_discovered_patterns);
  vector<int> getSortedDiscoveredPatterns();

  //information about cluster recognition
  vector<int> getActualActivityLabels();
  void setActualActivityLabels(vector<int> actual_activity_labels);
  vector<int> getPredictedActivityLabels();
  void setPredictedActivityLabels(vector<int> predicted_activity_labels);
  vector<int> getPredictedDiscoveredPatterns();
  void setPredictedDiscoveredPatterns(vector<int> predicted_discovered_patterns);
  string getAccuracyResultsMessage();
  void setAccuracyResultsMessage(string accuracy_results_message);

  //Threading information
  void setThreadID(int thread_id);
  int getThreadID();

  ~FeatureContainer();
private:
  //read features
  vector<string> activity_labels;
  vector<int>sequence_patterns;
  vector<int> discovered_patterns;
  vector<int> most_assigned_time_index_per_pattern;
  vector<float> time_index_per_pattern;
  vector<vector<float> > sensor_durations;
  vector<string> day_name_per_pattern;

  //processed features per pattern
  vector<vector<float> > average_sensor_duration_per_pattern;
  vector<vector<int> > activity_labels_per_pattern;
  vector<int> most_common_activity_label_per_pattern;
  vector<vector<int> > active_sensors_per_pattern;
  vector<int> patterns_length;

  //information about patterns to merge
  vector<vector<int> > patterns_to_merge;
  vector<vector<float> > similarity_score_per_pattern;
  vector<vector<int> > sorted_patterns_to_merge;
  vector<int> sorted_discovered_patterns;

  //predicted activity labels
  vector<int> predicted_activity_labels;
  //actual activity labels
  vector<int> actual_activity_labels;
  //predicted discovered patterns
  vector<int> predicted_discovered_patterns;
  //holder for cluster recognition results;
  string accuracy_results_message;
  int thread_id;
};

#endif // FEATURECONTAINER_H
