#ifndef SENSOR_H
#define SENSOR_H
#include<string>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;
class Sensor
{
public:
    Sensor(){}
    Sensor(string sensor_id){this->sensor_id=sensor_id;}
    const  string & getSensorId() const{ return sensor_id; }
    const  string & getSensorType() const{ return sensor_type; }
    const  string & getSensorRoomId() const{ return room_id; }
    const  string & getSensorDateTime() const{ return date_time; }
    const  string & getRelationToAreaSensor() const{ return relation_to_area_sensor; }
    const  string & getSensorState() const { return sensor_state;}
    const  time_t & getSensorTimestamp() const { return timestamp;}
    const  int & getoffset() const{ return offset; }
    const  int & getSensorMId() const{ return sensor_M_id; }
    const  int & getSensorDId() const{ return sensor_D_id; }
    const  Point & getSensorPosition() const{ return sensor_position; }
    const double & getSensorTimeSpent() const{return time_spent;}

    void  setSensorId(string sensor_id) {  this->sensor_id=sensor_id; }
    void  setSensorType(string sensor_type) {  this->sensor_type=sensor_type; }
    void  setSensorMId(int sensor_M_id){ this->sensor_M_id=sensor_M_id;}
    void  setSensorDId(int sensor_D_id){ this->sensor_D_id=sensor_D_id;}
    void  setRoomId(string room_id) {  this->room_id=room_id; }
    void  setOffset(int offset) {  this->offset=offset; }
    void  setSensorState(string sensor_state) { this->sensor_state=sensor_state;}
    void  setSensorPosition(Point sensor_position) {  this->sensor_position=sensor_position; }
    void  setRelationToAreaSensor(string sensor_id) { this->relation_to_area_sensor=sensor_id; }
    void  setSensorDateTime(string date_time) { this->date_time=date_time; }
    void setSensorTimestamp(time_t timestamp){ this->timestamp=timestamp;}
    void setSensorTimeSpent(double time_spent){ this->time_spent=time_spent;}
    void printSensorInfo(){cout<<getSensorId()<<" "<<getSensorState()<<" "<<getSensorPosition()<<" "<<getSensorType()<<" "<<getSensorRoomId()<<" "<<getRelationToAreaSensor()<<" "<<getSensorTimestamp()<<" "<<getSensorDateTime()<<endl;}

private:
    string sensor_id;
    int sensor_M_id;
    int sensor_D_id;
    Point sensor_position;
    string sensor_type;
    string room_id;
    string sensor_state;
    string relation_to_area_sensor;
    string date_time;
    time_t timestamp;
    double time_spent;
    int offset;

};

#endif // SENSOR_H
