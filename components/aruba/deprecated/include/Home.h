#ifndef HOME_H
#define HOME_H
#include "Sensor.h"
#include<vector>
#include <fstream>
#include <iostream>
#include "Common.h"

using namespace deprecated;

class Home
{
public:
    Home();
    ~Home()
    {

    }

    void run();
    const  vector<Sensor>& getHomeSensors() const{ return home_sensors; }
    const  Mat getHomeLayout() { loadHomeLayout(home_layout_file); return home_layout; }
    const  Mat getHomeGroundMap() {loadHomeGroundMap(home_groundmap_file); return home_groundmap; }
    const  Mat getNoActivityMap() { createNoActivityMap();return no_activity_map; }
    const  int& getNumberOfRooms() const{ return num_of_rooms; }
    const  int& getNumberOfMSensors() const{ return m_num_sensors; }
    const  int& getNumberOfDSensors() const{ return d_num_sensors; }
    const  vector<string>& getRoomSymbols() const{ return room_symbols;}
    Sensor getSensorInfo(string sensor_id);
    const vector<int>& getSensorsToRooms(){ return sensor_to_room;}
    void printHomeSensorsInfo();

private:
    void readHomeSensorsFile(string filename);
    void readSensorRelationFile(string filename);
    void loadHomeLayout(string filename);
    void loadHomeGroundMap(string filename);
    void createNoActivityMap();

private:
    int num_of_rooms,m_num_sensors,d_num_sensors;
    vector<Sensor> home_sensors;
    vector<int> sensor_to_room;
    vector<string>local_to_area_sensors,room_symbols;
    string home_sensor_file;
    string sensor_relations_file;
    string home_layout_file;
    string home_groundmap_file;
    Mat home_layout;
    Mat home_groundmap;
    Mat no_activity_map;

};

#endif // HOME_H
