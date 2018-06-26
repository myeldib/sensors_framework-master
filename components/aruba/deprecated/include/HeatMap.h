#ifndef HEATMAP_H
#define HEATMAP_H
#include <opencv2/opencv.hpp>

#include <iostream>
#include "Sensor.h"
using namespace cv;
using namespace std;
class HeatMap
{
public:
    HeatMap(Mat groundmap);
    Mat createHeatMap();
    Mat getOccupancyFrequency(){ return home_occ_freq;}
    Mat getOccupancyTime(){ return home_occ_time;}
    std::vector<cv::Point> getHomeGridPos(){ return home_grid_positions;}
    void computeOccupancyFrequency(vector<Sensor>& sensor_position,bool& is_valid_heatmap,set<string>& room_ids);
    void computeOccupancyTime(vector<Sensor>& sensor_position,bool& is_valid_heatmap, set<string>& room_ids);
    Mat createOccupancyFrequencyHeatMap(vector<Sensor>& sensor_position);
    Mat createOccupancyTimeHeatMap(vector<Sensor>& sensor_position);
    void printHomeGrid();
private:
    void sortHeatMapScores(vector<double>& scores,vector<Sensor>& positions);
private:
    Mat groundmap;
    Mat home_occ_freq,home_occ_time;
    std::vector<cv::Point> home_grid_positions;
};

#endif // HEATMAP_H
