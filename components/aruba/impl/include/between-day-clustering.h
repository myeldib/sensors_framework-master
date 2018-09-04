#ifndef BETWEENDAYCLUSTERING_H
#define BETWEENDAYCLUSTERING_H
#include<feature-reader.h>
#include<feature-writer.h>
#include<feature-processor.h>
#include<similarity-measure.h>
#include<logger.h>
#include<sorter-processor.h>
#include<patterns-statistics.h>
#include<home.h>

class BetweenDayClustering{
public:
    void run();
    void destroy();
    FeatureContainer* getMergedFeatureContainerPatterns();
    BetweenDayClustering(string reader_folder_path,string writer_folder_path,string pattern_stat_folder_path,string home_setup_file, string time_window_config,bool& success);
    ~BetweenDayClustering();
private:
    void processFeatures_();
    void clusterDiscoveredPatterns_(int day_index,FeatureContainer* fc1, FeatureContainer* fc2);
    void computeSimilarityBetweenPatterns_(FeatureContainer* merged_fc, FeatureContainer* fc);
    void printInforForGiveDay(string day);
    void init(string reader_folder_path,string writer_folder_path,string pattern_stat_folder_path,string home_setup_file, string time_window_config,bool& success);


private:
    vector<FeatureContainer*> featuresContainer;
    FeatureProcessor* featureProcessor;
    SimilarityMeasure* similarityMeasure;
    Home* home;
    SorterProcessor* sorterProcessor_;
    PatternsStatistics* patternStatistics;

    FeatureReader* featureReader;
    FeatureWriter* featureWriter;

    string reader_folder_path;
    string feature_writer_path;
    string logger_path;
    float emd_similarity_threshold;
    int new_initial_pattern_index;
    FeatureContainer* merged_patterns_fc_;

};

#endif // BETWEENDAYCLUSTERING_H
