#ifndef CONFUSIONMATRIX_H
#define CONFUSIONMATRIX_H
#include<Common.h>
#include<SensorReader.h>
#include <map>
#include<vector>
#include<numeric>
class ConfusionMatrix
{
public:
    ConfusionMatrix();
    void run();
private:
    vector<vector<string> > readDiscoveredFile();
    vector<vector<int> > readFeatureFile();
    vector<vector<int> > readSensorEllapsedTimeFile();
    void readConfigFile();
    void generateModifiedLabels(vector<vector<string> >& discovered_clusters, vector<vector<int> >& features);
    int computeDifference(vector<vector<int> >& source_features,vector<vector<int> >& processed_features);
    float computeClusterPurity();


private:
    SensorReader* f_sr;
    vector<string> room_sybmols;
    vector<string> days;
    vector<int> activity_indexes,sensor_to_rooms;
    vector<string>activity_names;
    int num_of_days;
    string base_dir;
    int f_timeslot,f_time_interval,f_diameter;
    int f_day_parts;
    int day_len;

};

#endif // CONFUSIONMATRIX_H
