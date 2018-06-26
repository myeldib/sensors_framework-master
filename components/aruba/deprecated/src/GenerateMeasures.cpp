#include "GenerateMeasures.h"

GenerateMeasures::GenerateMeasures()
{

    f_sr.run();
    f_timeslot=f_sr.getTimeSlot();
    f_time_interval=f_sr.getTimeInterval();
    f_day_parts=f_sr.getNumberOfDayParts();
    room_sybmols=f_sr.getRoomSymbols();
    cv::startWindowThread();
    Py_Initialize();

}
void GenerateMeasures::run()
{


    computeMeasures();
}

time_t GenerateMeasures::initializeClock(string& day)
{
    string clock=day;
    clock.append(" 00:00:00");

    time_t timestamp;
    struct tm tm;

    if (strptime(clock.c_str(), "%Y-%m-%d %H:%M:%S", &tm) != NULL )
        timestamp = mktime(&tm);

    string lower_time = asctime(localtime(&timestamp));

    if(lower_time.find("00:00:00")== std::string::npos)
        timestamp+=3600;

    return timestamp;
}



void GenerateMeasures::computeMeasures()
{

    // cv::namedWindow("layout",1);
    bool init_data=true;

    for(int i=0;i<f_sr.getNumOfDays(); i++)
    {

        vector<string> annotated_activities;
        vector<Sensor> sensors_per_day=f_sr.getAllSensors()[i];
        vector<string> activities_per_day=f_sr.getAllActivities()[i];
        string day=f_sr.getDays()[i];
        bool activity_begin;
        bool activity_end;
        string activity;
        int room_id;
        int sensor_id;


        cout<<"Day:"<<day<<endl;

        //preprocessData(sensors_per_day,day);

        if(i==0)
        {
            if(init_data)
                readLastEvent(day,activity_begin,activity_end,activity,room_id,sensor_id,true);
            else
                readLastEvent(day,activity_begin,activity_end,activity,room_id,sensor_id,false);

            extractLastEvent(activities_per_day,sensors_per_day,day,activity_begin,activity_end,activity,annotated_activities);
        }
        else
        {
            readLastEvent(f_sr.getDays()[i-1],activity_begin,activity_end,activity,room_id,sensor_id,false);
            extractLastEvent(activities_per_day,sensors_per_day,day,activity_begin,activity_end,activity,annotated_activities);
        }



        DayContainer coarse_grain_day=computeDayMeasures(sensors_per_day,f_timeslot,day,room_id,sensor_id);
        DayContainer fine_grain_day=computeDayMeasures(sensors_per_day,f_time_interval,day,room_id,sensor_id);


        fine_grain_day.set_annotated_activities(annotated_activities);
        fine_grain_day.setDay(day);

        ActivityDiscovery ad(f_sr,coarse_grain_day,fine_grain_day,f_timeslot,f_time_interval,room_sybmols);
        ad.run();

        vector<int> discovered_patterns=ad.getDiscoveredPatterns();
        vector<int> sequence_patterns=ad.getSequencePatterns();

        writeFeatures(fine_grain_day,discovered_patterns,sequence_patterns,day);

        cout<<"\n";

    }
    Py_Finalize();
}


