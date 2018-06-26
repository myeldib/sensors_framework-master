#ifndef ACTIVENESS_H
#define ACTIVENESS_H
#include <iostream>
#include <map>
#include<vector>
#include <algorithm>
#include <utility>
#include <string>
#include "Sensor.h"
#include "Home.h"
typedef map<string, int> TStrIntMap;
typedef pair<string, int> TStrIntPair;
typedef map<string, time_t> TStrTimeMap;
typedef pair<string, time_t> TStrTimePair;
using namespace std;
class Activeness
{

public:
    Activeness(time_t lower_timeslot,time_t upper_timeslot,time_t time_interval,int timeslot,int timeslot_segment);
    void computeLevelOfActiveness(Sensor local_sensor,Sensor area_sensor,int diameter,bool& valid_loa);
    void computeTimeSpentInRooms(Sensor s,time_t lower_timeslot,bool& valid_lom);
    int getLevelOfActiveness();
    bool doorWasOpened(Sensor s);
    string getMaxSpentRoomId();
    void setIsFirstEntry(bool entry){this->is_first_entry=entry;}
    TStrTimeMap getTimeSpentInRooms();
     vector<Sensor> getPositionsPerTimeSlot();
private:
    void calculateTimeIntervalSegment();
    void initializeContainer();

private:
    bool is_first_entry,door_status_segment;
    time_t a_lower_timeslot,a_upper_timeslot,a_sensor_time;
    time_t a_time_interval,a_time_interval_segment,time_spent,track_time;
    vector< TStrIntMap > activeness_container;
    int a_timeslot_segment;
    int a_timeslot;
    vector<Sensor> positions_per_timeslot,positions_per_timeslot2;
    TStrTimeMap level_of_movement;
};

#endif // ACTIVENESS_H
