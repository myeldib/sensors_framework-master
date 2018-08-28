/*
 * time-window-approach.h
 *
 *  Created on: 2017-03-31
 *      Author: Mohamed Eldib
 */
#ifndef TIMEWINDOWAPPROACH_H
#define TIMEWINDOWAPPROACH_H
#include<string>
#include <fstream>
#include <sstream>
#include <map>
#include<vector>
#include<numeric>
#include<utilities.h>
#include<constants.h>
#include<home.h>
#include<sensor.h>
#include<sensor-event-processor.h>
#include<logger.h>
#include<boost/thread.hpp>

using namespace std;
class TimeWindow{
public:
    TimeWindow(Home* home,string folder_path);
    ~TimeWindow();
    void run();
private:
    void createDurationInfoWindow(Sensor *s,SensorEventProcessor* sensorEventProcessor,vector<string>& sensor_events,time_t& lower_limit,vector<int>& duration_info,int time_interval,fstream& ds_outfile);
    void createDurationWindow_(vector<string> sensor_data_files);
    void processDateFile(string file_name);
    void readConfig();
    void divideElements_(int from,
                         int to,
                         vector<string> sensor_data_files,
                         vector<vector<string> >& sensor_data_files_per_thread);
    void divideElementsBetweenThreads_(vector<string> sensor_data_files,
                                       vector<vector<string> >& sensor_data_files_per_thread);
    int computeDaySegment(int window_id, int time_constraint, int time_interval);
    void writeDurationInfo(fstream& ds_outfile,SensorEventProcessor* sensorEventProcessor,vector<string>& sensor_events,vector<int>& duration_info,int time_interval);
    void writeActivityInfo(SensorEventProcessor* sensorEventProcesso,fstream& ds_outfile,vector<int>& activity_info,bool& is_label_synched );
    time_t initializeClock(string file_name);
    void checkWindowDuration(vector<int>& duration_info,int time_interval);
    void selectMostDominantActivityInWindow(Sensor *s,SensorEventProcessor* sensorEventProcessor,time_t& lower_limit,vector<int>& duration_info,int time_interval,fstream& ds_outfile,bool& is_label_synched );

private:
    vector<string> sensor_data_files;
    string folder_path;
    Home* home;
    int num_threads;
    boost::thread_group g;
};

#endif // TIMEWINDOWAPPROACH_H
