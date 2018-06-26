#ifndef INFERENTIALDISCOVERY_H
#define INFERENTIALDISCOVERY_H
#include<DayContainer.h>
#include "SensorReader.h"
#include "Home.h"
#include <numeric>
#include<FeatureContainer.h>
#include<ClusterPatterns.h>

class InferentialDiscovery
{
public:
    InferentialDiscovery();

    void run();
private:
    int calculateTimeslotSegment(int win_size_minute);
    time_t initializeClock(string& day);
    vector<FeatureContrainer*> readFeatureFiles();
    vector<string> preparePaths(string &day);
    vector<vector<int> > computeChiSquareIndTest(FeatureContrainer* fc1, FeatureContrainer* fc2);
    //compute the ellapsed time for each pattern
    void generateChiSquareFeatures(FeatureContrainer* fc);
    void findMostCommonActivityPattern(FeatureContrainer* fc);
    FeatureContrainer* computeFeature(FeatureContrainer* dataset_feature);
    void displayDiscoveredPatternInfo(FeatureContrainer* fc);
    void computeFeatures(vector<FeatureContrainer*>& dataset_features);
    void withinPatternSimilarityTest(FeatureContrainer* fc);
    vector< vector<int> >betweenMergePatterns(int mult_ind,vector<vector<int> > &patterns_to_merge_all,vector<int>& discovered_patterns1,vector<int>& discovered_patterns2, vector<int>& fine_grain_seq1,vector<int>& fine_grain_seq2,vector<int>& patter_day_parts1,vector<int>& pattern_day_parts2);
    vector<vector<int> > withinPatternsMerge(vector<vector<int> > & patterns_to_merge_all,vector<int>& fine_grain_seq,vector<int>& discovered_patterns,vector<int> & pattern_day_parts);
    FeatureContrainer* updateFeatures(vector<int>& activity_labels1,vector<int>& activity_labels2,vector<vector<int> >& new_discovered_features,vector<int>& discovered_patterns1,vector<int>& discovered_patterns2, vector<int>& fine_grain_seq1,vector<int>& fine_grain_seq2,vector<vector<int> >& features1,vector<vector<int> >& features2,vector<int>& pattern_day_parts1,vector<int>& pattern_day_parts2);
    FeatureContrainer* betweenPatternSimilarityTest(int multi_ind,FeatureContrainer* fc1, FeatureContrainer* fc2);
    vector<int> findPatternDayParts(vector<int>& discovered_patterns, vector<int>& sequence_patterns);
    void writeFeatureContainer(FeatureContrainer* fc);
    vector<vector<int> > computeGoodnessFit(FeatureContrainer* fc1, FeatureContrainer* fc2);
    vector<int> getNumberDiscoveredActivities(FeatureContrainer* fc);


private:
    string base_dir;
    int f_timeslot,f_time_interval,f_diameter;
    Mat home_layout;
    vector<string> room_sybmols;
    vector<string> days;
    vector<int> activity_indexes,sensor_to_rooms;
    vector<string>activity_names;
    int num_of_days;
    SensorReader* f_sr;
    int df=34;//number of sensors is 35-1
    float chi_square_critical_value=43.773;//40.256; // from chi_square table with 34 df and alpha=0.05
    int ind_uni_pattern;
    int f_day_parts;
    int day_len;
    bool verbose;

};

#endif // INFERENTIALDISCOVERY_H
