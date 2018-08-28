/*
 * utilities.h
 *
 *  Created on: 2017-03-14
 *      Author: Mohamed Eldib
 */

#ifndef UTILITIES_H
#define UTILITIES_H
#include <constants.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include<map>
#include<algorithm>
#include<logger.h>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>


#include<sensor.h>

using namespace cv;
using namespace std;
using namespace boost;

class Common{

public:
    static vector<string> &split(const string &s, char delim, vector<string> &elems);
    uint64_t static millis_from_date(const string& s);
    static string date_from_millis(uint64_t ms);
    static vector<string> getFolderFileNames(string directory,string file_name);
    static map<int,int> countVectorElements(vector<int> v);
    static bool fileExists(const string& filename);
    static bool deleteFolder(const string& folder_path);
    static bool createFolder(const string& folder_path);
    static string buildOutputFile(string folder_path,string path,string file_name);
    static string extractDayFromFileName(string file_name);
    static vector<vector<string> > readFile(string& folder_path, char separator);
    static void eraseDuplicatesFromVector(vector<int>& vec);
    static vector<string> getFolderNamesWithPaths(string directory);
    static vector<string> getFolderNames(string directory);
    static void computeNumElementsPerThread(int num_elements, int& num_threads, vector<int> &elemnts_per_thread);
    static void getMostCommonElementInVector(vector<int>& v, int& element);


};

#endif // UTILITIES_H