void GenerateMeasures::writeFeatures(DayContainer& fine_grain_day,vector<int>& discovered_patterns,vector<int>& sequence_patterns,string& day )
{
    string features_directory="../data/days/"+day+"/features";
    Common::createFolder(features_directory);

    std::ofstream sensor_ellapsed_time_file(features_directory+"/sensor_ellapsed_time.txt");
    std::ofstream room_ellapsed_time_file(features_directory+"/room_ellapsed_time.txt");
    std::ofstream sensor_count_file(features_directory+"/sensor_count.txt");
    std::ofstream room_count_file(features_directory+"/room_count.txt");
    std::ofstream sequence_patterns_file(features_directory+"/sequence_patterns.txt");
    std::ofstream discovered_patterns_file(features_directory+"/discovered_patterns.txt");
    std::ofstream sum_patterns_file(features_directory+"/sum_patterns.txt");
    std::ofstream activity_labels_file(features_directory+"/activity_labels.txt");


    Common::createFile(discovered_patterns,discovered_patterns_file,1);
    Common::createFile(sequence_patterns,sequence_patterns_file,1);

    vector<vector<int > > f_day_sensor_ellapsed_time=fine_grain_day.get_day_sensor_ellapsed_time(); //ellapsed time per sensor in seconds in 1 minute window
    vector<vector<int > > f_day_room_m_sensor_level=fine_grain_day.get_day_room_m_sensor_level(); // number of events per room in 1 minute window
    vector<vector<int > > f_day_room_ellapsed_time=fine_grain_day.get_day_room_ellapsed_time(); // ellapsed time per room in seconds in 1 minute window
    vector<vector<int > >f_day_sensor_m_level=fine_grain_day.get_day_sensor_m_level(); // number of events per sensor in 1 minute window
    vector<string> annotated_activities=fine_grain_day.get_annotated_activities(); //activity labels per 1 minute window


    vector<vector<int> > sum_pattern_ellapsed_time=computeStatisticalFeatures(sequence_patterns,discovered_patterns,f_day_sensor_ellapsed_time);


    for(int i=0;i<f_day_sensor_ellapsed_time.size();i++)
    {
        Common::createFile(f_day_sensor_ellapsed_time[i],sensor_ellapsed_time_file,1);
        Common::createFile(f_day_room_ellapsed_time[i],room_ellapsed_time_file,1);
        Common::createFile(f_day_sensor_m_level[i],sensor_count_file,1);
        Common::createFile(f_day_room_m_sensor_level[i],room_count_file,1);


    }

    for(int i=0;i<sum_pattern_ellapsed_time.size();i++)
    {
        vector<int> sum_patterns=sum_pattern_ellapsed_time[i];
        // sum_patterns.push_back(discovered_patterns[i]);
        Common::createFile(sum_patterns,sum_patterns_file,1);
    }
    for(int i=0;i<annotated_activities.size();i++)
        activity_labels_file<<annotated_activities[i]<<endl;

}

void GenerateMeasures::readLastEvent(string day, bool& activit_begin, bool& activity_end, string& activity, int& room_id,int& sensor_id,bool is_first_day)
{

    if(!is_first_day)
    {
        std::ifstream infile("../data/days/"+day+"/day_config.txt");
        int line_index=0;
        while (infile)
        {
            string line;
            vector<string> elem;
            if (!getline( infile, line )) break;
            Common::split(line,'=',elem);
            if(line_index==0)
                activit_begin=atoi(elem[1].c_str());
            if(line_index==1)
                activity_end=atoi(elem[1].c_str());
            if(line_index==2)
                activity=elem[1];
            if(line_index==3)
                room_id=atoi(elem[1].c_str());
            if(line_index==4)
                sensor_id=atoi(elem[1].c_str());

            line_index++;

        }
    }
    else
    {
        activit_begin=false;
        activity_end=false;
        activity="-";
        room_id=0;
        sensor_id=2;
    }

}

