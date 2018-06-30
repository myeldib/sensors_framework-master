#include<test-constants.h>
#include<feature-reader.h>
#include<similarity-measure.h>
#include<home.h>
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

TEST(SimilarityMeasure, createSimilarityMeasureObject)
{
    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/SimilarityMeasureTester/result/";
    FeatureContainer* featureContainer = createFeatureContainer(tmp_result_path);

    ASSERT_TRUE(featureContainer);

    SimilarityMeasure* similarityMeasure = new SimilarityMeasure(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

    ASSERT_TRUE(similarityMeasure);


    if(featureContainer)
    {
        delete featureContainer;
    }

    if(similarityMeasure)
    {
        delete similarityMeasure;
    }

}


TEST(SimilarityMeasure, compute1DEMD)
{
    float   dist =  0.0;
    float expectedEMDValue1= 60000*32*1.0;
    float expectedEMDValue2= 60000*31*1.0;

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/SimilarityMeasureTester/result/";

    FeatureContainer* featureContainer = createFeatureContainer(tmp_result_path);

    ASSERT_TRUE(featureContainer);

    vector<vector<float> > sensor_durations =featureContainer->getSensorDurations();

    SimilarityMeasure* similarityMeasure = new SimilarityMeasure(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

    ASSERT_TRUE(similarityMeasure);

    std::vector<float> sensor_durations_float1(sensor_durations[0].begin(), sensor_durations[0].end());
    std::vector<float> sensor_durations_float16(sensor_durations[16].begin(), sensor_durations[16].end());
    std::vector<float> sensor_durations_float17(sensor_durations[17].begin(), sensor_durations[17].end());

    similarityMeasure->compute1DEMD(sensor_durations_float1,sensor_durations_float16,dist);
    ASSERT_EQ(expectedEMDValue1,dist);

    similarityMeasure->compute1DEMD(sensor_durations_float1,sensor_durations_float17,dist);
    ASSERT_EQ(expectedEMDValue2,dist);

    if(featureContainer)
    {
        delete featureContainer;
    }

    if(similarityMeasure)
    {
        delete similarityMeasure;
    }

}

