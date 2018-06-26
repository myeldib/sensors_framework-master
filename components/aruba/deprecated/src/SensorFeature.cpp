#include<SensorFeature.h>

SensorFeature::SensorFeature(int num_rooms, int m_num_sensors, int d_num_sensors, time_t lower_limit, time_t upper_limit)
{

    this->lower_limit=lower_limit;
    this->upper_limit=upper_limit;

    null_sensor.setSensorId("Dummy");
    outside_time=0;
    w_numMSensors=0;
    w_numDSensors=0;
    w_numTotalSensors=0;
    this->m_num_sensors=m_num_sensors;
    this->d_num_sensors=d_num_sensors;
    is_first_room=true;


    for(int i=0;i<num_rooms; i++)
    {
        r_numDSensors.push_back(0);
        r_numMSensors.push_back(0);
        r_numTotalSensors.push_back(0);
        r_ellapsed_time.push_back(0.0);

    }

    for(int j=0;j<m_num_sensors;j++)
    {
        s_numMSensors.push_back(0);
        s_ellapsedSensors.push_back(0.0);
    }


    for(int c=0;c<d_num_sensors;c++)
    {
        s_numDSensors.push_back(0);
    }

}

void SensorFeature::computeEllapsedTimeLevel(vector<Sensor> &s,int& prev_room,int& sensor_id,int& segment_cnt,bool& prev_wind_leave)
{


    computeRoomEllapsedTimeLevel(s,prev_room,sensor_id,segment_cnt,prev_wind_leave);

}


void SensorFeature::computeRoomEllapsedTimeLevel(vector<Sensor> & cur_win_sensors,int& curr_room,int& curr_sensor, int& segment_cnt,bool& prev_wind_leave)
{


    bool is_leave=false;
    Sensor open_door=null_sensor,close_door=null_sensor;


    if(cur_win_sensors.size()==0)
    {

        r_ellapsed_time[curr_room]=upper_limit-lower_limit;
        s_ellapsedSensors[curr_sensor]=upper_limit-lower_limit;

    }
    if(cur_win_sensors.size()==1)
    {
        //  if(cur_win_sensors[0].getSensorType()=="local_sensor" || cur_win_sensors[0].getSensorType()=="door_sensor")
        {
            r_ellapsed_time[atoi(cur_win_sensors[0].getSensorRoomId().c_str())]=upper_limit-lower_limit;
            s_ellapsedSensors[cur_win_sensors[0].getSensorMId()]=upper_limit-lower_limit;

            curr_room=atoi(cur_win_sensors[0].getSensorRoomId().c_str());
            curr_sensor=cur_win_sensors[0].getSensorMId();
        }

    }


    for(int i=0; i<cur_win_sensors.size() && cur_win_sensors.size()>1; i++)
    {


        Sensor s=cur_win_sensors[i];
        //   if(s.getSensorType()=="local_sensor" || s.getSensorType()=="door_sensor")
        {


            if(is_first_room)
            {
                //check if a door event is open
                if(s.getSensorType()=="door_sensor" && s.getSensorState()=="OPEN")
                {
                    curr_sensor=s.getSensorMId();
                    open_door=s;
                    is_leave=false;
                }

                //check if a door event is close
                if(s.getSensorType()=="door_sensor" && s.getSensorState()=="CLOSE")
                {
                    curr_sensor=s.getSensorMId();
                    close_door=s;
                    is_leave=false;
                }


                //comute the time spent based on previous room id
                is_first_room=false;
                r_ellapsed_time[curr_room]+= (s.getSensorTimestamp()-lower_limit);
                s_ellapsedSensors[curr_sensor]+= (s.getSensorTimestamp()-lower_limit);

            }

            else
            {
                //check if a door event is open
                if(s.getSensorType()=="door_sensor" && s.getSensorState()=="OPEN")
                {
                    curr_sensor=s.getSensorMId();
                    open_door=s;
                    is_leave=false;
                }

                //check if a door event is close
                if(s.getSensorType()=="door_sensor" && s.getSensorState()=="CLOSE")
                {
                    curr_sensor=s.getSensorMId();
                    close_door=s;
                    is_leave=false;
                }


                //if a matching open/close door event is found, comute not_here time
                if(open_door.getSensorState()=="OPEN" && close_door.getSensorState()=="CLOSE")
                {
                    outside_time+=(close_door.getSensorTimestamp()-open_door.getSensorTimestamp());

                    close_door=null_sensor;
                    open_door=null_sensor;
                    is_leave=true;
                }

                //compute time spent based on the room id of the previous event
                r_ellapsed_time[atoi(cur_win_sensors[i-1].getSensorRoomId().c_str())]+= (s.getSensorTimestamp()-prev_time);
                s_ellapsedSensors[cur_win_sensors[i-1].getSensorMId()]+= (s.getSensorTimestamp()-prev_time);

            }

            //if no more sensor event in the current window, then compute the time spent for the last sensor event, and update room_id
            if(i==cur_win_sensors.size()-1)
            {
                r_ellapsed_time[atoi(s.getSensorRoomId().c_str())]+= upper_limit-s.getSensorTimestamp();
                s_ellapsedSensors[s.getSensorMId()]+= upper_limit-s.getSensorTimestamp();

                curr_room=atoi(s.getSensorRoomId().c_str());
                curr_sensor=s.getSensorMId();
            }

            //record the state if a person is isnide or not
            prev_wind_leave=is_leave;
            prev_time=s.getSensorTimestamp();
        }

    }

    if(cur_win_sensors.size()>1)
    {

        // if a door is open in the current window without a close pair, then check the time spent in the hallway
        //if it is more than 15% of the time spent in the current window, then this time spent belongs to not_here instead of hallway
        if(open_door.getSensorState()=="OPEN" && r_ellapsed_time[7]<r_ellapsed_time[6] && r_ellapsed_time[6]/(upper_limit-lower_limit) >0.15 )
        {
            r_ellapsed_time[7]+=r_ellapsed_time[6];
            r_ellapsed_time[6]=0;
            prev_wind_leave=true;

            for(int j=0;j<m_num_sensors;j++)
                s_ellapsedSensors[j]=0;

            s_ellapsedSensors[m_num_sensors-1]=r_ellapsed_time[7];

        }

        //if the computed pair open and close door time is more greater than current not_here time, then update
        if(r_ellapsed_time[7]<outside_time)
        {
            r_ellapsed_time[7]=outside_time;
            s_ellapsedSensors[m_num_sensors-1]=outside_time;
        }

        int tmp_room;

        //check if the most frequent occupied room is hallway
        std::vector<float>::iterator result=max_element(r_ellapsed_time.begin(),r_ellapsed_time.end());
        tmp_room=std::distance(r_ellapsed_time.begin(), result);


        //if the most frequent occupied room is the hallway, and the person is not here,
        //and the hallway time spent in the current window is more than 51%, then update not_here time
        if( (tmp_room==6 ) && (r_ellapsed_time[6]>r_ellapsed_time[7] && is_leave) &&  r_ellapsed_time[6]/(upper_limit-lower_limit) >0.51)
        {
            r_ellapsed_time[7]+=r_ellapsed_time[6];
            r_ellapsed_time[6]=0;

            for(int j=0;j<m_num_sensors;j++)
                s_ellapsedSensors[j]=0;

            s_ellapsedSensors[m_num_sensors-1]=r_ellapsed_time[7];
        }

    }

    //select the highest occupied room
    std::vector<float>::iterator result=max_element(r_ellapsed_time.begin(),r_ellapsed_time.end());
    mostCommonRegion=std::distance(r_ellapsed_time.begin(), result);

    //dirty workaround to set room count
    Home h;
    h.run();
    for(int i=0;i<s_ellapsedSensors.size();i++)
    {
        if(s_ellapsedSensors[i]>0)
        {
            Sensor s=h.getSensorInfo(std::to_string(i));
            s_numMSensors[i]++;
            r_numMSensors[atoi(s.getSensorRoomId().c_str())]++;

        }
    }



}


