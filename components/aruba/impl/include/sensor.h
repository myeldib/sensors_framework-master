/*
 * sensor.h
 *
 *  Created on: 2017-03-14
 *      Author: Mohamed Eldib
 */

#ifndef SENSOR_H
#define SENSOR_H

#include<string>
using namespace std;

#include<opencv2/opencv.hpp>
using namespace cv;


class Sensor{
public:
    Sensor(){}


    void setLocation(string location){this->location=location;}
    void setId(string id)
    {
        this->id=id;

        //give an id to door sensor after last id of motion sensor
        if(id.find("D") != std::string::npos)
        {
            this->id2=num_motion_sensors+atoi(id.substr(1,id.size()).c_str())-1;
        }
        else
        {
            id2=atoi(id.substr(1,id.size()).c_str())-1;
        }
    }
    void setPosition(Point position){this->position=position;}
    void setState(string state){this->state=state;}
    void setReadableTime(string time_readable)
    {
        this->time_readable=time_readable;

    }
    void setTimeMilli(time_t time_milli){this->time_milli=time_milli;}
    void setDuration(double duration){this->duration=duration;}
    void setDate(string date){this->date=date;}
    void setType(string type){this->type=type;}
    void setActivity(string activity){this->activity=activity;}
    void setActivityState(string activity_state){this->activity_state=activity_state;}
    void setId2(int id2){this->id2=id2;}

    const string  & getReadableTime(){return this->time_readable;}
    const string & getLocation(){return this->location;}
    const  string & getId(){return this->id;}
    const  int & getId2(){return this->id2;}
    const  Point & getPosition(){return this->position;}
    const string & getstate(){return this->state;}
    const time_t & getTimeMilli(){return this->time_milli;}
    const double & getDuration(){return this->duration;}
    const  string & getDate(){return this->date;}
    const  string & getType(){return this->type;}
    const  string & getActivity(){return this->activity;}
    const string & getActivityState(){return this->activity_state;}
    friend ostream& operator<<(ostream& out,  Sensor& s)
    {
        out<<s.getReadableTime()<<","<<s.getDuration()<<","<<
             s.getId()<<","<<s.getId2()<<","<<s.getActivity()<<","<<s.getLocation();
        return out;
    }

private:
    // in which room the sensor belongs to
    string location="";
    string  id="";
    int id2;
    //sensor position in home
    Point  position;
    string state="";
    //two types: local or area sensor
    string type="";

    string time_readable="";
    time_t time_milli;

    //time spent around sensor in miliseconds
    double duration;
    string date="";

    //activity performed around sensor
    string activity="";
    string activity_state="";

    const int num_motion_sensors=31;

}; // SENSOR_H
#endif
