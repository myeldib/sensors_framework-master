#ifndef PATTERNSSTATISTICS_H
#define PATTERNSSTATISTICS_H
#include<vector>
#include<feature-writer.h>
#include<home.h>
#include<logger.h>

class PatternsStatistics{
public:
    PatternsStatistics(string feature_writer_path,Home* home,bool& success);
    ~PatternsStatistics();
    void computeBetweenDayPatternStatistics(int num_of_all_patterns, int num_of_pruned_patterns, vector<int> mostCommonActivityLabelPerPattern);
    void destroy();

private:
    void computeNumberOfDiscoveredPatterns_(int num_of_all_patternse);
    void computeOfNumberPrunedPatterns_(int num_of_patterns);
    void computeNumberOfDiscoveredActivities_(vector<int> mostCommonActivityLabelPerPattern);
    void init(string feature_writer_path,Home* home,bool& success);
private:
    std::vector<int> num_of_all_discovered_patterns_;
    std::vector<int> num_of_pruned_patterns_;
    std::vector<int> num_of_discovered_activity_;
    std::vector<bool> is_activity_discovered_;
    Home* home_;
    FeatureWriter* featureWriter;
    string feature_writer_path;
    bool success;

};

#endif // PATTERNSSTATISTICS_H