void GenerateMeasures::extractLastEvent(vector<string> &activities_per_day, vector<Sensor> & sensors_per_day,string& day,bool& activity_begin, bool& activity_end, string& curr_activity,vector<string>& annotated_activities)
{
    int win_size_sec=60;// in seconds
    int f_timeslot_segment=calculateTimeslotSegment(win_size_sec);
    bool home_activity_beg=false;
    bool home_activity_end=false;
    string tmp_home_activity="-";

    int tmp_start_seg=0;
    int tmp_end_seg=0;
    //    vector<string> annotated_activities2;

    for(int i=0;i<f_timeslot_segment;i++)
        annotated_activities.push_back("-");

    //window definitions
    int segment_cnt=0;
    time_t lower_timeslot=initializeClock(day);
    time_t upper_timeslot=lower_timeslot+win_size_sec;
    bool is_prev=false;

    Sensor last_sensor_event;
    //    annotated_activities2=annotated_activities;


    for(int j=0; j<f_timeslot_segment;j++)
    {



        for(int c=0; c<sensors_per_day.size();c++)
        {

            Sensor s=sensors_per_day[c];
            string activity=activities_per_day[c];


            if(s.getSensorTimestamp()>=lower_timeslot && s.getSensorTimestamp()<upper_timeslot)
            {
                if (activity.find("begin")!=std::string::npos && activity.find("Home")!=std::string::npos)
                {
                    home_activity_beg=true;
                    is_prev=true;

                    vector<string> elem;
                    Common::split(activity,' ',elem);
                    tmp_home_activity=elem[0];

                }

                if (activity.find("end")!=std::string::npos && activity.find("Home")!=std::string::npos)
                    home_activity_end=true;

                if (activity.find("begin")!=std::string::npos && activity.find("Home")==std::string::npos)
                {
                    activity_begin=true;
                    is_prev=true;

                    vector<string> elem;
                    Common::split(activity,' ',elem);

                    curr_activity=elem[0];

                }

                if (activity.find("end")!=std::string::npos && activity.find("Home")==std::string::npos)
                    activity_end=true;


                if(!activity_begin && activity_end)
                {

                    activity_begin=false;
                    activity_end=false;
                    curr_activity="-";

                    vector<string> elem;
                    tmp_end_seg=segment_cnt;
                    Common::split(activity,' ',elem);

                    for(int k=tmp_start_seg;k<tmp_end_seg;k++)
                        annotated_activities[k]=elem[0];

                }
                if(activity_begin && activity_end)
                {
                    activity_begin=false;
                    activity_end=false;

                    curr_activity="-";
                    tmp_start_seg=segment_cnt;

                }


                if(!activity_begin && !activity_end && home_activity_beg && !home_activity_end)
                    curr_activity=tmp_home_activity;


                if(!activity_begin && !activity_end && home_activity_beg && home_activity_end)
                {
                    home_activity_beg=false;
                    home_activity_end=false;

                    if(curr_activity.find("Leave")!=std::string::npos)
                        curr_activity=tmp_home_activity;
                    else
                        curr_activity="-" ;

                }
                if((s.getSensorType()=="local_sensor" && s.getSensorState()=="ON") || s.getSensorType()=="door_sensor")
                    last_sensor_event=s;


            }


        }


        if(is_prev)
        {
            is_prev=false;
            annotated_activities[segment_cnt]="-" ;
        }
        else
            annotated_activities[segment_cnt]=curr_activity;

        //      annotated_activities2[segment_cnt]=curr_activity;
        //cout<<segment_cnt<<"\t"<<curr_activity<<"\t"<<last_sensor_event.getSensorMId()<<endl;

        lower_timeslot=upper_timeslot;
        upper_timeslot+=win_size_sec;
        segment_cnt++;

    }



    //    for(int i=0;i<annotated_activities.size();i++)
    //        cout<<annotated_activities[i]<<"\t"<<annotated_activities2[i]<<endl;

    std::ofstream out_file("../data/days/"+day+"/day_config.txt");
    out_file<<"activity_begin="<<activity_begin<<endl;
    out_file<<"activity_end="<<activity_end<<endl;
    out_file<<"curr_activity="<<curr_activity<<endl;
    out_file<<"room_id="<<last_sensor_event.getSensorRoomId()<<endl;
    out_file<<"sensor_id="<<(last_sensor_event.getSensorMId())<<endl;
    out_file.close();

    // cout<<"out_file\t"<<last_sensor_event.getSensorRoomId()<<"\t"<<activity_begin<<"\t"<<activity_end<<"\t"<<curr_activity<<endl;

}

