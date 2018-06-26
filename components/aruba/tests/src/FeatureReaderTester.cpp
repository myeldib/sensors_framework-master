#include<test-constants.h>
#include<feature-reader.h>
#include<home.h>
#include <gtest/gtest.h>



TEST(FeatureReader, createFeatureReaderObject)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/FeatureReaderTester/result/";

    Home* home = new Home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

    ASSERT_TRUE(home);

    FeatureReader* featureReader = new FeatureReader(home);

    ASSERT_TRUE(featureReader != NULL);

    delete featureReader;

}

TEST(FeatureReader, readFeaturesFilesWithinDayCluster)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/FeatureReaderTester/result/";
    int expectedNumberOfDays = 1;
    int expectedNumberOfDiscoveredPatterns = 2;
    int expectedNumberOfSequencePatterns = 90;
    float expectedThirdMinuteForFirstPattern = 0.03;
    float expectedLastMinuteForFirstPattern = 0.59;
    float expectedThirdMinuteForSecondPattern = 1.03;
    float expectedLastMinuteForSecondPattern = 1.29;
    string expectedDayName = "2010-11-04";

    Home* home = new Home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

    ASSERT_TRUE(home);

    FeatureReader* featureReader = new FeatureReader(home);

    ASSERT_TRUE(featureReader != NULL);

    vector<FeatureContainer*> featuresContainer = featureReader->readFeatures(tmp_result_path,Constants::Cluster_Type::within_day_cluster);

    ASSERT_EQ(expectedNumberOfDays, featuresContainer.size());

    FeatureContainer* featureContainer = featuresContainer[0];
    vector<int> discovered_patterns = featureContainer->getDiscoveredPatterns();
    vector<int> sequence_patterns= featureContainer->getSequencePatterns();
    vector<string> activity_labels = featureContainer->getActivityLabel();
    vector<vector<float> > sensor_durations = featureContainer->getSensorDurations();
    vector<float> time_index_per_pattern = featureContainer->getTimeIndexPerPattern();
    vector<string> day_name_per_patten = featureContainer->getDayNamePerPattern();

    ASSERT_EQ(expectedNumberOfDiscoveredPatterns,discovered_patterns.size());
    ASSERT_EQ(expectedNumberOfSequencePatterns,sequence_patterns.size());
    ASSERT_EQ(expectedNumberOfSequencePatterns,activity_labels.size());
    ASSERT_EQ(expectedNumberOfSequencePatterns,sensor_durations.size());
    ASSERT_EQ(expectedDayName,day_name_per_patten[0]);

    ASSERT_FLOAT_EQ(expectedThirdMinuteForFirstPattern,time_index_per_pattern[3]);
    ASSERT_FLOAT_EQ(expectedLastMinuteForFirstPattern,time_index_per_pattern[59]);
    ASSERT_FLOAT_EQ(expectedThirdMinuteForSecondPattern,time_index_per_pattern[63]);
    ASSERT_FLOAT_EQ(expectedLastMinuteForSecondPattern,time_index_per_pattern[89]);


    delete featureReader;
}

TEST(FeatureReader, readFeaturesFilesBetweenDayCluster)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/FeatureReaderTester/between_day_cluster/";
    int expectedNumberOfFiles = 1;
    int expectedNumberOfDiscoveredPatterns = 2;
    int expectedNumberOfSequencePatterns = 90;
    float expectedThirdMinuteForFirstPattern = 0.03;
    float expectedLastMinuteForFirstPattern = 0.59;
    float expectedThirdMinuteForSecondPattern = 1.03;
    float expectedLastMinuteForSecondPattern = 1.29;
    string expectedDayName = "2010-11-04";

    Home* home = new Home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

    ASSERT_TRUE(home);

    FeatureReader* featureReader = new FeatureReader(home);

    ASSERT_TRUE(featureReader != NULL);

    vector<FeatureContainer*> featuresContainer = featureReader->readFeatures(tmp_result_path,Constants::Cluster_Type::between_day_cluster);

    ASSERT_EQ(expectedNumberOfFiles, featuresContainer.size());

    FeatureContainer* featureContainer = featuresContainer[0];
    vector<int> discovered_patterns = featureContainer->getDiscoveredPatterns();
    vector<int> sequence_patterns= featureContainer->getSequencePatterns();
    vector<string> activity_labels = featureContainer->getActivityLabel();
    vector<vector<float> > sensor_durations = featureContainer->getSensorDurations();
    vector<float> time_index_per_pattern = featureContainer->getTimeIndexPerPattern();
    vector<string> day_name_per_patten = featureContainer->getDayNamePerPattern();

    ASSERT_EQ(expectedNumberOfDiscoveredPatterns,discovered_patterns.size());
    ASSERT_EQ(expectedNumberOfSequencePatterns,sequence_patterns.size());
    ASSERT_EQ(expectedNumberOfSequencePatterns,activity_labels.size());
    ASSERT_EQ(expectedNumberOfSequencePatterns,sensor_durations.size());
    ASSERT_EQ(expectedDayName,day_name_per_patten[0]);

    ASSERT_FLOAT_EQ(expectedThirdMinuteForFirstPattern,time_index_per_pattern[3]);
    ASSERT_FLOAT_EQ(expectedLastMinuteForFirstPattern,time_index_per_pattern[59]);
    ASSERT_FLOAT_EQ(expectedThirdMinuteForSecondPattern,time_index_per_pattern[63]);
    ASSERT_FLOAT_EQ(expectedLastMinuteForSecondPattern,time_index_per_pattern[89]);


    delete featureReader;
}
