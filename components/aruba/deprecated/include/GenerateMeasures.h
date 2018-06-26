#ifndef GENERATEMEASURES_H
#define GENERATEMEASURES_H
#include "SensorReader.h"
#include "Home.h"
#include "SensorFeature.h"
#include "HeatMap.h"
#include<DayContainer.h>
#include<ActivityDiscovery.h>
using namespace std;
class GenerateMeasures{
public:
    GenerateMeasures();
    void run();
    void writeFeatures(DayContainer& fine_grain_day,vector<int>& discovered_patterns,vector<int>& sequence_patterns,string& day);
private:
    vector<vector<int> >  computeStatisticalFeatures(vector<int> &fine_grain_seq, vector<int> &discovered_patterns, vector<vector<int> >& f_day_sensor_ellapsed_time);
    void computeMeasures();
    void preprocessData(vector<Sensor>& sensors_per_day,string& day);
    void extractLastEvent(vector<string>& activity_per_day,
                          vector<Sensor>& day_sensors,
                          string& day,
                          bool& activity_begin,
                          bool& activity_end,
                          string& activity,
                          vector<string>& annotated_activities
                          );
    void readLastEvent(string day,
                       bool& activit_begin,
                       bool& activity_end,
                       string& activity,
                       int& room_id,
                       int& sensor_id,
                       bool is_first_day);
    int calculateTimeslotSegment(int win_size_minute);
        time_t initializeClock(string& day);
    DayContainer computeDayMeasures(vector<Sensor>& day_sensors,
                                    int win_size_sec,
                                    string& curr_day,
                                    int prev_room,
                                    int sensor_id
                                    );


    string timeStampToHReadble(const time_t rawtime);

private:
    int f_timeslot,f_time_interval,f_diameter,f_day_parts;
    Mat home_layout;
    vector<string> room_sybmols;
    SensorReader f_sr;
};

#endif // GENERATEMEASURES_H
