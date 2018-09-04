#ifndef FEATUREWRITER_H
#define FEATUREWRITER_H
#include<feature-container.h>
#include<constants.h>
#include<utilities.h>
#include<home.h>
#include <iostream>
#include<sorter-processor.h>
#include <string>
#include<logger.h>

class FeatureWriter
{
public:
  FeatureWriter(string folder_path, string extension, Home* home,bool& success);
  ~FeatureWriter();
  void writeFeatures(FeatureContainer* featureContainer,Constants::Cluster_Type type);
  void writeEvaluationResults(FeatureContainer* fc,Constants::Cluster_Type cluster_type,Constants::Result_Type result_type,string& path_to_results);
  void writePatternsStatistics(vector<int> num_of_discovered_patterns, vector<int> num_of_pruned_patterns, vector<int> num_of_discovered_activities);
private:
  void writeClusterRecognition_(FeatureContainer* fc);
  void writeBetweenDayPatterns_(FeatureContainer* fc);
  void writeDiscoveredPatterns(FeatureContainer* fc);
  void writeSequencePatterns(FeatureContainer* fc);
  void writeSensorDurations(FeatureContainer* fc);
  void writeActivityLabels(FeatureContainer* fc);
  void writeDayNames(FeatureContainer* fc);
  void writeTimeIndex(FeatureContainer* fc);
  void writePatternStatistics_(string file,vector<int> num_of_discovered_activities);
  void writeActualActivityLabels_(FeatureContainer* fc);
  void writePredictedActivityLabels_(FeatureContainer* fc);
  void writePredictedDiscoveredPatterns_(FeatureContainer* fc);
  void writeRecognitionAccuracy_(FeatureContainer* fc, string file_name);
  void writeModelRecognitionTestData_(FeatureContainer* fc);
  void writeModelRecognitionTrainData_(FeatureContainer* fc);
  void writeModelRecognition_(FeatureContainer* fc);
private:
  string folder_path;
  Home* home_;
  SorterProcessor* sorterProcessor_;
};

#endif // FEATUREWRITER_H
