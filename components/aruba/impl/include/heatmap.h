/*
 * heatmap.cpp
 *
 *  Created on: 2017-04-03
 *      Author: Mohamed Eldib
 */

#ifndef GNUDRAWING_H
#define GNUDRAWING_H
#include<home.h>
#include <vector>
#include <cmath>
#include<constants.h>

using namespace std;

class HeatMap{
public:
    HeatMap(string file_path,Home* home);
    void drawMostOccuppiedTimeIntervalHeatMap();
    void drawClusterScatterPlot();
    void saveHeatMap(string file_path);
private:
    int computeNumberOfTimeIntervals(int  time_interval);
    void drawHeatMap( vector< vector< float > > frame );
    int determineNumberOfCols();
private:
    vector<vector<float> > img;
    string file_path;
    Home* home;
};

#endif // GNUDRAWING_H
