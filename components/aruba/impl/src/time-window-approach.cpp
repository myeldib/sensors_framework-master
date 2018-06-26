#include<time-window-approach.h>
/*
 * time-window-approach.cpp
 *
 *  Created on: 2017-03-31
 *      Author: Mohamed Eldib
 * This class sums the durations of each sensor and location inside time window intervals (sensor and location)
 */

/**
 * @brief TimeWindow::TimeWindow
 * @param home: home object which include info setup about dataset
 * @param config_file: window config setup
 */
TimeWindow::TimeWindow(Home* home,string folder_path)
{
    COUT<<"TimeWindow"<<endl;
    logging::INFO("TimeWindow");

    this->home=home;
    home->readHomeSetup();
    this->folder_path=folder_path;
    this->sensor_data_files=Common::getFolderFileNames(folder_path,"");
    this->num_threads = boost::thread::hardware_concurrency();

}

/**
 * @brief TimeWindow::~TimeWindow
 */
TimeWindow::~TimeWindow()
{
    COUT<<"~TimeWindow"<<endl;
    logging::INFO("~TimeWindow");
}

/**
 * @brief TimeWindow::createDurationWindows
 */
void TimeWindow::run()
{

    COUT<<"run"<<endl;
    logging::INFO("run");


    vector<vector<string> > sensor_data_files_per_thread;

    divideElementsBetweenThreads_(sensor_data_files,sensor_data_files_per_thread);

    //threading to generateClusters_
    for(int i =0;i<sensor_data_files_per_thread.size();i++)
    {
        vector<string> sub_sensor_data_files= sensor_data_files_per_thread[i];

        for(int j=0;j<sub_sensor_data_files.size();j++)
        {
            logging::INFO(sub_sensor_data_files[j]);
        }

        g.add_thread(new boost::thread([sub_sensor_data_files,this] { createDurationWindow_(sub_sensor_data_files); }));
    }

    g.join_all();

}

/**
 * @brief TimeWindow::divideElementsBetweenThreads_
 * @param sensor_data_files
 * @param sensor_data_files_per_thread
 */
void TimeWindow::divideElementsBetweenThreads_(vector<string> sensor_data_files,
                                                        vector<vector<string> > &sensor_data_files_per_thread)
{
    logging::INFO("divideElementsBetweenThreads_");

    //number of assigned elements to run in each thread
    vector<int> num_elements_per_thread;

    Common::computeNumElementsPerThread(sensor_data_files.size(),num_threads,num_elements_per_thread);

    int from=0;
    int to = num_elements_per_thread[0];

    for(int i =0; i<num_elements_per_thread.size();i++)
    {
        logging::INFO("from:"+
                      std::to_string(from)+
                      "\t"+
                      "to:"+
                      std::to_string(to));


        divideElements_(from,
                        to,
                        sensor_data_files,
                        sensor_data_files_per_thread);

        //update ranges
        from = to;
        to+=num_elements_per_thread[i+1];
    }
}

/**
 * @brief TimeWindow::divideElements_
 * @param from
 * @param to
 * @param sensor_data_files
 * @param sensor_data_files_per_thread
 */
void TimeWindow::divideElements_(int from,
                                          int to,
                                          vector<string> sensor_data_files,
                                          vector<vector<string> > &sensor_data_files_per_thread)
{
    logging::INFO("divideElements_");

    vector<string> sub_sensor_data_files;


    //copy data to sub container
    for(int i = from; i< to; i++)
    {
        sub_sensor_data_files.push_back(sensor_data_files[i]);
    }

    sensor_data_files_per_thread.push_back(sub_sensor_data_files);
}

/**
 * @brief TimeWindow::createDurationWindow_
 * @param sensor_data_files
 */
void TimeWindow::createDurationWindow_(vector<string> sensor_data_files)
{
    logging::INFO("createDurationWindow_");
    for(int i=0;i<sensor_data_files.size();i++)
    {
        cout<<"Processing file:"<<sensor_data_files[i]<<endl;
        logging::INFO("Processing file:"+sensor_data_files[i]);

        processDateFile(sensor_data_files[i]);
    }
}

