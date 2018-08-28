/*
 * heatmap.cpp
 *
 *  Created on: 2017-04-14
 *      Author: Mohamed Eldib
 * This class provides drawing functionality using gnuplot
 */
#ifndef HISTOGRAMCLUSTERING_H
#define HISTOGRAMCLUSTERING_H
#include<string>
#include<vector>
#include<constants.h>
#include<utilities.h>
#include<home.h>
#include<file-separator.h>
#include<logger.h>
#include<boost/thread.hpp>

using namespace std;
using namespace cv;

class HistogramClustering{
public:
  HistogramClustering(string hist_folder_path,Home* home);
  ~HistogramClustering();
  void run();
  void destroy();
private:
  vector<vector<int> > readHistogramFile(string file_path);
  vector<string> readActivityFile(string file_path);
  void clusterHistograms(vector<vector<int> > histograms,int time_interval,int time_constraint,int duration_lambda,vector<int>& clusters);
  void generateClusters_(vector<string> location_hist, vector<string> sensor_hist,vector<string> data_files,vector<string> short_interval_activit_files,vector<string> long_interval_activit_files);
  void divideElements_(int from,
                       int to,
                       vector<string> location_hist_files,
                       vector<string> sensor_hist_files,
                       vector<string> data_files,
                       vector<string> short_interval_activity_files,
                       vector<string> long_interval_activity_files,
                       vector<vector<string> >& location_hist_files_per_thread,
                       vector<vector<string> >& sensor_hist_files_per_thread,
                       vector<vector<string> >& data_files_per_thread,
                       vector<vector<string> >& short_interval_activity_files_per_thread,
                       vector<vector<string> >& long_interval_activity_files_per_thread);
  void divideElementsBetweenThreads_(vector<string> location_hist_files,
                                     vector<string> sensor_hist_files,
                                     vector<string> data_files,
                                     vector<string> short_interval_activity_files,
                                     vector<string> long_interval_activity_files,
                                     vector<vector<string> >& location_hist_files_per_thread,
                                     vector<vector<string> >& sensor_hist_files_per_thread,
                                     vector<vector<string> >& data_files_per_thread,
                                     vector<vector<string> >& short_interval_activty_files_per_thread,
                                     vector<vector<string> >& long_interval_activty_files_per_thread);
  int computeDaySegment(int window_id, int time_constraint, int time_interval);
  bool isSimilarHistograms(vector<int> hist1, vector<int> hist2,int duration_lambda);
  float computeHistogramsDifference(vector<int> hist1,vector<int> hist2);
  void mapLocationClustersToSensorClusters(vector<int> location_clusters,vector<vector<int> >& location_hist,vector<int>& sensor_clusters,vector<vector<int> > & sensor_hist,vector<string>& short_interval_activity_labels,vector<string>& long_interval_activity_labels, int short_time_interval, int long_time_interval);
  int serializeAbsDurationDifference(vector<vector<int> > histograms,int time_interval,int time_constraint,string file_name);
  void writeClusteredPatterns(vector<int> clustered_sensors,vector<vector<int> >& sensors_hist, vector<string>& activity_labels,string folder_name);
  void writeAbsDifferenceToFile(vector<vector<int> > location_durations, vector<vector<int> >sensor_durations);


private:
  string hist_folder_path;
  bool serialize_results;
  Home* home;
  boost::thread_group g;
  int num_threads;

};

#endif // HISTOGRAMCLUSTERING_H
