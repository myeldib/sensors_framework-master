#ifndef SENSORFEATURE_H
#define SENSORFEATURE_H

#endif // SENSORFEATURE_H
#include <iostream>
#include <map>
#include<vector>
#include <algorithm>
#include <utility>
#include <string>
#include "Sensor.h"
#include "Home.h"

using namespace std;
class SensorFeature
{

public:
    SensorFeature(int num_rooms,int m_num_sensors,int d_num_sensors,time_t lower_limit, time_t upper_limit);
    void computeReadingsLevel(Sensor& s);
    void computeEllapsedTimeLevel(vector<Sensor>& curr_window_s,int& prev_room,int& sensor_id, int& segment_cnt,bool& prev_wind_leave );
    int getWindowNumMSensorsLevel(){ return w_numMSensors;}
    int getWindowNumDSensorsLevel(){ return w_numDSensors;}
    int getWindowNumTotalSensorsLevel(){ return w_numTotalSensors;}
    int getRoomMSensorsLevel(int i){return r_numMSensors[i];}
    int getRoomDSensorsLevel(int i){return r_numDSensors[i];}
    int getRoomTotalSensorsLevel(int i){return r_numTotalSensors[i];}
    int getMSensorsLevel(int i){return s_numMSensors[i];}
    int getDSensorsLevel(int i){return s_numDSensors[i];}
    int getRoomEllapsedTimeLevel(int i){ return r_ellapsed_time[i];}
    float getSensorEllapsedTimeLevel(int i){ return s_ellapsedSensors[i];}
    int getMostCommonRegion(){return mostCommonRegion;}

private:
    void computeWindowReadingsLevel(Sensor& s);
    void computeRoomReadingsLevel(Sensor& s);
    void computeSensorReadingsLevel(Sensor& s);
    void computeRoomEllapsedTimeLevel(vector<Sensor>& s,int& prev_room,int& curr_sensor, int& segment_cnt,bool& prev_wind_leave );
    void computeSensorEllapsedTimeLevel(vector<Sensor>& s,int& prev_room, int& segment_cnt,bool& prev_wind_leave );


private:
    int w_numMSensors,w_numDSensors,w_numTotalSensors; // window readings level
    vector<int> r_numMSensors,r_numDSensors,r_numTotalSensors; // room readings level
    vector<int>s_numMSensors, s_numDSensors; // sensor readings level per room
    vector<float> s_ellapsedSensors;  // compute time spent in each sensor;
    vector<float> r_ellapsed_time; // compute time spent in each room;
    int mostCommonRegion; // the most fequently occupied region
    vector<string> bagOfRooms; // the most common region sorted
    vector<string> bagOfSensors; // the most common sensor sequence (?)
    time_t lower_limit,upper_limit;
    bool is_first_room;
    Sensor null_sensor;
    float outside_time;
    int m_num_sensors,d_num_sensors;
    time_t prev_time;



};
