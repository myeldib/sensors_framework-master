/*
 * home.h
 *
 *  Created on: 2017-03-31
 *      Author: Mohamed Eldib
 */
#ifndef HOME_H
#define HOME_H
#include<vector>
#include<string>
#include <fstream>
#include<constants.h>
#include <map>
#include <iostream>
#include <utilities.h>
#include<logger.h>

#include <boost/filesystem.hpp>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;
using namespace boost;
class Home{

public:
    Home(string home_setup_file,string time_window_config_file);
    ~Home();
    void readHomeSetup();
    vector<string> getHomeSensorsId(){return this->sensors_id;}
    vector<Point> getHomeSensorsPosition(){return this->sensors_position;}
    vector<string> getHomeSensorsType(){return this->sensors_type;}
    vector<string> getHomeSesnorLocation(){return this->sensors_location;}
    vector<string> getHomeSetupConfig(){return this->home_setup_config;}
    int getNumerOfLocations(){return this->locations.size();}
    int getLocationAsInt(string key){return this->locations.find(key)->second;}
    map<string,int> getLocationsAsMap(){return this->locations;}
    int getSensorTimeInterval(){return this->sensor_time_interval;}
    int getLocationTimeInterval(){return this->location_time_interval;}
    int getTimeConstraint(){return this->time_constraint;}
    void setTimeConstraint(int time_constraint){this->time_constraint=time_constraint;}
    int getLongDurationLambda(){return this->long_duration_lambda;}
    int getShortDurationLambda(){return this->short_duration_lambda;}
    void setLongDurationLambda(int long_duration_lambda){this->long_duration_lambda=long_duration_lambda;}
    void setShortDurationLambda(int short_duration_lambda){this->short_duration_lambda=short_duration_lambda;}
    float getHistogramSimilarityThreshold(){return this->histogram_similarity_threshold;}
    void setHistogramSimilarityThreshold(float exp_threshold){this->histogram_similarity_threshold=exp_threshold;}
    float getEmdSimilarityThreshold(){return this->emd_similarity_threshold;}
    void setEmdSimilarityThreshold(float emd_similarity_threshold){this->emd_similarity_threshold=emd_similarity_threshold;}
    float getHierarchalClusteringThreshold(){ return hierarchal_threshold;}
    void setHierarchalClusteringThreshold(float hierarchal_threshold){ this->hierarchal_threshold =hierarchal_threshold;}
    void setIncludeOtherActivityClass(int include_other_activity_class){this->include_other_activity_class=include_other_activity_class;}
    int  getIncludeOtherActivityClass(){return include_other_activity_class;}
    map<string,int> getActivityLabelStringIntMap() {return activity_labels_string_keys;}
    map<int,string> getActivityLabelIntStringMap() {return activity_labels_int_keys;}
private:
    void createDistinctLocationIndex_();
    void readConfig_();
    void checkTimeWindowParameters_();
private:
    //properties of installed sensors and locations in home
    //id of installed sensor
    vector<string> sensors_id;
    //physical position of installed sensor
    vector<Point> sensors_position;
    //type of sensor: local or area sensor
    vector<string> sensors_type;
    //the location of the sensor in the home: kitchen, bedroom,etc
    vector<string> sensors_location;
    vector<string> home_setup_config;
    //path to setup file for information about installed sensors and locations
    string home_setup_file;
    //path to config file for time_window parameters
    string time_window_config_file;
    int num_locations;
    map<string,int> locations;
    //This varaible defines the size of the small duration feature vector (sensor)
    int sensor_time_interval;
    //This variable defines the size of the long duration feature vector (location)
    int location_time_interval;
    //This variables determines if two feature vectors are within the same starting time
    int time_constraint;
    //These variables determine the threshold values to compute the distance between two histograms
    int long_duration_lambda,short_duration_lambda;
    //This variable indicates if the two histograms are similar or not based on their similarity distance
    float histogram_similarity_threshold;
    //This variable indicates if two histograms are similar or not based on the emd disntace (miliseconds)
    float emd_similarity_threshold;
    //This variable indicates if two patterns are similar or not based on structure, duration and time similarities
    float hierarchal_threshold;
    //flag to be used to include other activity class during cluster purity computation
    bool include_other_activity_class;
    //The set of activity labels
    map<string,int> activity_labels_string_keys;
    map<int,string> activity_labels_int_keys;
    bool is_setup_read;
};

#endif // HOME_H

