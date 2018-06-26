/*
 * preprocessing-stage.h
 *
 *  Created on: 2017-03-14
 *      Author: Mohamed Eldib
 */
#ifndef PREPROCESSING_STAGE_H_
#define PREPROCESSING_STAGE_H_
#include <fstream>
#include<utilities.h>
#include <iostream>
#include<sensor.h>
#include<constants.h>
#include<home.h>
#include <boost/filesystem.hpp>
#include <sstream>
#include <string>
#include <vector>
#include<logger.h>

using namespace std;

class PreprocessingStage{

private:
    string dataset_name;
    Home* home;
    fstream ds_outfile;
    bool is_leave;
public:
    PreprocessingStage(string dataset_name,string output_file,Home* home);
    ~PreprocessingStage();
    void extractSensorInfo();
private:
    void  addArtificalSensorsPerSec(Sensor* s1, Sensor* s2);
    void  addArtificalSensorsPerMiliSec(Sensor* s1, Sensor* s2);
    void markActvitiyState(Sensor*s1,Sensor* s2);
    void addLocationInfo(Sensor* s);
    void detectLeaveHome(Sensor* s);
    double computeDuration(Sensor* s1,Sensor* s2);

};

#endif
