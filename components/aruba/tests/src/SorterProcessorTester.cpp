#include<home.h>
#include<sorter-processor.h>
#include<feature-reader.h>
#include<test-constants.h>
#include <gtest/gtest.h>


FeatureContainer* createFeatureContainer(string folder_path)
{
    Home* home = new  Home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

    if(!home)
    {
        return NULL;
    }

    FeatureReader* featureReader = new FeatureReader (home);

    if(!featureReader)
    {
        return NULL;
    }


    vector<FeatureContainer*> featuresContainer =featureReader->readFeatures(folder_path,Constants::Cluster_Type::within_day_cluster);


    FeatureContainer* featureContainer = featuresContainer[0];

    if(!featureContainer)
    {
        return NULL;
    }

    delete featureReader;

    return featureContainer;

}
TEST(SorterProcessor, createSorterProcessor)
{
    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/SorterProcessorTester/radix_sort/";
    FeatureContainer* featureContainer = createFeatureContainer(tmp_result_path);

    ASSERT_TRUE(featureContainer);

    SorterProcessor* sorterProcessor = new SorterProcessor();

    ASSERT_TRUE(sorterProcessor);


    if(featureContainer)
    {
        delete featureContainer;
    }

    if(sorterProcessor)
    {
        delete sorterProcessor;
    }
}

TEST(SorterProcessor, sortUsingRadixAlgorithm)
{
    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/SorterProcessorTester/radix_sort/";
    FeatureContainer* featureContainer = createFeatureContainer(tmp_result_path);
    string expectedSortedActivityLabel = "Work";
    int expectedSortedSequencePattern = 1000;
    float expectedSortedSensorDuration = 15000.0;

    ASSERT_TRUE(featureContainer);

    SorterProcessor* sorterProcessor = new SorterProcessor();

    ASSERT_TRUE(sorterProcessor);

    sorterProcessor->radixSort(featureContainer);

    vector<string> activity_labels = featureContainer->getActivityLabel();
    vector<int> sequence_patterns = featureContainer->getSequencePatterns();
    vector<vector<float> > sensors_durations = featureContainer->getSensorDurations();
    vector<float> sensor_dur = sensors_durations[sensors_durations.size()-1];


    ASSERT_EQ(expectedSortedActivityLabel,activity_labels[activity_labels.size()-1]);
    ASSERT_EQ(expectedSortedSequencePattern,sequence_patterns[sequence_patterns.size()-1]);
    ASSERT_FLOAT_EQ(expectedSortedSensorDuration,sensor_dur[sensor_dur.size()-1]);


    if(featureContainer)
    {
        delete featureContainer;
    }

    if(sorterProcessor)
    {
        delete sorterProcessor;
    }
}
