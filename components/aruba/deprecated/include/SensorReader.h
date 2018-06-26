#ifndef SENSORREADER_H
#define SENSORREADER_H
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <sstream>
#include <time.h>
#include <dirent.h>
#include "Sensor.h"
#include "Home.h"
#include <vector>
#include "Common.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>
using namespace boost;
using namespace std;
using namespace deprecated;

class SensorReader
{
public:
    SensorReader();
    ~SensorReader()
    {
    }

public:
    void run();
    Home* getHome() { return home; }
    const  vector<string>& getDays() const{ return s_days_folder_names; }
    const  vector<string>& getSensorPaths() const{ return s_days_folder_file_names; }
    const  int& getTimeSlot() const{ return s_timeslot; }
    const  int& getTimeInterval() const{ return s_time_interval; }
    const  int& getNumOfDays() const{ return s_days_folder_names.size(); }
    const  int& getNumOfRooms() const{ return s_num_rooms; }
    const  int& getNumOfMSensors() const{ return m_num_sensors; }
    const  int& getNumOfDSensors() const{ return d_num_sensors; }
    const  vector<string>& getActivityNames(){return activity_names;}
    const vector<int>& getActivityIndexes(){ return activity_indexes;}
    const  string& getSensorDirectory() const{ return s_directory_name; }
    const vector<vector <Sensor> >& getAllSensors() const {return sensor_data;}
    const vector<vector <string> >& getAllActivities() const {return activity_data;}
    const vector<string> getRoomSymbols() const {return room_sybmols;}
    const int & getNumberOfDayParts() const {return day_parts;}
    const vector<int> getSensorToRooms(){ return sensors_to_rooms;}
private:
    void getSensorInfoFromFile(string filename);
    void getAllSensorInfo();
    void readConfig();
private:
    vector<string> s_days_folder_file_names,s_days_folder_names;
    vector<vector <Sensor> > sensor_data;
    vector< vector<string> > activity_data;
    Home* home;
    int s_num_rooms,d_num_sensors,m_num_sensors;
    vector<string> room_sybmols;
    vector<int> sensors_to_rooms;
    int s_timeslot;
    int s_time_interval;
    int day_parts;
    string s_directory_name;
    vector<string> activity_names;
    vector<int> activity_indexes;

};

#endif // SENSORREADER_H
