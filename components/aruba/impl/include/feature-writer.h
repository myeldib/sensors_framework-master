#ifndef FEATUREWRITER_H
#define FEATUREWRITER_H
#include<feature-container.h>
#include<constants.h>
#include<utilities.h>
#include<logger.h>

class FeatureWriter
{
public:
  FeatureWriter(string folder_path, string extension, bool& success);
  ~FeatureWriter();
  void writeFeatures(FeatureContainer* featureContainer,Constants::Cluster_Type type);
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
  void writeRecognitionAccuracy_(FeatureContainer* fc);
private:
  string folder_path;
};

#endif // FEATUREWRITER_H
