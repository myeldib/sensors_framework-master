#include<between-day-clustering.h>
/**
 * @brief BetweenDayClustering::BetweenDayClustering
 * @param folder_path
 * @param home_setup_file
 * @param time_window_config
 */
BetweenDayClustering::BetweenDayClustering(string reader_folder_path,string writer_folder_path,string pattern_stat_folder_path,string home_setup_file, string time_window_config,bool& success)
{
    COUT<<"BetweenDayClustering"<<endl;

    logging::INFO("BetweenDayClustering");


    success = true;
    init(reader_folder_path,writer_folder_path,pattern_stat_folder_path,home_setup_file,time_window_config,success);

    if(!success)
    {
        return;
    }

}

/**
 * @brief BetweenDayClustering::init
 * @param reader_folder_path
 * @param writer_folder_path
 * @param pattern_stat_folder_path
 * @param home_setup_file
 * @param time_window_config
 */
void BetweenDayClustering::init(string reader_folder_path,string feature_writer_path,string pattern_stat_folder_path,string home_setup_file, string time_window_config,bool& success)
{
    logging::INFO("init");

    home = new Home(home_setup_file,time_window_config);
    home->readHomeSetup();

    this->reader_folder_path = reader_folder_path+
            "_"+
            std::to_string(home->getShortDurationLambda())+
            "_"+
            std::to_string(home->getLongDurationLambda())+
            "/";

    featureReader = new FeatureReader(home);

    this->feature_writer_path = feature_writer_path;

    featureWriter = new FeatureWriter(feature_writer_path,std::to_string((int)home->getEmdSimilarityThreshold()),home,success);

    if(!success)
    {
        return;
    }

    success = true;
    patternStatistics = new PatternsStatistics(pattern_stat_folder_path,home,success);

    if(!success)
    {
        return;
    }

    featureProcessor = new FeatureProcessor(home);
    similarityMeasure = new SimilarityMeasure(home_setup_file,time_window_config);
    sorterProcessor_ = new SorterProcessor();


    //similarity threshold for emd between (1.0 - 0.0 )
    this->emd_similarity_threshold = home->getEmdSimilarityThreshold();
    this->new_initial_pattern_index = 10000;
}

/**
 * @brief BetweenDayClustering::getMergedFeatureContainerPatterns
 * @return
 */
FeatureContainer* BetweenDayClustering::getMergedFeatureContainerPatterns()
{
    return merged_patterns_fc_;
}

/**
 * @brief BetweenDayClustering::~BetweenDayClustering
 */
BetweenDayClustering::~BetweenDayClustering()
{
    COUT<<"~BetweenDayClustering"<<endl;
    logging::INFO("~BetweenDayClustering");

    if(featureReader)
    {
        delete featureReader;
    }

    if(featureProcessor)
    {
        delete featureProcessor;
    }

    if(similarityMeasure)
    {
        delete similarityMeasure;
    }

    if(patternStatistics)
    {
        delete patternStatistics;
    }

    for(int i=0;i<featuresContainer.size();i++)
    {
        if(featuresContainer[i])
        {
            delete featuresContainer[i];
        }
    }

    if(sorterProcessor_)
      {
        delete sorterProcessor_;
      }
}

/**
 * @brief BetweenDayClustering::destroy
 */
void BetweenDayClustering::destroy()
{
    logging::INFO("destroy");
    delete this;
}

/**
 * @brief BetweenDayClustering::computeSimilarityBetweenPatterns_
 * @param merged_fc
 * @param fc
 */
void BetweenDayClustering::computeSimilarityBetweenPatterns_(FeatureContainer *merged_fc, FeatureContainer *fc)
{
    COUT<<"computeSimilarityBetweenPatterns_"<<endl;
    logging::INFO("computeSimilarityBetweenPatterns_");

    vector<vector<int> > patterns_to_merge;
    vector<vector<float> > similarity_score_per_pattern;

    vector<vector<float > > average_duration1 = merged_fc->getAverageSensorDurationsPerPattern();
    vector<vector<float > > average_duration2 = fc->getAverageSensorDurationsPerPattern();

    vector<float> time_index_per_pattern1 = merged_fc->getTimeIndexPerPattern();
    vector<float> time_index_per_pattern2 = fc->getTimeIndexPerPattern();

    for(int i = 0; i<average_duration1.size();i++)
    {
        vector<int> similar_patterns;
        vector<float> similarity_score;
        bool is_similar = false;

        int time_index1 = time_index_per_pattern1[i];

        for(int j =0 ; j<average_duration2.size();j++)
        {
            int time_index2 = time_index_per_pattern2[j];

            if(time_index1 != time_index2) break;


            float similarity_value = 0.0;

            similarityMeasure->compute1DEMD(average_duration1[i],average_duration2[j],similarity_value);

            if(similarity_value < emd_similarity_threshold)
            {
                similar_patterns.push_back(j);
                similarity_score.push_back(similarity_value);
                is_similar = true;
            }
        }

        //no patterns to merge from guest feature container with host feature container
        if(!is_similar)
        {
            similar_patterns.push_back(-1);
            similarity_score.push_back(-1);
        }

        similarity_score_per_pattern.push_back(similarity_score);
        patterns_to_merge.push_back(similar_patterns);

    }

    merged_fc->setPatternsToMerge(patterns_to_merge);
    merged_fc->setSimilarityScorePerPattern(similarity_score_per_pattern);

}

