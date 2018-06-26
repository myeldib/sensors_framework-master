/*
 * heatmap.cpp
 *
 *  Created on: 2017-04-03
 *      Author: Mohamed Eldib
 * This class provides drawing functionality using gnuplot
 */

#include<heatmap.h>
#include <boost/tuple/tuple.hpp>
#include <gnuplot-iostream.h>
/**
 * @brief HeatMap::HeatMap
 * @param file_path: file_path to durations information (sensor or location)
 * @param home: configuration setup for dataset
 */
HeatMap::HeatMap(string file_path,Home* home)
{
    this->file_path=file_path;
    this->home=home;

}
/**
 * @brief HeatMap::computeNumberOfSlots
 * @param time_interval
 * @return number of time intervals (windows) in a day given the size of time interval
 */
int HeatMap::computeNumberOfTimeIntervals(int time_interval)
{

    return ((Constants::FULL_DAY_IN_HOURS*Constants::FULL_HOUR_IN_SECONDS)/(time_interval));
}

/**
 * @brief HeatMap::drawHeatMap
 * @param frame: 2d matrix to draw using gnuplot
 */
void HeatMap::drawHeatMap( vector< vector< float > > frame )
{

    stringstream tics;
    for (auto const& x : home->getLocationsAsMap())
        tics<<"'"<<x.first<<"' "<<x.second<<",";


    Gnuplot gp;
    //setup gnuplot
    gp<<" set size 1,1\n";
    gp << "unset key\n";
    //    gp<<"set term png\n";
    //    gp<< "set output '/home/meldib/printme.png'\n";
    //set colorbar
    gp << "set pm3d\n";

    gp<<"set cbtics ("<<tics.str()<<")\n";
    //disable 3d plotting
    gp << "set hidden3d\n";
    gp << "set view map\n";
    gp<<"set palette defined ( 0 '#000090',"
        " 1 '#000fff',"
        "  2 '#0090ff',"
        "  3 '#0fffee',"
        "   4 '#90ff70',"
        "    5 '#ffee00',"
        "   6 '#ff7000',"
        " 7 '#ee0000',"
        "  8 '#7f0000')";
    //setup color model

    gp << "set palette model RGB\n";
    //setup x and y ranges
    gp << "set xrange [ 0: "<<frame[0].size()<<"] \n";
    gp << "set yrange [ 0 : 1] \n";
    //plot the figure
    gp << "splot '-'\n";
    //    gp<< "set term x11";
    gp.send2d(frame);

    //this is a trick to keep figure visible
    int in=0;
    while (in == 0) {
        in = getchar();
    }
}
/**
 * @brief HeatMap::determineNumberOfCols
 * given the type of feature vector (sensor or location), determine the number of columns
 * @return the number of columns
 */
int HeatMap::determineNumberOfCols()
{
    //set number of cols according to duration type (sensor or location)
    if(this->file_path.find("location") != std::string::npos)
        return computeNumberOfTimeIntervals(home->getLocationTimeInterval());
    else
        return computeNumberOfTimeIntervals(home->getSensorTimeInterval());
}

void HeatMap::drawClusterScatterPlot()
{

    vector<int> x,y;

    for(int i=0;i<10;i++)
    {
        x.push_back(i);
        y.push_back(i*2);
    }


    Gnuplot gp;
    //gp<<"set term png\n";
    //gp<<"set output '../config/automobile.png'\n";
    gp<<"set title 'Price versus Curb weight'\n";
    gp<<"set xlabel 'Price in dollars'\n";
    gp<<"set ylabel 'Curb weight in pounds'\n";
    gp<<"set grid\n";
    gp<<"set timestamp\n";
    gp<<"set datafile separator ','\n";
    gp << "plot" << gp.file(x) << ":"<< gp.file(y) <<" notitle"<< std::endl;
   // gp<<"plot  '../config/automobile_data.txt' using 14:26 notitle \n";



}

/**
 * @brief HeatMap::drawMostOccuppiedTimeIntervalHeatMap
 * draw the maximum duration for each time interval
 */
void HeatMap::drawMostOccuppiedTimeIntervalHeatMap()
{
    //input file
    ifstream in_file( this->file_path);

    //number of rows
    int rows=2;

    //number of cols
    int cols=determineNumberOfCols();

    cout<<home->getLocationTimeInterval()<<endl;


    //frame which will contain the most occuppied durations
    vector< vector< float > > frame ( rows, vector<float> ( cols, 0 ) );

    int  time_interval_index=0;

    //read duration file
    while (in_file)
    {
        string line;
        vector<string> elem;
        int max_val=0;

        if (!getline( in_file, line )) break;

        //extract line
        Common::split(line,',',elem);

        //select max duration per line
        for(int i=0;i<elem.size();i++)
        {
            int val=atoi(elem[i].c_str());
            if(val>max_val)
            {
                max_val=val;
                frame[0][time_interval_index]=i;
                frame[1][time_interval_index]=i;
            }
        }

        //increment next entry in frame
        time_interval_index++;

    }

    drawHeatMap(frame);

}
