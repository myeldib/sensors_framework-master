#ifndef COMMON_H
#define COMMON_H
#include <dirent.h>
#include "Sensor.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

using namespace cv;
using namespace std;

namespace deprecated
{


class Common
{
public:
    static vector<string> &split(const string &s, char delim, vector<string> &elems);
    static void saveImage(string path,Mat img);
    static vector<string> getFolderFileNames(string directory);
    static bool compareMapValues(const std::pair<std::string, int>& lhs,
                                 const std::pair<std::string, int>& rhs);
    static bool withinAreaSensorDiameter(Sensor local_sensor,Sensor area_sensor,int diameter);
    static  double calculateDistance(Point p1,Point p2);
    static string execTerminalCMD(string cmd);
    static void createFile(vector<int> &features, ofstream& output,int choice);
    static void sortFrequentPatterns(vector<int>& frequent_pattern,vector<int>& weights);
    static void sortPatternFeatures(vector<int>& frequent_pattern,vector<int>& discovered_patterns,vector<int>& weights,vector<vector<int> > & features,vector<vector<int> >& activity_labels,vector<int>& times);
    static bool createFolder(string directory);
    static vector<Sensor> useKalmanFilter(vector<Sensor>& sensor_positions);
    static void useKalmanFilter2(vector<Sensor> & sensor_positions);
    static int isUniqueGeneratedPattern(int pattern_gen,vector<int>& discovered_patterns);
    static int findDayPart(int unit,int win_size_sec,int f_day_parts);
    static vector<int> findPatternDayParts(vector<int> &discovered_patterns, vector<int> &sequence_patterns,int f_day_parts);
    static vector<string> getFolderNames(vector<string> folder_file_names);


};
}

#endif // COMMON_H
