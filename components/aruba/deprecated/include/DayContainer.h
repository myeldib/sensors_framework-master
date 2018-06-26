#ifndef DAYCONTAINER_H
#define DAYCONTAINER_H
#include<vector>
#include<Sensor.h>
#include <fstream>

using namespace std;

class DayContainer{
public:
    DayContainer(){}
    DayContainer(vector<vector< int> > day_room_m_sensor_level,
                 vector<vector<int > > day_room_d_sensor_level,
                 vector<vector<int > >day_room_total_sensor_level,
                 vector<vector<int > > day_window_level,
                 vector<vector<int > >day_sensor_m_level,
                 vector<vector<int> >day_sensor_d_level,
                 vector<vector<int > >day_room_ellapsed_time,
                 vector<vector<int> > day_sensor_ellapsed_time,
                 vector<vector<Sensor> > day_sensor_per_window,
                 vector<string> time_slots,
                 vector<int> day_parts)
    {
        this->day_room_m_sensor_level=day_room_m_sensor_level;
        this->day_room_d_sensor_level=day_room_d_sensor_level;
        this->day_room_total_sensor_level=day_room_total_sensor_level;
        this->day_window_level=day_window_level;
        this->day_sensor_m_level=day_sensor_m_level;
        this->day_sensor_d_level=day_sensor_d_level;
        this->day_room_ellapsed_time=day_room_ellapsed_time;
        this->day_sensor_ellapsed_time=day_sensor_ellapsed_time;
        this->day_sensor_per_window=day_sensor_per_window;
        this->time_slots=time_slots;
        this->day_parts=day_parts;
    }
    vector<vector<int > > get_day_room_ellapsed_time(){ return day_room_ellapsed_time;}
    vector<vector<int > > get_day_room_m_sensor_level(){ return day_room_m_sensor_level;}
    vector<vector<int > > get_day_room_d_sensor_level(){ return day_room_d_sensor_level;}
    vector<vector<int > > get_day_room_total_sensor_level(){ return day_room_total_sensor_level;}
    vector<vector<int > > get_day_window_level(){ return day_window_level;}
    vector<vector<int > > get_day_sensor_m_level(){ return day_sensor_m_level;}
    vector<vector<int > > get_day_sensor_d_level(){ return day_sensor_d_level;}
    vector<vector<Sensor> > get_day_sensor_per_window(){return day_sensor_per_window;}
    vector<string> get_time_slots(){return time_slots;}
    vector<int> getDayParts(){return this->day_parts;}
    vector<string> get_annotated_activities(){return annotated_activities;}
    vector<vector<int> > get_day_sensor_ellapsed_time(){return  day_sensor_ellapsed_time;}
    void set_annotated_activities(vector<string> annotated_activities){this->annotated_activities=annotated_activities;}
    void setDay(string day){this->day=day;}
    string getDay(){return this->day;}


private:
    vector<vector<int > >day_room_ellapsed_time;
    vector<vector<int > > day_room_m_sensor_level;
    vector<vector<int > >day_room_d_sensor_level;
    vector<vector<int > > day_room_total_sensor_level;
    vector<vector<int > > day_window_level;
    vector<vector<int > >day_sensor_m_level;
    vector<vector<int> >day_sensor_d_level;
    vector<vector<Sensor> > day_sensor_per_window;
    vector<vector<int> > day_sensor_ellapsed_time;
    vector<string> annotated_activities;
    vector<string> time_slots;
    vector<int> day_parts;
    string day;
};

#endif // DAYCONTAINER_H
