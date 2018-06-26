#include "SensorReader.h"
#include<opencv2/opencv.hpp>


SensorReader::SensorReader()
{
    s_directory_name="../data/days/";

}

void SensorReader::readConfig()
{
    string config_path="../data/config";
    std::ifstream infile( config_path.c_str());
    int line_index=0;


    while (infile)
    {

        string line;

        if (!getline( infile, line )) break;

        vector<string> elem;
        Common::split(line,'=',elem);

        if(line_index==0)
            s_timeslot=atoi(elem[1].c_str())*60;
        else if(line_index==1)
            s_time_interval=atoi(elem[1].c_str())*60;
        else if(line_index==2)
        {
            day_parts=atoi(elem[1].c_str());

        }
        else
        {
            activity_names.push_back(elem[0]);
            activity_indexes.push_back(atoi(elem[1].c_str()));
        }

        line_index++;
    }


}


void SensorReader::getAllSensorInfo()
{
    for(int i=0;i<s_days_folder_file_names.size();i++)
    {
        cout<<s_days_folder_file_names[i]<<endl;
        getSensorInfoFromFile(s_days_folder_file_names[i]);
    }
}

void SensorReader::getSensorInfoFromFile(string filename)
{

    vector<Sensor> sensors_in_file;
    vector<string> activity_in_file;
    std::ifstream infile(filename.c_str());

    while (infile)
    {

        vector<string> line_elem,time_elem;
        string line;
        stringstream date_time;

        if (!getline( infile, line )) break;

        cout<<line<<endl;
        size_t pos = 0;

        if((pos = line.find("  ", pos)) != std::string::npos)
            line.replace(pos, std::string("  ").length(), " ");

        Common::split(line,' ',line_elem);

        //Labelled activites don't have the same delimited spaces, and this is temp solution
        string buf; // Have a buffer string
        stringstream ss(line); // Insert the string into a stream
        vector<string> tokens; // Create vector to hold our words

        while (ss >> buf)
            tokens.push_back(buf);

        if(line_elem.size()>4)
        {
            stringstream activity;
            activity<<tokens[4]<<" "<<tokens[5];
            activity_in_file.push_back(activity.str());
        }
        else
            activity_in_file.push_back("-");



        Sensor s=home->getSensorInfo(line_elem[2]);

        s.setSensorState(line_elem[3]);

        Common::split(line_elem[1],':',time_elem);


        date_time<<line_elem[0]<<" "<<time_elem[0]<<":"<<time_elem[1]<<":"<<static_cast<ostringstream*>( &(ostringstream() << atoi(time_elem[2].c_str())) )->str();
        // date_time<<line_elem[0]<<" "<<line_elem[1];

        time_t epoch;
        struct tm tme;

        if (strptime(date_time.str().c_str(), "%Y-%m-%d %H:%M:%S", &tme) != NULL )
            epoch = mktime(&tme);

        string epoch_str= asctime(localtime(&epoch));


        if(epoch_str.find("23") != std::string::npos && sensors_in_file.size()==0)
            epoch=epoch+3600;


        s.setSensorTimestamp(epoch);
        s.setSensorDateTime(date_time.str());

        sensors_in_file.push_back(s);
    }

    infile.close();
    sensor_data.push_back(sensors_in_file);
    activity_data.push_back(activity_in_file);

    if (!infile.eof())
        cerr << "can't open file "<<filename<<endl;

}

void SensorReader::run()
{
    readConfig();
    home = new Home();
    home->run();
    s_num_rooms=home->getNumberOfRooms();
    room_sybmols=home->getRoomSymbols();
    d_num_sensors=home->getNumberOfDSensors();
    m_num_sensors=home->getNumberOfMSensors();
    sensors_to_rooms=home->getSensorsToRooms();
    s_days_folder_file_names=Common::getFolderFileNames(s_directory_name);
    s_days_folder_names=Common::getFolderNames(s_days_folder_file_names);

    getAllSensorInfo();

    delete home;

}

