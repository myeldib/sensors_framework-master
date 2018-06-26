/*
 * file-separator.h
 *
 *  Created on: 2017-03-31
 *      Author: Mohamed Eldib
 */

#ifndef FILESEPARATOR_H
#define FILESEPARATOR_H
#include<string>
#include<iostream>
#include <sstream>
#include<utilities.h>
#include<constants.h>
#include<logger.h>

using namespace std;

class FileSeparator{
public:
    FileSeparator(string file_name,string folder_path);
    void writeDailySensorData();
    int    writeValueToFile(int value);

    ~FileSeparator();

private:
    string createFolderFromDate(string folder_name);
    string extractDateFromLine(string line);
    string buildDateFileName(string folder_path, string file_name);

private:
    string file_name;
    string folder_path_;
    std::ofstream* output_file;

};

#endif // FILESEPARATOR_H
