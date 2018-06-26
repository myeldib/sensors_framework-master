#include "MeasureReader.h"

MeasureReader::MeasureReader()
{
    m_directory_name="../data/processed/";

}

void MeasureReader::readMeasuresData(string directtory)
{
    m_days_folder_file_names=Common::getFolderFileNames(directtory);
    readHeatMapData();


}

void MeasureReader::defineConstantCost()
{

    double cc=0;
    home.run();
    vector<Sensor> home_sensors= home.getHomeSensors();

    for(int i=0; i<home_sensors.size();i++)
    {
        Sensor s1=home_sensors[i];
        for(int j=1+i; j<home_sensors.size();j++)
        {
            Sensor s2=home_sensors[j];

            if(s1.getSensorType()=="area_sensor" && s2.getSensorType()=="area_sensor" )
                cc+=Common::calculateDistance(s1.getSensorPosition(),s2.getSensorPosition());
        }
    }

    cc=cc/home.getNumberOfRooms();
    m_cc=cc;


}

void MeasureReader::run()
{
    readMeasuresData(m_directory_name);
    readConfig();
    defineConstantCost();

}

void MeasureReader::readLoA(string filename)
{

    std::ifstream infile(filename.c_str());
    vector<int> line_loa;
    while (infile)
    {
        string line;

        if (!getline( infile, line )) break;


        line_loa.push_back(atoi(line.c_str()));
    }

    m_days_loa.push_back(line_loa);
    infile.close();

}


void MeasureReader::readRoomIds(string filename)
{

    std::ifstream infile(filename.c_str());

    vector< vector < string> > file_rid;


    while (infile)
    {
        string line;
        vector<string> elem;
        vector<string> line_rid;

        if (!getline( infile, line )) break;



        Common::split(line,' ',elem);

        for(int i=0; i<elem.size();i++)
            line_rid.push_back(elem[i]);

        file_rid.push_back(line_rid);
    }

    infile.close();

    m_days_rid.push_back(file_rid);

}

void MeasureReader::readConfig()
{
    string config_path="../data/config";
    std::ifstream infile( config_path.c_str());
    int line_index=0;


    while (infile)
    {
        string line;

        if (!getline( infile, line )) break;

        istringstream comma_sep_line( line );


        while (comma_sep_line)
        {

            string line_entries;
            if (!getline( comma_sep_line, line_entries, '=' )) break;

            if(line_index==0)
                s_timeslot=atoi(line_entries.c_str())*60;
            if(line_index==1)
                s_time_interval=atoi(line_entries.c_str());
            if(line_index==2)
                s_diameter=atoi(line_entries.c_str());


        }
        line_index++;
    }
}

void MeasureReader::readLoM(string filename)
{
    std::ifstream infile(filename.c_str());

    vector< vector < int> > file_lom;

    while (infile)
    {
        string line;
        vector<string> elem;
        vector<int> line_lom;


        if (!getline( infile, line )) break;


        Common::split(line,' ',elem);

        for(int i=0; i<elem.size();i++)
            line_lom.push_back(atoi(elem[i].c_str()));

        file_lom.push_back(line_lom);
    }

    infile.close();

    m_days_lom.push_back(file_lom);

}

string MeasureReader::prepareRoomIdFilePath(string directory)
{
    vector<string> elem;
    Common::split(directory,'/',elem);

    string folder="";

    for(int j=0;j<elem.size()-1;j++)
    {
        folder.append(elem[j]);
        folder.append("/");
    }

    string rid_path="";
    rid_path.append(folder);
    rid_path.append(elem[elem.size()-2]);
    rid_path.append("_rid.txt");

    return rid_path;

}

string MeasureReader::prepareLoMFilePath(string directory)
{
    vector<string> elem;
    Common::split(directory,'/',elem);

    string folder="";

    for(int j=0;j<elem.size()-1;j++)
    {
        folder.append(elem[j]);
        folder.append("/");
    }

    string lom_path="";
    lom_path.append(folder);
    lom_path.append(elem[elem.size()-2]);
    lom_path.append("_lom.txt");

    return lom_path;

}

string MeasureReader::prepareLoAFilePath(string directory)
{
    vector<string> elem;
    Common::split(directory,'/',elem);

    string folder="";

    for(int j=0;j<elem.size()-1;j++)
    {
        folder.append(elem[j]);
        folder.append("/");
    }

    string loa_path="";
    loa_path.append(folder);
    loa_path.append(elem[elem.size()-2]);
    loa_path.append("_loa.txt");

    return loa_path;

}


void MeasureReader::readHeatMapData()
{
    for(int i=0;i<m_days_folder_file_names.size();i++)
    {
        vector<string> elem;
        Common::split(m_days_folder_file_names[i],'/',elem);

        string folder="";
        string data_file=elem[elem.size()-1];

        for(int j=0;j<elem.size()-1;j++)
        {
            folder.append(elem[j]);
            folder.append("/");
        }

        m_days_names.push_back(elem[elem.size()-2]);

        string rid_path=prepareRoomIdFilePath(m_days_folder_file_names[i]);
        string loa_path=prepareLoAFilePath(m_days_folder_file_names[i]);
        string lom_path=prepareLoMFilePath(m_days_folder_file_names[i]);


        DIR *dir;
        vector<string> heatmaps_img,heatmap_mat;
        struct dirent *ent;

        if ((dir = opendir (folder.c_str())) != NULL)
        {
            while ((ent = readdir (dir)) != NULL)
            {
                string filename=ent->d_name;

                if(filename.find("_lom") == std::string::npos
                        && filename.find("_loa") == std::string::npos
                        && filename.find("_rid") == std::string::npos
                        && filename.find("_act") == std::string::npos
                        && string(".").compare(filename) != 0
                        && string("..").compare(filename) !=0
                        && filename.compare(data_file)!=0
                        && filename.find("_mat") == std::string::npos
                        && filename.find("_l") == std::string::npos)
                {
                    string heatmap_full_path=folder;
                    heatmap_full_path.append(filename);

                    heatmaps_img.push_back(heatmap_full_path);
                }

                if(filename.find("_rid") != std::string::npos)
                    readRoomIds(rid_path);

                if(filename.find("_loa") != std::string::npos)
                    readLoA(loa_path);

                if(filename.find("_lom") != std::string::npos)
                    readLoM(lom_path);
                if(filename.find("_mat") != std::string::npos)
                {
                    string heatmap_mat_full_path=folder;

                    heatmap_mat_full_path.append(filename);

                    heatmap_mat.push_back(heatmap_mat_full_path);
                }
            }
            closedir (dir);
        }

        sort(heatmaps_img.begin(), heatmaps_img.end());
        sort(heatmap_mat.begin(), heatmap_mat.end());

        m_days_heatmap_files.push_back(heatmaps_img);
        m_days_heatmap_mat_files.push_back(heatmap_mat);
    }
}