void SensorFeature::computeReadingsLevel(Sensor &s)
{
    computeWindowReadingsLevel(s);

    computeRoomReadingsLevel(s);

    computeSensorReadingsLevel(s);

}


void SensorFeature::computeWindowReadingsLevel(Sensor &s)
{

    if(s.getSensorType()!="door_sensor")
    {
        if(s.getSensorState()=="ON")
        {

            w_numMSensors++;
        }
    }
    else
    {
        if(s.getSensorState()=="OPEN" || s.getSensorState()=="CLOSE")
        {

            w_numDSensors++;

        }
    }
    w_numTotalSensors=w_numDSensors+w_numMSensors;

}

void SensorFeature::computeRoomReadingsLevel(Sensor &s)
{
    if(s.getSensorType()!="door_sensor")
    {
        if(s.getSensorState()=="ON")
        {

            r_numMSensors[atoi(s.getSensorRoomId().c_str())]++;

        }
    }
    else
    {
        if(s.getSensorState()=="OPEN" || s.getSensorState()=="CLOSE")
        {
            r_numMSensors[atoi(s.getSensorRoomId().c_str())]++;
            r_numDSensors[atoi(s.getSensorRoomId().c_str())]++;

        }
    }
    r_numTotalSensors[atoi(s.getSensorRoomId().c_str())]= r_numDSensors[atoi(s.getSensorRoomId().c_str())]+ r_numMSensors[atoi(s.getSensorRoomId().c_str())];

}


void SensorFeature::computeSensorReadingsLevel(Sensor &s)
{

    if(s.getSensorState()=="ON" || s.getSensorType()=="door_sensor")
    {


        s_numMSensors[s.getSensorMId()]++;

        if(s.getSensorState()=="OPEN" || s.getSensorState()=="CLOSE")
        {
            s_numDSensors[s.getSensorDId()]++;

        }

    }

}
