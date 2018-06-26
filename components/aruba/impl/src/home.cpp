#include<home.h>
/*
 * home.cpp
 *
 *  Created on: 2017-03-31
 *      Author: Mohamed Eldib
 * This class reads dataset configuration file
 * It computes duration information in miliseconds, location information for every second
 */
/**
 * @brief Home::Home
 * @param home_setup_file
 */
Home::Home(string home_setup_file,string config_file)
{
    COUT<<"Home"<<endl;
    logging::INFO("Home");
    this->home_setup_file=home_setup_file;
    this->time_window_config_file=config_file;
    is_setup_read = false;
}


Home::~Home()
{
    COUT<<"~Home"<<endl;
    logging::INFO("~Home");
}

/**
 * @brief Home::readHomeSetup
 * read home setup config file
 */
void Home::readHomeSetup()
{
    COUT<<"readHomeSetup"<<endl;
    logging::INFO("readHomeSetup");

    if(is_setup_read)
        return;

    //read home setup config file
    ifstream in_file( this->home_setup_file );


    if (!in_file.is_open())
    {
        cerr<< "Can't find:"<<this->home_setup_file<<endl;
        exit(0);
    }

    while (in_file)
    {
        string line;
        vector<string> elem;
        if (!getline( in_file, line )) break;

        this->home_setup_config.push_back(line);

        Common::split(line,',',elem);

        this->sensors_id.push_back(elem[0]);
        this->sensors_position.push_back(Point(atoi(elem[1].c_str()),atoi(elem[2].c_str())));
        this->sensors_type.push_back(elem[3]);
        this->sensors_location.push_back(elem[4]);
    }

    //create an index for each string location
    createDistinctLocationIndex_();

    //read definitions of time-window parameters
    readConfig_();

    //check if time-window parameters are correct
    checkTimeWindowParameters_();

    is_setup_read = true;
}
/**
 * @brief Home::checkTimeWindowParameters
 * check if the definitions of time window parameters are correct
 * Correctness definition: 1- the dividion of the time constraint by the long duration time interval should not produce any fractions
 *                         2- the dividion of the long duration time interval by the short duration time interval should not produce any fractions
 *                         3- the histogram similarity threshould should be 0 and 1
 */
void Home::checkTimeWindowParameters_()
{
    COUT<<"checkTimeWindowParameters"<<endl;
    logging::INFO("checkTimeWindowParameters");

    if(this->time_constraint%this->location_time_interval!=0)
    {
        cerr<<"The dividion of the time constraint and the long duration time interval did not yield an integer number "<<endl;
        logging::ERROR("The dividion of the time constraint and the long duration time interval did not yield an integer number");
        exit(0);
    }
    if(this->location_time_interval%this->sensor_time_interval!=0)
    {
        cerr<<"The dividion of the long duration time interval and the short duration time interval did not yield an integer number "<<endl;
        logging::ERROR("The dividion of the time constraint and the long duration time interval did not yield an integer number");
        exit(0);
    }
    if(this->histogram_similarity_threshold<0 || histogram_similarity_threshold>1)
    {
        cerr<<"The histogram similarity threshould value should be between 0 and 1 "<<endl;
        logging::ERROR("The histogram similarity threshould value should be between 0 and 1");
        exit(0);
    }
}

/**
 * @brief Home::readConfig
 * read the definitions of time window parameters
 */
void Home::readConfig_()
{
    COUT<<"readConfig"<<endl;
    logging::INFO("readConfig");


    ifstream in_file(this->time_window_config_file);
    int line_id=0;

    while (in_file)
    {
        string line;
        vector<string> elem;

        if (!getline( in_file, line )) break;
        Common::split(line,Constants::EQUAL_SEPARATOR,elem);

        //read sensor time interval in seconds
        if(line_id==0)
            this->sensor_time_interval=atoi(elem[1].c_str());
        //read location time interval in seconds
        if(line_id==1)
            this->location_time_interval=atoi(elem[1].c_str());
        //read time constraint in seconds
        if(line_id==2)
            this->time_constraint=atoi(elem[1].c_str());
        //read long duration lambda for location histogram comparison
        if(line_id==3)
            this->long_duration_lambda=atoi(elem[1].c_str());
        //read short duration lambda for sensor histogram comparison
        if(line_id==4)
            this->short_duration_lambda=atoi(elem[1].c_str());
        //read exp threshold in order to determine if two histograms are similar or not with time constraints
        if(line_id==5)
            this->histogram_similarity_threshold=atof(elem[1].c_str());
        // read emd similarity threshold to merge two patterns from different days with time constraints
        if(line_id==6)
            this->emd_similarity_threshold=atof(elem[1].c_str());
        //read hierarchal thresold to merge patterns without time constraints
        if(line_id==7)
            this->hierarchal_threshold=atof(elem[1].c_str());
        if(line_id==8)
            this->include_other_activity_class=atof(elem[1].c_str());
        //assign the name of the activity label not the index
        else if(line_id> 8)
        {
            activity_labels_string_keys.insert(pair<string,int>(elem[0],atoi(elem[1].c_str())));
            activity_labels_int_keys.insert(pair<int,string>(atoi(elem[1].c_str()),elem[0]));
        }

        line_id++;
    }

}

/**
 * @brief Home::createDistinctLocationIndex
 * created a location index from location strings
 */
void Home::createDistinctLocationIndex_()
{
    COUT<<"createDistinctLocationIndex"<<endl;
    logging::INFO("createDistinctLocationIndex");

    int id=0;
    for(int i=0;i<this->sensors_location.size();i++)
    {
        //check if location in map, if not found, then insert it in map
        if ( this->locations.find(this->sensors_location[i]) == this->locations.end() )
        {
            this->locations[this->sensors_location[i]]=id;
            id++;
        }
        //if found then continue
        else
            continue;
    }
}
