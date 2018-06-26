#ifndef ACTIVITYDISCOVERY_H
#define ACTIVITYDISCOVERY_H
#include <iostream>
#include<DayContainer.h>
#include "SensorReader.h"
#include<vector>
#include <stdlib.h>
#include <iostream>
#include"EMD.h"
#include <vector>
#include <Python.h>
#include <algorithm>
#include <utility>
#include <string>
#include "Sensor.h"
#include "Home.h"

class ActivityDiscovery{

public:
    ActivityDiscovery(SensorReader& sr,DayContainer& coarse_grain_day, DayContainer& fine_grain_day,int& coarse_grain_win,int& fine_grain_win,vector<string> room_sybmols);
    void run();
    vector<int> getDiscoveredPatterns(){ return patterns;}
    vector<int> getSequencePatterns(){return fine_grain_seq;}
private:
    void printRoomEllapsedTimeLevel(vector<int>& room_ellapsed_time);
    void printWindowSensorEvents(vector<Sensor>& sensors);
    void findFrequentPatterns(vector<int> &cnt_frequent_pattern, vector<int> &frequent_pattern, vector<int> &subfrequent_pattern,vector<int>& cnt_subfrequent_pattern,vector<int>& seq,int& fine_grain_win, vector<int> &discovered_patterns);
    bool computeTimeDepenency(int X1,vector<int>& t1, vector<int>& t2);
    bool computeMinimumDiscriptionLength(int pattern_length, int frequency, int dataset_size);
    void discoverDistinctPatterns(vector<int>& seq,int& fine_grain_win, vector<int> &discovered_patterns,vector<vector<int > >& f_day_window_level);
    void findFineGrainPatterns(vector<int>& discovered_patterns, vector<int>& uni_pattern_index);
    void findCoarseGrainPatterns();
    bool findPattern(vector<int> & room_ellapsed_time1 , vector<int>& room_ellapsed_time2,int& coarse_grain_seq,int& pattern_gen);
    bool computeEMDSimilarity(vector<int>& e1,vector<int>& e2);

private:
    SensorReader sr;
    DayContainer coarse_grain_day,fine_grain_day;
    int coarse_grain_win,fine_grain_win;
    vector<int> coarse_grain_seq,fine_grain_seq;
    vector<string> room_sybmols;
    vector<int> patterns;
    float dl_threshold,emd_threshold,time_dependency_threshold;
    int X1,X2;
    vector<int> patterns_size;
    std::ofstream room_output_file,sensor_output_file;




};

#endif // ACTIVITYDISCOVERY_H