string GenerateMeasures::timeStampToHReadble(const time_t rawtime)
{
    struct tm * dt;
    char buffer [30];
    dt = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", dt);
    return std::string(buffer);
}
void GenerateMeasures::preprocessData(vector<Sensor>& sensors_per_day,string& day)
{
    int f_timeslot_segment=calculateTimeslotSegment(60);

    //window definitions
    int segment_cnt=0;
    time_t lower_timeslot=initializeClock(day)+3600;
    time_t upper_timeslot=lower_timeslot+3660;
    int num_events=0;
    int after_midnight=60;
    int hours_to_process=5;

    for(int j=after_midnight; j<60*hours_to_process;j++)
    {

        for(int c=0; c<sensors_per_day.size();c++)
        {

            Sensor s=sensors_per_day[c];

            if(s.getSensorTimestamp()>=lower_timeslot && s.getSensorTimestamp()<upper_timeslot)
            {
                //&& s.getSensorId() !="M002"
                if(s.getSensorId() !="M001" && s.getSensorId() !="M002"&& s.getSensorId() !="M003" &&s.getSensorId() !="M004" &&s.getSensorId() !="M005" &&s.getSensorId() !="M006" &&s.getSensorId() !="M007" )
                {
                    s.printSensorInfo();
                    cout<<s.getSensorId()<<endl;
                    num_events++;
                }

            }
        }


        lower_timeslot=upper_timeslot;
        upper_timeslot+=60;
        segment_cnt++;
    }
}

DayContainer GenerateMeasures::computeDayMeasures(vector<Sensor>& sensors_per_day, int win_size_sec,string& day,int prev_room,int sensor_id)
{

    int f_timeslot_segment=calculateTimeslotSegment(win_size_sec);

    //window definitions
    int segment_cnt=0;
    time_t lower_timeslot=initializeClock(day);
    time_t upper_timeslot=lower_timeslot+win_size_sec;
    int sensor_time_index=0;
    vector<string> timeslots;

    //room ellapsed time level definitions
    bool prev_wind_leave=false;
    vector<vector<int > > day_room_ellapsed_time_level;
    vector<vector<int> > day_sensor_ellapsed_time_level;

    //sensor readings level definitions
    vector<vector<int > > day_room_m_sensor_level;
    vector<vector<int > >day_room_d_sensor_level;
    vector<vector<int > > day_room_total_sensor_level;
    vector<vector<int > > day_window_level;
    vector<vector<int > >day_sensor_m_level;
    vector<vector<int> >day_sensor_d_level;
    vector<vector<Sensor> > day_sensor_per_window;
    vector<int> day_parts;

    for(int j=0; j<f_timeslot_segment;j++)
    {

        vector<Sensor> cur_window_sensors;
        SensorFeature sf(f_sr.getNumOfRooms(),f_sr.getNumOfMSensors(),f_sr.getNumOfDSensors(),lower_timeslot,upper_timeslot);

        for(int c=sensor_time_index; c<sensors_per_day.size();c++)
        {

            Sensor s=sensors_per_day[c];

            if(s.getSensorTimestamp()>=lower_timeslot && s.getSensorTimestamp()<upper_timeslot)
            {

                if((s.getSensorState()=="ON" && s.getSensorType()=="local_sensor") || s.getSensorType()=="door_sensor")
                {
                    sf.computeReadingsLevel(s);
                    cur_window_sensors.push_back(s);
                }

            }

        }


        vector<int> room_ellapsed_time_level;
        vector<int>sensor_ellapsed_time_level;
        vector<int> room_m_sensor_level;
        vector<int> room_d_sensor_level;
        vector<int> room_total_sensor_level;
        vector<int> window_level;
        vector<int> sensor_m_level;
        vector<int> sensor_d_level;

        sf.computeEllapsedTimeLevel(cur_window_sensors,prev_room,sensor_id,segment_cnt,prev_wind_leave);

        timeslots.push_back(timeStampToHReadble(lower_timeslot));

        window_level.push_back(sf.getWindowNumMSensorsLevel());
        window_level.push_back(sf.getWindowNumDSensorsLevel());
        window_level.push_back(sf.getWindowNumTotalSensorsLevel());
        window_level.push_back(sf.getMostCommonRegion());


        for(int z=0;z<f_sr.getNumOfRooms();z++)
        {
            room_ellapsed_time_level.push_back(sf.getRoomEllapsedTimeLevel(z));
            room_m_sensor_level.push_back(sf.getRoomMSensorsLevel(z));
            room_d_sensor_level.push_back(sf.getRoomDSensorsLevel(z));
            room_total_sensor_level.push_back(sf.getRoomTotalSensorsLevel(z));

        }

        for(int z=0;z<f_sr.getNumOfMSensors();z++)
        {
            sensor_m_level.push_back(sf.getMSensorsLevel(z));
            sensor_ellapsed_time_level.push_back(sf.getSensorEllapsedTimeLevel(z));
        }

        for(int z=0;z<f_sr.getNumOfDSensors();z++)
        {
            sensor_d_level.push_back(sf.getDSensorsLevel(z));

        }

        day_sensor_m_level.push_back(sensor_m_level);
        day_sensor_d_level.push_back(sensor_d_level);
        day_room_d_sensor_level.push_back(room_d_sensor_level);
        day_room_m_sensor_level.push_back(room_m_sensor_level);
        day_room_total_sensor_level.push_back(room_total_sensor_level);
        day_window_level.push_back(window_level);
        day_room_ellapsed_time_level.push_back(room_ellapsed_time_level);
        day_sensor_ellapsed_time_level.push_back(sensor_ellapsed_time_level);
        day_sensor_per_window.push_back(cur_window_sensors);
        day_parts.push_back(Common::findDayPart(j,win_size_sec,f_day_parts));

        lower_timeslot=upper_timeslot;
        upper_timeslot+=win_size_sec;
        segment_cnt++;

    }


    DayContainer dc(day_room_m_sensor_level,
                    day_room_d_sensor_level,
                    day_room_total_sensor_level,
                    day_window_level,
                    day_sensor_m_level,
                    day_sensor_d_level,
                    day_room_ellapsed_time_level,
                    day_sensor_ellapsed_time_level,
                    day_sensor_per_window,
                    timeslots,
                    day_parts
                    );

    return dc;
}