/**
 * @brief TimeWindow::initializeClock
 * @param file_name: path to daily instance sensor events file
 * @return time in miliseconds at midnight of that day
 */
time_t TimeWindow::initializeClock(string file_name)
{
    COUT<<"initializeClock"<<endl;
    logging::INFO("initializeClock");

    //extract date from file path, and prepare clock from midnight
    string date_from_file=Common::extractDayFromFileName(file_name)+Constants::DATE_SEPARATOR+"00:00:00.00"+Constants::MILI_SECOND_SEPARATOR;

    //convert date to miliseconds
    return Common::millis_from_date(date_from_file);
}

/**
 * @brief TimeWindow::checkWindowDuration check if the durations sum is equal to time interval,
 * if duratiosn sum not equal to time interval, then update max duration with the difference
 * between durations sum and time interval
 * @param duration_info
 * @param time_interval
 */
void TimeWindow::checkWindowDuration(vector<int> &duration_info, int time_interval)
{
    COUT<<"checkWindowDuration"<<endl;
    //logging::INFO("checkWindowDuration");

    int wind_dur_sum=std::accumulate(duration_info.begin(),duration_info.end(),0);

    if(wind_dur_sum!=time_interval*Constants::ONE_SECOND_IN_MELI)
    {
        //if time interval is larger, than sum, then add leftover to max duration
        if(time_interval>wind_dur_sum)
            *max_element(duration_info.rbegin(), duration_info.rend())+=time_interval*Constants::ONE_SECOND_IN_MELI-wind_dur_sum;
        //if durations sum is larger, then leftover of durations sum from max duration
        else
            *max_element(duration_info.rbegin(), duration_info.rend())-=wind_dur_sum-time_interval*Constants::ONE_SECOND_IN_MELI;
    }
}

/**
 * @brief TimeWindow::writeDurationInfo
 * @param ds_outfile
 * @param duration_info
 * @param time_interval
 * @param is_duration
 */
void TimeWindow::writeDurationInfo(fstream &ds_outfile, vector<int> &duration_info,int time_interval,bool is_duration)
{
    COUT<<"writeDurationInfo"<<endl;
    //logging::INFO("writeDurationInfo");

    if(is_duration)
    {
        checkWindowDuration(duration_info,time_interval);

        for(int i=0;i<duration_info.size();i++)
        {
            ds_outfile<<duration_info[i]<<",";
        }

        ds_outfile<<endl;
    }
    else
    {
        int indexOfDominantActivity =std::distance(duration_info.begin(), std::max_element(duration_info.begin(), duration_info.end()));
        string activity = home->getActivityLabelIntStringMap().at(indexOfDominantActivity);
        ds_outfile<<activity<<endl;
    }
}

/**
 * @brief TimeWindow::selectMostDominantActivityInWindow selects the most dominant activity in at the small time interval window
 * @param s
 * @param lower_limit
 * @param activity_times
 * @param time_interval
 * @param ds_outfile
 */
void TimeWindow::selectMostDominantActivityInWindow(Sensor *s, time_t &lower_limit, vector<int> &activity_times, int time_interval, fstream &ds_outfile)
{

    COUT<<"selectMostDominantActivityInWindow"<<endl;
    //logging::INFO("selectMostDominantActivityInWindow");

    //set window upper limit
    time_t upper_limit=lower_limit+time_interval*Constants::ONE_SECOND_IN_MELI;

    //check if sensor time within window limit
    if(lower_limit<= s->getTimeMilli() && s->getTimeMilli() <=upper_limit)
    {
        activity_times[home->getActivityLabelStringIntMap().at(s->getActivity())]++;
    }
    else
    {
        //update window lower limit
        lower_limit+=time_interval*Constants::ONE_SECOND_IN_MELI;

        //write to file
        writeDurationInfo(ds_outfile,activity_times,time_interval,false);

        //clear pevious window duration info
        std::fill(activity_times.begin(), activity_times.end(), 0);

        //update current window with new activity info
        activity_times[home->getActivityLabelStringIntMap().at(s->getActivity())]++;
    }

}

