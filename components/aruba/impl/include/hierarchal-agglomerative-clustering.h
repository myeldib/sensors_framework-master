#ifndef HIERARCHALAGGLOMERATIVECLUSTERING_H
#define HIERARCHALAGGLOMERATIVECLUSTERING_H
#include<logger.h>
#include<similarity-measure.h>
#include <home.h>
#include <numeric>
#include <math.h>
#include <feature-container.h>
#include <feature-processor.h>
#include <feature-reader.h>
#include <feature-writer.h>
#include <utilities.h>
#include <boost/thread.hpp>

class HierarchalAgglomerativeClustering{

public:
    HierarchalAgglomerativeClustering(string feature_reader_path,string hierarchal_clustering_path,string home_setup_file, string time_window_config,bool& success);
    ~HierarchalAgglomerativeClustering();
    void run();
    void destroy();
    FeatureContainer* getClusteredPatterns();
private:
    void computeProximityMatrix_(FeatureContainer* featureContainer,vector<vector<float> >& proximity_matrix,int& host_pattern, int& guest_pattern, float& similarity);
    void init_(string feature_reader_path,string hierarchal_clustering_path,string home_setup_file, string time_window_config,bool& success);
    void computeFeatures_(FeatureContainer* fc);
    void computePatternsToMerge_(vector<vector<float> >& proximity_matrix,int& host_pattern, int& guest_pattern, float& similarity);
    void mergePatterns_(FeatureContainer* featureContainer, int& host_pattern_index, int& guest_pattern_index);
    void updatePatterns_(FeatureContainer* featureContainer, int host_pattern_index,int guest_pattern_index);
    void divideContainerToSubContainers_(FeatureContainer* mergedSubFeatureContainer,vector<FeatureContainer*>& subFeatureContainers,int& num_threads);
    void divideContainer_(int from, int to,FeatureContainer* mergedSubFeatureContainers, FeatureContainer* subFeatureContainer);
    void mergeSubContainersToContainer_(vector<FeatureContainer*>& subFeatureContainers, FeatureContainer* mergedSubFeatureContainer);
    void computeSubContainersClusters_(FeatureContainer *mergedFeatureContainers,int& num_threads);
    void computeContainerClusters_(FeatureContainer *featureContainer);
    void updateProximityMatrix_(FeatureContainer* featureContainer,vector<vector<float> >& proximity_matrix,int& host_pattern_index, int& guest_pattern_index,float& similarity);
    void updateProximityMatrixWithGuestPattern_(int &host_pattern_index, int& guest_pattern_index, vector<vector<float> >& proximity_matrix);
    void updateProximityMatrixWithHostPattern_(int& host_pattern_index,int& guest_pattern_index,FeatureContainer* featureContainer,vector<vector<float> >& proximity_matrix);
    void removeInvalidDiscoveredPatterns_(FeatureContainer* featureContainer);
    void printDebugInfo(FeatureContainer* featureContainer, float sim);
    void checkFeatures(FeatureContainer* featureContainer, int& host_pattern_index, int& guest_pattern_index,vector<vector<float> >& proximity_matrix);

private:
    FeatureContainer* mergedSubFeatureContainers_;
    FeatureProcessor* featureProcessor_;
    FeatureReader* featureReader_;
    FeatureWriter* featureWriter_;
    SimilarityMeasure* similarityMeasure_;
    boost::thread_group g;
    Home* home_;


    string hierarchal_clustering_path_;
    float hierarchal_threshold;
    int last_discovered_patterns;
    int num_threads;

};

#endif // HIERARCHALAGGLOMERATIVECLUSTERING_H