vector<vector<int> > GenerateMeasures::computeStatisticalFeatures(vector<int> &fine_grain_seq, vector<int> &discovered_patterns, vector<vector<int> >& f_day_sensor_ellapsed_time)
{


    vector<vector<int> > sum_pattern_ellapsed_time;


    for (int j=0; j<discovered_patterns.size();j++)
    {
        int count=0;

        vector<int> sum_sensor_ellapsed_time(f_day_sensor_ellapsed_time[0].size(),0);

        for(int i=0;i<fine_grain_seq.size();i++)
        {
            if(discovered_patterns[j]==fine_grain_seq[i])
            {
                vector<int> day_sensor_m_level=f_day_sensor_ellapsed_time[i];
                count++;

                for(int c=0;c<day_sensor_m_level.size();c++)
                {

                    sum_sensor_ellapsed_time[c]+=(1.0*day_sensor_m_level[c]);
                }
            }

        }



        for(int c=0;c<sum_sensor_ellapsed_time.size();c++)
            sum_sensor_ellapsed_time[c]= ceil((sum_sensor_ellapsed_time[c]*1.0)/(count*1.0));

        for(int c=0;c<sum_sensor_ellapsed_time.size();c++)
        {
            if(sum_sensor_ellapsed_time[c]==1)
                sum_sensor_ellapsed_time[c]=2;

            if(sum_sensor_ellapsed_time[c]==0)
                sum_sensor_ellapsed_time[c]=1;

        }


        sum_pattern_ellapsed_time.push_back(sum_sensor_ellapsed_time);


    }
    return sum_pattern_ellapsed_time;
}


int GenerateMeasures::calculateTimeslotSegment(int win_size_sec)
{
    int timeslot_segment;
    return timeslot_segment=((24*3600)/(win_size_sec));
}
