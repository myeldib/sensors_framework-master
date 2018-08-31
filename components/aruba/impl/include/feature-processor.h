#ifndef FEATUREPROCESSOR_H
#define FEATUREPROCESSOR_H
#include<feature-container.h>
#include<sorter-processor.h>
#include<home.h>
#include<logger.h>

class FeatureProcessor {
public:
    FeatureProcessor(Home* home);
    ~FeatureProcessor();
    void computeAverageSensorDurationPerPattern(FeatureContainer* featureContainer);
    void computeOptimizedAverageSensorDurationPerPattern(FeatureContainer* featureContainer);
    void computeMostCommonActivityLabelPerPattern(FeatureContainer* featureContainer);
    void computeOptimizedMostCommonActivityLabelPerPattern(FeatureContainer* featureContainer);
    void computeActiveSensors(FeatureContainer* featureContainer,bool include_non_active_sensors);
    void computeMostAssignedTimeIndex(FeatureContainer* featureContainer);
    void computeOptimizedMostAssignedTimeIndex(FeatureContainer* featureContainer);
    void computePatternsLength(FeatureContainer* featureContainer);
    void sortPatternsUsingSimilarityScores(FeatureContainer* featureContainer);
    void mergePatterns(int day_index, int new_initial_pattern_index, FeatureContainer* merged_fc, FeatureContainer* fc);
private:
    void computeAverageSimilarityScorePerPattern_(vector<vector<float> >& similarity_score_per_pattern, vector<float>& avg_similarity_score_per_pattern);
    void sortPatterns_(vector<vector<int> >& patterns_to_merge, vector<int>& discovered_patterns, vector<float>& avg_similarity_score);
    void removeDuplicatePatternsToMerge_(vector<vector<int> >& patterns_to_merge);
    void printPatterns_(vector<vector<int> >& patterns_to_merge,vector<int>& discovered_patterns, vector<float>& avg_similarity_score);
    void updateHostPatterns_(int new_pattern_index,
                             FeatureContainer *merged_fc,
                             vector<int>& new_sequence_patterns,
                             vector<int>& new_discovered_patterns,
                             vector<vector<float> >& new_sensors_duration,
                             vector<float>& new_time_index_per_pattern,
                             vector<string>& new_day_name_per_pattern,
                             vector<string>& new_activity_labels);

    void updateOptimizedHostPatterns_(int new_pattern_index,
                             FeatureContainer *merged_fc,
                             vector<int>& new_sequence_patterns,
                             vector<int>& new_discovered_patterns,
                             vector<vector<float> >& new_sensors_duration,
                             vector<float>& new_time_index_per_pattern,
                             vector<string>& new_day_name_per_pattern,
                             vector<string>& new_activity_labels);

    void updateGuestPatterns_(FeatureContainer* fc,
                              vector<vector<int> > sorted_patterns_to_merge,
                              vector<int>& new_sequence_patterns,
                              vector<int>& new_discovered_patterns,
                              vector<vector<float> >& new_sensors_duration,
                              vector<float>& new_time_index_per_pattern,
                              vector<string>& new_day_name_per_pattern,
                              vector<string>& new_activity_labels);
private:
    Home* home_;
    SorterProcessor* sorterProcesser_;
};

#endif // FEATUREPROCESSOR_H
