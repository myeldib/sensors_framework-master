/*
 * preprocessing-stage.cpp
 *
 *  Created on: 2017-03-14
 *      Author: Mohamed Eldib
 * This class reads the raw sensor data file
 * It computes duration information in miliseconds, location information for every second
 */


#include<preprocessing-stage.h>

/**
 * @brief PreprocessingStage::PreprocessingStage
 * @param dataset_name
 * @param home
 */
PreprocessingStage::PreprocessingStage(string dataset_name,string output_file,Home* home)
{
    COUT<<"PreprocessingStage"<<endl;
    logging::INFO("PreprocessingStage");

    //home setup file
    this->home=home;
    this->dataset_name=dataset_name;
    //initial assumption is that person is inside
    this->is_leave=false;
    //extracted info about sensors are written to this file
    this->ds_outfile.open (output_file.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
}
PreprocessingStage::~PreprocessingStage()
{
    COUT<<"~PreprocessingStage"<<endl;
}

/**
 * @brief PreprocessingStage::extractSensorInfo
 */
void PreprocessingStage::extractSensorInfo()
{
    COUT<<"extractSensorInfo"<<endl;
    logging::INFO("extractSensorInfo");

    this->home->readHomeSetup();

    ifstream ds_infile( dataset_name );
    Sensor* s1= new Sensor();
    bool is_first=true;
    int num_lines=1;

    while (ds_infile)
    {
        string line;
        Sensor* s2 = new Sensor();
        vector<string> elem;
        int i=0;

        if (!getline( ds_infile, line )) break;

        COUT<<num_lines<<endl;
        //logging::INFO(std::to_string(num_lines));
        num_lines++;


        //ignore temperature sensor event, ignore sensor off events
        if(line.find(Constants::TEMP_SENSOR) != std::string::npos)
            continue;


        //ignore all off events except the ones  which contain end of activity info
        if( ((line.find(Constants::OFF) != std::string::npos) && (line.find(Constants::BEGIN_ACTIVITY) == std::string::npos) && (line.find(Constants::END_ACTIVITY) == std::string::npos)))
            continue;


        Common::split(line,',',elem);


        //extract sensor information with on state
        while (i!=elem.size())
        {

            if(i==0)
                s2->setDate(elem[i]);

            else if(i==1)
            {

                string timestamp =s2->getDate()+Constants::DATE_SEPARATOR+elem[i]+Constants::MILI_SECOND_SEPARATOR;

                s2->setTimeMilli(Common::millis_from_date(timestamp));
                s2->setReadableTime(Common::date_from_millis(s2->getTimeMilli()));

            }

            else if(i==2)
                s2->setId(elem[i]);
            else if(i==3)
                s2->setState(elem[i]);
            else if(i==4)
                s2->setActivity(elem[i]);
            else if(i==5)
                s2->setActivityState(elem[i]);

            i++;
        }

        //set sensor type:local or area sensor
        s2->setType(home->getHomeSensorsType()[s2->getId2()]);
        //set sensor position in home
        s2->setPosition(home->getHomeSensorsPosition()[s2->getId2()]);
        //check where does the sensor belong to (which room)
        addLocationInfo(s2);


        if(is_first)
        {
            s1=s2;
            is_first=false;
        }
        else
        {
            //add artificial sensor events
            addArtificalSensorsPerSec(s1,s2);
            s1=s2;
        }



    }

    ds_outfile.close();

}

/**
 * @brief PreprocessingStage::markActvitiyState
 * @param s1
 * @param s2
 */
void PreprocessingStage::markActvitiyState(Sensor *s1, Sensor *s2)
{
    COUT<<"markActvitiyState"<<endl;
    //logging::INFO("markActvitiyState");

    //if first sensor event is mark of begin activity,
    //and second sensor event is not mark of end of activity,
    //then mark second sensor event as part of activity
    if(s1->getActivityState()==Constants::BEGIN_ACTIVITY && s2->getActivityState()!=Constants::END_ACTIVITY)
    {
        s2->setActivityState(s1->getActivityState());
        s2->setActivity(s1->getActivity());
    }
}

/**
 * @brief PreprocessingStage::addArtificalSensorsPerSec
 * @param s1
 * @param s2
 */
void  PreprocessingStage::addArtificalSensorsPerSec(Sensor *s1, Sensor *s2)
{

    COUT<<"addArtificalSensorsPerSec"<<endl;
    //logging::INFO("addArtificalSensorsPerSec");

    //get last and second last sensors
    Sensor* tmp_s = new Sensor();


    //if sensor event does not have activity state, then use previous actvity state for marking
    markActvitiyState(s1,s2);

    //compute time difference to add artificial sensors
    double time_diff_seconds=(s2->getTimeMilli()-s1->getTimeMilli());

    COUT<<"***"<<time_diff_seconds<<"\t"<<*s2<<endl;

    //different less than a second
    if(time_diff_seconds<=Constants::ONE_SECOND_IN_MELI)
    {
        s1->setDuration(time_diff_seconds);
        ds_outfile<<*s1<<endl;
    }
    else
    {
        //convert time difference from mili seconds to seconds for looping
        time_diff_seconds=time_diff_seconds/Constants::ONE_SECOND_IN_MELI;
        //write artificial sensors, except last sensor
        //the remaining of the duration is part of the last sensor
        for(int i=0;i<time_diff_seconds-1;i++)
        {

            //increment in seconds
            tmp_s->setTimeMilli(Common::millis_from_date(s1->getReadableTime())+Constants::ONE_SECOND_IN_MELI);
            tmp_s->setReadableTime(Common::date_from_millis(tmp_s->getTimeMilli()));

            //compute duration from time information
            double duration=computeDuration(s1,tmp_s);
            s1->setDuration( duration);

            //check if the different between last artifical sensor and last sensor is less than 2 seconds
            // then update duration
            if(computeDuration(s1,s2)<=Constants::ONE_SECOND_IN_MELI*2.0)
                s1->setDuration(computeDuration(s1,s2));

            COUT<<*s1<<"\t"<<tmp_s->getTimeMilli()<<"\t"<<computeDuration(s1,s2)<<endl;
            ds_outfile<<*s1<<endl;

            //update sensor to compute duration
            s1->setTimeMilli(tmp_s->getTimeMilli());
            s1->setReadableTime(tmp_s->getReadableTime());

        }
    }

}

/**
 * @brief PreprocessingStage::addArtificalSensorsPerMiliSec
 * @param s1
 * @param s2
 */
void PreprocessingStage::addArtificalSensorsPerMiliSec(Sensor *s1, Sensor *s2)
{

    COUT<<"addArtificalSensorsPerMiliSec"<<endl;
    //logging::INFO("addArtificalSensorsPerMiliSec");
    //get last and second last sensors
    Sensor* tmp_s = new Sensor();

    //if sensor event does not have activity state, then use previous actvity state for marking
    markActvitiyState(s1,s2);

    //compute time difference to add artificial sensors
    double time_diff_seconds=(s2->getTimeMilli()-s1->getTimeMilli());

    //write artificial sensors, except last sensor
    for(int i=0;i<time_diff_seconds-1;i++)
    {

        //increment in mili seconds
        tmp_s->setTimeMilli(Common::millis_from_date(s1->getReadableTime())+Constants::ONE_MILI_SECOND);
        tmp_s->setReadableTime(Common::date_from_millis(tmp_s->getTimeMilli()));

        //compute duration from time information
        double duration=computeDuration(s1,tmp_s);
        s1->setDuration( duration);

        ds_outfile<<*s1<<endl;

        //update sensor to compute duration
        s1->setTimeMilli(tmp_s->getTimeMilli());
        s1->setReadableTime(tmp_s->getReadableTime());

    }


}

/**
 * @brief PreprocessingStage::computeDuration
 * @param s1
 * @param s2
 * @return
 */
double PreprocessingStage::computeDuration(Sensor *s1, Sensor *s2)
{
    COUT<<"computeDuration"<<endl;
    //logging::INFO("computeDuration");

    return (s2->getTimeMilli()-s1->getTimeMilli())*1.0;
}

/**
 * @brief PreprocessingStage::detectLeaveHome
 * @param s
 */
void PreprocessingStage::detectLeaveHome(Sensor* s)
{
    COUT<<"detectLeaveHome"<<endl;
    //logging::INFO("detectLeaveHome");
    //if door is open, then person is outside
    if(s->getstate()==Constants::OPEN)
        is_leave=true;

    //if door is closed, then person is inside
    if(s->getstate()==Constants::CLOSE && is_leave)
        is_leave=false;

}
/**
 * @brief PreprocessingStage::addLocationInfo
 * @param s
 */
void PreprocessingStage::addLocationInfo(Sensor* s)
{
    COUT<<"addLocationInfo"<<endl;
    //logging::INFO("addLocationInfo");
    //detect if door sensor open or close
    detectLeaveHome(s);

    //if door sensor is open, then person is outside, otherwise inside
    if(is_leave)
        s->setLocation(home->getHomeSesnorLocation()[home->getHomeSesnorLocation().size()-1]);
    else
        s->setLocation(home->getHomeSesnorLocation()[s->getId2()]);

}
