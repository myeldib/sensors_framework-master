#include<patterns-statistics.h>

/**
 * @brief PatternsStatistics::PatternsStatistics
 * @param feature_writer_path
 * @param home
 */
PatternsStatistics::PatternsStatistics(string feature_writer_path,Home* home,bool& success)
{
    logging::INFO("PatternsStatistics");


    init(feature_writer_path,home,success);


    if(!success)
    {
        destroy();
        return ;
    }

}

/**
 * @brief PatternsStatistics::init
 * @param feature_writer_path
 * @param home
 * @param success
 */
void PatternsStatistics::init(string feature_writer_path,Home* home,bool& success)
{
    logging::INFO("init");


    this->home_ = home;
    this->home_->readHomeSetup();

    this->feature_writer_path=feature_writer_path;


    featureWriter = new FeatureWriter(feature_writer_path,std::to_string((int)this->home_->getEmdSimilarityThreshold()),home_,success);
    this->success=success;

    if(!this->success)
    {
        success = false;
        return;
    }

    for(int i = 0;i<home->getActivityLabelStringIntMap().size();i++)
    {
        is_activity_discovered_.push_back(false);
    }
}

/**
 * @brief PatternsStatistics::destroy
 */
void PatternsStatistics::destroy()
{
    logging::INFO("destroy");
    delete this;
}

/**
 * @brief PatternsStatistics::~PatternsStatistics
 */
PatternsStatistics::~PatternsStatistics()
{
    logging::INFO("~PatternsStatistics");

    if(featureWriter)
    {
        if(this->feature_writer_path !="" && success)
        {
            featureWriter->writePatternsStatistics(num_of_all_discovered_patterns_,num_of_pruned_patterns_,num_of_discovered_activity_);
        }

        delete featureWriter;
    }

}

/**
 * @brief PatternsStatistics::computeBetweenDayPatternStatistics
 * @param num_of_all_patterns
 * @param num_of_pruned_patterns
 * @param mostCommonActivityLabelPerPattern
 */
void PatternsStatistics::computeBetweenDayPatternStatistics(int num_of_all_patterns, int num_of_pruned_patterns, vector<int> mostCommonActivityLabelPerPattern)
{
    logging::INFO("computeBetweenDayPatternStatistics");

    if(this->feature_writer_path !="")
    {
        computeNumberOfDiscoveredPatterns_(num_of_all_patterns);
        computeOfNumberPrunedPatterns_(num_of_pruned_patterns);
        computeNumberOfDiscoveredActivities_(mostCommonActivityLabelPerPattern);
    }
}

/**
 * @brief PatternsStatistics::computeNumberOfDiscoveredActivities
 * @param mostCommonActivityLabelPerPattern
 */
void PatternsStatistics::computeNumberOfDiscoveredActivities_(vector<int> mostCommonActivityLabelPerPattern)
{
    for(int i = 0; i<mostCommonActivityLabelPerPattern.size();i++)
    {
        is_activity_discovered_[mostCommonActivityLabelPerPattern[i]]=true;
    }

    int count_discovered_activity = 0;

    for(int i = 0;i<is_activity_discovered_.size();i++)
    {
        if(is_activity_discovered_[i])
        {
            count_discovered_activity++;
        }
    }

    num_of_discovered_activity_.push_back(count_discovered_activity);
}

/**
 * @brief PatternsStatistics::computeNumberPrunedPatterns
 * @param num_of_pruned_patterns
 */
void PatternsStatistics::computeOfNumberPrunedPatterns_(int num_of_pruned_patterns)
{
    float sum_of_all_discovered_patterns = 0.0;
    for(int i =0; i <num_of_all_discovered_patterns_.size();i++)
    {
        sum_of_all_discovered_patterns+=num_of_all_discovered_patterns_[i];
    }

    sum_of_all_discovered_patterns = sum_of_all_discovered_patterns-num_of_pruned_patterns;
    num_of_pruned_patterns_.push_back(sum_of_all_discovered_patterns);
}
/**
 * @brief PatternsStatistics::computeTotalNumberOfDiscoveredPatterns
 * @param num_of_all_patterns
 */
void PatternsStatistics::computeNumberOfDiscoveredPatterns_(int num_of_all_patterns)
{
    num_of_all_discovered_patterns_.push_back(num_of_all_patterns);
}


