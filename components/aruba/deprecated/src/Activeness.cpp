#include "Activeness.h"

Activeness::Activeness(time_t lower_timeslot, time_t upper_timeslot,time_t time_interval,int timeslot,int timeslot_segment)
{
    this->is_first_entry=true;
    this->a_timeslot=timeslot;
    this->a_lower_timeslot=lower_timeslot;
    this->a_upper_timeslot=upper_timeslot;
    this->a_time_interval=time_interval;
    this->a_timeslot_segment=timeslot_segment;
    calculateTimeIntervalSegment();
    initializeContainer();
}
bool Activeness::doorWasOpened(Sensor s)
{
    if(s.getSensorState()=="OPEN")
    {
        //cout<<"OPEN"<<endl;
        this->door_status_segment=true;
        return this->door_status_segment;
    }

}

void Activeness::initializeContainer()
{

    activeness_container.reserve(a_time_interval_segment);

    for(int i=0;i<a_time_interval_segment;i++)
    {
        TStrIntMap  triggers_per_sensor;

        activeness_container.push_back(triggers_per_sensor);


    }

}
void Activeness::computeTimeSpentInRooms(Sensor s,  time_t lower_timeslot, bool& valid_lom)
{

    if( s.getSensorRoomId()!="-1" && s.getSensorState()=="ON")
    {
        if(is_first_entry)
        {
            valid_lom=true;
            is_first_entry=false;

            time_spent=(s.getSensorTimestamp()-lower_timeslot)*1.0;
            s.setSensorTimeSpent(time_spent);
            positions_per_timeslot2.push_back(s);

            level_of_movement.insert(TStrTimePair(s.getSensorRoomId(),time_spent));
            track_time=s.getSensorTimestamp();
        }
        else
        {

            valid_lom=true;
            time_spent=(s.getSensorTimestamp()-track_time)*1.0;
            s.setSensorTimeSpent(time_spent);
            positions_per_timeslot2.push_back(s);

            if (level_of_movement.count(s.getSensorRoomId()) == 0)
                level_of_movement.insert(TStrTimePair(s.getSensorRoomId(),time_spent));
            else
                level_of_movement[s.getSensorRoomId()]+=time_spent;


            track_time=s.getSensorTimestamp();

        }

    }
    else
    {
        if(s.getSensorState()=="OPEN")
        {
            time_spent=0;
            track_time=s.getSensorTimestamp();
        }
    }

}

TStrTimeMap Activeness::getTimeSpentInRooms()
{

    int sum=0;
    string arg_max;
    time_t others_time_spent=0,currentMax=0;
    map<string,time_t>::iterator iter;
    if(level_of_movement.size()==1)
    {

        for (iter = level_of_movement.begin(); iter != level_of_movement.end(); ++iter)
        {
            if (iter ->second > currentMax)
            {
                arg_max = iter->first;
                currentMax = iter->second;
            }
            level_of_movement[iter->first]=a_timeslot;

        }
    }
    else
    {
        for (iter = level_of_movement.begin(); iter != level_of_movement.end(); ++iter)
        {

            if (iter ->second > currentMax) {
                arg_max = iter->first;
                currentMax = iter->second;
            }
            sum+=iter->second;

        }


    }

     cout << "Value " << arg_max << " occurs " << currentMax << " times " << endl;

    if(sum!=a_timeslot && level_of_movement.size()!=1)
    {
        others_time_spent=a_timeslot-sum;
        level_of_movement.insert(TStrTimePair("o",others_time_spent));
    }


    return level_of_movement;

}

int Activeness::getLevelOfActiveness()
{

    int level_of_activeness=0;

    for(int i=0; i<activeness_container.size();i++)
    {
        TStrIntMap  triggers_per_sensor=activeness_container[i];

        //        if(triggers_per_sensor.size()>0)
        //        {

        //            for (TStrIntMap::iterator it = triggers_per_sensor.begin(); it != triggers_per_sensor.end(); it++) {
        //                string thisKey = it->first;
        //                int thisValue = it->second;

        //            }
        //        }


        if(activeness_container[i].size()>0)
        {
            level_of_activeness+=max_element(triggers_per_sensor.begin(), triggers_per_sensor.end(),Common::compareMapValues)->second;
            // cout<<"#####"<<max_element(triggers_per_sensor.begin(), triggers_per_sensor.end(),Common::compareMapValues)->second<<"#####"<<endl;
        }

    }



    return level_of_activeness;
}
vector<Sensor> Activeness::getPositionsPerTimeSlot()
{


    vector<Sensor> s;

    if(positions_per_timeslot.size()>0)
    {

     s =Common::useKalmanFilter(positions_per_timeslot);
    }

    return s;
}

void Activeness::computeLevelOfActiveness(Sensor local_sensor,Sensor area_sensor,int diameter,bool& valid_loa)
{

    time_t lower_time_interval=a_lower_timeslot;
    time_t upper_time_interval=a_lower_timeslot+a_time_interval;

    bool within_area_sensor=Common::withinAreaSensorDiameter(local_sensor,area_sensor,diameter);

    for(int i=0; i<a_time_interval_segment; i++)
    {


        if((local_sensor.getSensorTimestamp()>=lower_time_interval && local_sensor.getSensorTimestamp()<upper_time_interval) && ( local_sensor.getSensorRoomId()=="7" && local_sensor.getSensorState()=="ON"))
        {

            if(local_sensor.getSensorType()=="local_sensor")
                positions_per_timeslot.push_back(local_sensor);

        }


        if((local_sensor.getSensorTimestamp()>=lower_time_interval && local_sensor.getSensorTimestamp()<upper_time_interval) && (within_area_sensor&& local_sensor.getSensorState()=="ON"))
        {

            valid_loa=true;

            if (activeness_container[i].count(local_sensor.getSensorId()) == 0)
                activeness_container[i].insert(TStrIntPair(local_sensor.getSensorId(),1));
            else
                activeness_container[i][local_sensor.getSensorId()]++;

            if(local_sensor.getSensorType()=="local_sensor")
                positions_per_timeslot.push_back(local_sensor);


            // break;

        }



        lower_time_interval=upper_time_interval;
        upper_time_interval+=a_time_interval;
    }
}
void Activeness::calculateTimeIntervalSegment()
{
    a_time_interval_segment=(a_upper_timeslot-a_lower_timeslot)/a_time_interval;
}
