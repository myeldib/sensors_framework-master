#ifndef FEATUREREADER_H
#define FEATUREREADER_H
#include <utilities.h>
#include <home.h>
#include<feature-container.h>
#include<logger.h>


class FeatureReader{

public:

    FeatureReader(Home* home);
    ~FeatureReader();
    vector<FeatureContainer*> readFeatures(string folder_path,Constants::Cluster_Type type);

private:
    void readActivityFile_(string file_path,FeatureContainer* fc);
    void readSequencePatternsFile_(string file_path,FeatureContainer* fc);
    void readDiscoveredPatternsFile_(string file_path,FeatureContainer* fc);
    void readSensorDurationsFile_(string file_path,FeatureContainer* fc);
    void AssignTimeIndexPerPattern_(FeatureContainer* fc);
    void assignDayName_(string file_path,FeatureContainer* fc);
    void readTimeIndex_(string file_path,FeatureContainer* fc);
    void readDayNames_(string file_path,FeatureContainer* fc);
    void readWithinDayClusterFeatures_(string folder_path,vector<FeatureContainer*>& featureContainers);
    void readBetweenDayClusterFeatures_(string folder_path,vector<FeatureContainer*>& featureContainers);
    void readModelResults_(string folder_path, vector<FeatureContainer*>& featureContainers);
    void readClusterResults_(string folder_path, vector<FeatureContainer*>& featureContainers);
    void readActualActivityLabels_(string file_path,FeatureContainer* fc);
    void readPredictedActivityLabels_(string file_path,FeatureContainer* fc);

private:
    Home* home;

};

#endif // FEATUREREADER_H
