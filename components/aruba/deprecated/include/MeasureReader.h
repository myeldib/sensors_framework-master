#ifndef MEASUREREADER_H
#define MEASUREREADER_H
#include "Common.h"
#include "Home.h"
#include <fstream>

class MeasureReader
{
public:
    MeasureReader();
    void run();
    vector< vector<string> > getDaysHeatMapFiles() { return m_days_heatmap_files;}
    vector< vector<string> > getDaysHeatMapMatFiles() { return m_days_heatmap_mat_files;}
    vector< vector< vector <string> > > getDaysRIdsFiles() { return m_days_rid;}
    vector< vector< vector <int> > > getDaysLoMFiles() { return m_days_lom;}
    vector<string> getDaysNames(){ return m_days_names;}
    vector< vector<int> > getDaysLoAFiles(){ return m_days_loa;}
    string getBaseDirectory() { return m_directory_name;}
    int getConstantCost(){ return (1.0*m_cc);}
    const  int& getTimeSlot() const{ return s_timeslot; }
    const  int& getSegmentCount() const{ return ((24*3600)/(s_timeslot)); }
    const  int& getTimeInterval() const{ return s_time_interval; }
    const  int& getDiameter() const{ return s_diameter; }
private:
    vector<string> m_days_folder_file_names;
    vector<string> m_days_names;
    vector< vector<string> > m_days_heatmap_files,m_days_heatmap_mat_files;
    vector< vector< vector <string> > > m_days_rid;
    vector< vector<int> > m_days_loa;
    vector< vector< vector <int> > > m_days_lom;
    string m_directory_name;
    int s_timeslot;
    int s_time_interval;
    int s_diameter;
    Home home;
    double m_cc;


private:
    void readMeasuresData(string directory);
    void readHeatMapData();
    void readLoA(string filename);
    void readLoM(string filename);
    void readRoomIds(string filename);
    void readMat(string filename);
    void defineConstantCost();
    void readConfig();
    string prepareRoomIdFilePath(string directory);
    string prepareLoAFilePath(string directory);
    string prepareLoMFilePath(string directory);
    string prepareLMatFilePath(string directory);

};

#endif // MEASUREREADER_H