/**
 * @brief BetweenDayClustering::clusterDiscoveredPatterns_
 * @param day_index
 * @param fc1
 * @param fc2
 */
void BetweenDayClustering::clusterDiscoveredPatterns_(int day_index, FeatureContainer* merged_fc, FeatureContainer* fc)
{
    COUT<<"clusterDiscoveredPatterns_"<<endl;
    logging::INFO("clusterDiscoveredPatterns_");

    computeSimilarityBetweenPatterns_(merged_fc,fc);

    featureProcessor->sortPatternsUsingSimilarityScores(merged_fc);

    featureProcessor->mergePatterns(day_index,this->new_initial_pattern_index,merged_fc,fc);

    sorterProcessor_->radixSort(merged_fc);
    featureProcessor->computeOptimizedAverageSensorDurationPerPattern(merged_fc);
    featureProcessor->computeOptimizedMostCommonActivityLabelPerPattern(merged_fc);
}

/**
 * @brief BetweenDayClustering::run
 */
void BetweenDayClustering::run()
{
    COUT<<"run"<<endl;
    logging::INFO("run");

    //initialize features by processing features per pattern
    processFeatures_();

    FeatureContainer* merged = featuresContainer[0];

    patternStatistics->computeBetweenDayPatternStatistics(merged->getDiscoveredPatterns().size(),merged->getDiscoveredPatterns().size(),merged->getMostCommonActivityLabelPerPattern());

    for( int i =1; i<featuresContainer.size(); i++)
    {
        FeatureContainer* fc= featuresContainer[i];

        cout<<fc->getDayNamePerPattern()[0]<<endl;
        logging::INFO(fc->getDayNamePerPattern()[0]);

        clusterDiscoveredPatterns_(i,merged,fc);

        patternStatistics->computeBetweenDayPatternStatistics(fc->getDiscoveredPatterns().size(),merged->getDiscoveredPatterns().size(),merged->getMostCommonActivityLabelPerPattern());
    }

    merged_patterns_fc_ = merged;

    if(this->feature_writer_path !="")
    {
        featureWriter->writeFeatures(merged_patterns_fc_,Constants::Cluster_Type::between_day_cluster);
    }

}

/**
 * @brief BetweenDayClustering::processFeatures_
 */
void BetweenDayClustering::processFeatures_()
{
    COUT<<"processFeatures_"<<endl;
    logging::INFO("processFeatures_");

    featuresContainer = featureReader->readFeatures(this->reader_folder_path,Constants::Cluster_Type::within_day_cluster);

    for(int i = 0 ; i<featuresContainer.size();i++)
    {
        logging::INFO(featuresContainer[i]->getDayNamePerPattern()[0]);
        sorterProcessor_->radixSort(featuresContainer[i]);
        featureProcessor->computeOptimizedAverageSensorDurationPerPattern(featuresContainer[i]);
        featureProcessor->computeOptimizedMostCommonActivityLabelPerPattern(featuresContainer[i]);
    }
}

/**
 * @brief BetweenDayClustering::printInforForGiveDay
 * @param day
 */
void BetweenDayClustering::printInforForGiveDay(string day)
{
    COUT<<"printInforForGiveDay"<<endl;
    logging::INFO("printInforForGiveDay");

    vector<float> time_index = merged_patterns_fc_->getTimeIndexPerPattern();
    vector<int> sequence_patterns = merged_patterns_fc_->getSequencePatterns();
    vector<int> discovered_patterns = merged_patterns_fc_->getDiscoveredPatterns();
    vector<vector<float> > avg_sensor_durations_per_pattern = merged_patterns_fc_->getAverageSensorDurationsPerPattern();
    vector<string> day_names = merged_patterns_fc_->getDayNamePerPattern();
    vector<int> most_time_index = merged_patterns_fc_->getMostAssignedTimeIndexPerPatternInHourIndex();
    vector<int>  most_activiy_label = merged_patterns_fc_->getMostCommonActivityLabelPerPattern();
    vector<string> activity_label  = merged_patterns_fc_->getActivityLabel();

    int minutes=0;
    for(int i = 0 ; i <day_names.size();i++)
    {

        if(day_names[i]==day)
        {

            int  pos = distance(discovered_patterns.begin(), find(discovered_patterns.begin(), discovered_patterns.end(), sequence_patterns[i]));
            vector<float> avg_sensor_durations=avg_sensor_durations_per_pattern[pos];

            stringstream message;
            for(int k=0; k<avg_sensor_durations.size();k++)
                message<<k<<":"<<avg_sensor_durations[k]<<",";
            message<<endl;

            message<<day_names[i]<<"\t"<<sequence_patterns[i]<<"\t"<<time_index[i]<<"\t"<<activity_label[i]<<"\t"<<home->getActivityLabelIntStringMap().at(most_activiy_label[pos])<<endl;
            message<<"========================"<<endl;

            logging::INFO(message.str().c_str());
            minutes++;
        }

    }

    cout<<minutes<<endl;
}