/**
 * @brief TimeWindow::createDurationInfoWindow
 * @param s: sensor event
 * @param lower_limit: lower window time limit
 * @param duration_info: duration information for N sensors or M locations
 * @param time_interval: size of window
 * @param ds_outfile: write durations information to file
 */
void TimeWindow::createDurationInfoWindow(Sensor *s,time_t& lower_limit,vector<int>& duration_info,int time_interval,fstream& ds_outfile)
{

    COUT<<"createDurationInfoWindow"<<endl;


    //set window upper limit
    time_t upper_limit=lower_limit+time_interval*Constants::ONE_SECOND_IN_MELI;

    //check if sensor time within window limit
    if(lower_limit<= s->getTimeMilli() && s->getTimeMilli() <=upper_limit)
        duration_info[s->getId2()]+=s->getDuration();
    else
    {
        //update window lower limit
        lower_limit+=time_interval*Constants::ONE_SECOND_IN_MELI;
        //write to file
        writeDurationInfo(ds_outfile,duration_info,time_interval,true);
        //clear pevious window duration info
        std::fill(duration_info.begin(), duration_info.end(), 0);

        //update current window with new duration info
        duration_info[s->getId2()]+=s->getDuration();
    }

}

/**
 * @brief TimeWindow::processDateFile
 * @param file_name: process sensor events in order to computer durations in sensor and location time intervals
 */
void TimeWindow::processDateFile(string file_name)
{
    COUT<<"processDateFile"<<endl;
    logging::INFO("processDateFile");

    //read day instance file
    ifstream in_file(file_name);
    //prepare initial time in miliseconds from date
    time_t sensor_lower_limit=initializeClock(file_name);
    time_t activity_lower_limit=initializeClock(file_name);
    time_t location_lower_limit=initializeClock(file_name);

    //prepare holders for duration information
    vector<int> sensors_duration(home->getHomeSensorsId().size(),0);
    vector<int> locations_duration(home->getNumerOfLocations(),0);
    vector<int> activity_times(home->getActivityLabelStringIntMap().size(),0);

    //prepare output file
    fstream  sensor_outfile(Common::buildOutputFile(this->folder_path,Common::extractDayFromFileName(file_name),"sensors_duration"), std::fstream::out);
    fstream  location_outfile(Common::buildOutputFile(this->folder_path,Common::extractDayFromFileName(file_name),"locations_duration"), std::fstream::out);
    fstream  activity_outfile(Common::buildOutputFile(this->folder_path,Common::extractDayFromFileName(file_name),"activity_per_window"), std::fstream::out);


    while (in_file)
    {
        string line;
        vector<string> elem;
        Sensor* s= new Sensor();

        if (!getline( in_file, line )) break;

        Common::split(line,Constants::COMMA_SEPARATOR,elem);

        //extract sensor information from file
        s->setReadableTime(elem[0]);
        s->setDuration(atof(elem[1].c_str()));
        s->setTimeMilli(Common::millis_from_date(elem[0]));
        s->setId(elem[2]);
        s->setActivity(elem[4]);
        s->setLocation(elem[5]);
        //set sensor type:local or area sensor
        s->setType(home->getHomeSensorsType()[s->getId2()]);
        //set sensor position in home
        s->setPosition(home->getHomeSensorsPosition()[s->getId2()]);

        //compute sensor time interval window
        createDurationInfoWindow(s,sensor_lower_limit,sensors_duration,home->getSensorTimeInterval(),sensor_outfile);

        selectMostDominantActivityInWindow(s,activity_lower_limit,activity_times,home->getSensorTimeInterval(),activity_outfile);

        //set location id
        s->setId2(home->getLocationAsInt(s->getLocation()));

        //compute location time interval window
        createDurationInfoWindow(s,location_lower_limit,locations_duration,home->getLocationTimeInterval(),location_outfile);


        delete s;

    }

    //write last window
    writeDurationInfo(sensor_outfile,sensors_duration,home->getSensorTimeInterval(),true);
    writeDurationInfo(location_outfile,locations_duration,home->getLocationTimeInterval(),true);
    writeDurationInfo(activity_outfile,activity_times,home->getSensorTimeInterval(),false);


    //close files
    sensor_outfile.close();
    location_outfile.close();
    in_file.close();

}


