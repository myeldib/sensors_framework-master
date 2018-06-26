#ifndef RECOGNITION_H
#define RECOGNITION_H
#include<Common.h>
#include <map>
#include<vector>
#include<numeric>
using namespace deprecated;
class Recognition{
public:
    Recognition();
    void run();
private:
    vector<vector<string> > readDiscoveredFile();
    vector<vector<string> > readFeatureFile();
    void applyOneLeaveOut(vector<vector<string> > features, vector<vector<string> > clusters);
    void recongize(vector<vector<string> > train_days, vector<vector<string> > test_day, vector<vector<string> > clusters);
    int getStartTime(int index);
    vector<vector<float> >  computeSimilarity(vector<vector<string> > test_day, vector<vector<int> >& clusters_sensor_duration,vector<vector<int> >& clusters_sensor_structure,vector<int>& clusters_start_time,vector<float>& clusters_weight);
    void computeClusterCentroid(vector<vector<string> > cluster_feature, int& start_time, vector<int>& duration, vector<int>& sensor_structure);
    void readConfigFile();

    float computeTimeSimilarity(int& start_time1,int& start_time2);
    float computeStructureSimilarity(vector<int>& xvec, vector<int>& yvec);
    float computeDurationSimilarity(vector<int>& dur1,vector<int>& dur2);
private:
    vector<int> activity_indexes;
    vector<string>activity_names;

};

#endif // RECOGNITION_H
