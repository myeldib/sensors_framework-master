#include"Home.h"

Home::Home()
{
    this->num_of_rooms=0;
    this->home_sensor_file="../data/sensor_positions";
    this->home_layout_file="../data/layout.jpg";
    this->home_groundmap_file="../data/groundmap.bmp";
    this->sensor_relations_file="../data/sensor_relations";
    this->m_num_sensors=0;
    this->d_num_sensors=0;

}



void Home::readSensorRelationFile(string filename)
{
    std::ifstream infile(filename.c_str());
    while (infile)
    {
        string line;
        if (!getline( infile, line )) break;
        this->local_to_area_sensors.push_back(line);
        room_symbols.push_back("");
        num_of_rooms++;
    }

}
void Home::loadHomeLayout(string filename)
{
    this->home_layout=imread(filename);
}
void Home::loadHomeGroundMap(string filename)
{
    this->home_groundmap=imread(filename);
}
void Home::createNoActivityMap()
{
    this->no_activity_map=Mat::zeros(home_groundmap.rows,home_groundmap.cols,home_groundmap.type());
}

void Home::run()
{
    readSensorRelationFile(this->sensor_relations_file);
    readHomeSensorsFile(this->home_sensor_file);
    loadHomeLayout(this->home_layout_file);
    loadHomeGroundMap(this->home_groundmap_file);
    createNoActivityMap();
}

void Home::readHomeSensorsFile(string filename)
{
    std::ifstream infile(filename.c_str());
    while (infile)
    {
        string line;

        if (!getline( infile, line )) break;


        Sensor s;
        vector<string>elem;
        Common::split(line,',',elem);

        s.setSensorId(elem[0]);

        for(int i=0;i<this->local_to_area_sensors.size();i++)
        {

            if(local_to_area_sensors[i].find(elem[0].c_str()) != std::string::npos)
            {
                vector<string>elem2;
                Common::split(local_to_area_sensors[i],',',elem2);
                s.setRoomId(elem2[0]);
                s.setRelationToAreaSensor(elem2[elem2.size()-1]);
                room_symbols[atoi((elem2[0].c_str()))]=elem2[elem2.size()-1].substr(0,1);
                sensor_to_room.push_back(atoi(elem2[0].c_str()));

            }
        }

        if(elem[0].find("M0") != std::string::npos)
        {

            s.setSensorMId(atoi(elem[0].substr(1,elem[0].size()).c_str())-1);
            s.setSensorPosition(Point(atoi(elem[1].c_str()),atoi(elem[2].c_str())));
            s.setSensorType(elem[3]);
            s.setOffset(0);
            m_num_sensors++;
        }

        if(elem[0].find("D0") != std::string::npos)
        {
            s.setSensorDId(atoi(elem[0].substr(1,elem[0].size()-1).c_str())-1);
            s.setSensorMId((atoi(elem[0].substr(1,elem[0].size()).c_str())+m_num_sensors)-1);

            s.setSensorPosition(Point(atoi(elem[1].c_str()),atoi(elem[3].c_str())));
            s.setSensorType(elem[5]);
            s.setOffset(50);
            d_num_sensors++;
        }

        this->home_sensors.push_back(s);

    }

    m_num_sensors=m_num_sensors+d_num_sensors;
}


Sensor Home::getSensorInfo(string sensor_id)
{

    if(sensor_id!="-1")
    {
        for(int i=0;i<home_sensors.size();i++)
        {
            Sensor s=home_sensors[i];
            if(s.getSensorId()==sensor_id)
                return s;
        }
        for(int i=0;i<home_sensors.size();i++)
        {
            Sensor s=home_sensors[i];
            if (s.getSensorMId()==atoi(sensor_id.c_str()))
                return s;
        }
    }

    else
        return Sensor("dummy");
}
void Home::printHomeSensorsInfo()
{
    vector<Sensor> home_sensors=getHomeSensors();
    for(int i=0;i<home_sensors.size();i++)
    {
        Sensor s=home_sensors[i];
        cout<<s.getSensorId()<<" "<<s.getSensorPosition()<<" "<<s.getSensorType()<<" "<<s.getSensorRoomId()<<" "<<s.getRelationToAreaSensor()<<endl;
    }

}

