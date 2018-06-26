#include<test-constants.h>
#include<feature-processor.h>
#include<home.h>
#include<constants.h>
#include<feature-reader.h>
#include <gtest/gtest.h>

FeatureProcessor* createSUT(string folder_path,vector<FeatureContainer*>& featuresContainer)
{
  Home* home = new Home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

  if(!home)
    {
      return NULL;
    }


  FeatureReader* featureReader = new FeatureReader (home);

  if(!featureReader)
    {
      return NULL;
    }

  featuresContainer = featureReader->readFeatures(folder_path,Constants::Cluster_Type::within_day_cluster);


  FeatureProcessor* featureProcessor = new FeatureProcessor(home);

  if(!featureProcessor)
    {
      return NULL;
    }

  delete featureReader;

  return featureProcessor;

}

void prepareSimilarityData1(FeatureContainer* fc)
{
  COUT<<"prepareSimilarityData"<<endl;
  vector<int> discovered_patterns;
  discovered_patterns.push_back(1);
  discovered_patterns.push_back(2);

  vector<float> similarity_score_per_pattern1;
  similarity_score_per_pattern1.push_back(50000);
  similarity_score_per_pattern1.push_back(51000);
  similarity_score_per_pattern1.push_back(52000);
  similarity_score_per_pattern1.push_back(13000);
  similarity_score_per_pattern1.push_back(54000);

  vector<float> similarity_score_per_pattern2;
  similarity_score_per_pattern2.push_back(10000);
  similarity_score_per_pattern2.push_back(11000);
  similarity_score_per_pattern2.push_back(12000);
  similarity_score_per_pattern2.push_back(13000);
  similarity_score_per_pattern2.push_back(14000);
  similarity_score_per_pattern2.push_back(15000);

  vector<int> sequence_patterns1;
  sequence_patterns1.push_back(10);
  sequence_patterns1.push_back(11);
  sequence_patterns1.push_back(12);
  sequence_patterns1.push_back(13);
  sequence_patterns1.push_back(14);

  vector<int> sequence_patterns2;
  sequence_patterns2.push_back(20);
  sequence_patterns2.push_back(21);
  sequence_patterns2.push_back(22);
  sequence_patterns2.push_back(13);
  sequence_patterns2.push_back(24);
  sequence_patterns2.push_back(25);

  vector<vector<int> > patterns_to_merge;
  patterns_to_merge.push_back(sequence_patterns1);
  patterns_to_merge.push_back(sequence_patterns2);

  vector<vector<float>  >similarity_score_per_patterns;
  similarity_score_per_patterns.push_back(similarity_score_per_pattern1);
  similarity_score_per_patterns.push_back(similarity_score_per_pattern2);

  fc->setSimilarityScorePerPattern(similarity_score_per_patterns);
  fc->setDiscoveredPatterns(discovered_patterns);
  fc->setPatternsToMerge(patterns_to_merge);
}

void prepareSimilarityData2(FeatureContainer* fc)
{
  COUT<<"prepareSimilarityData"<<endl;
  vector<int> discovered_patterns;
  discovered_patterns.push_back(1);
  discovered_patterns.push_back(2);
  discovered_patterns.push_back(3);

  vector<float> similarity_score_per_pattern1;
  similarity_score_per_pattern1.push_back(50000);
  similarity_score_per_pattern1.push_back(51000);
  similarity_score_per_pattern1.push_back(52000);
  similarity_score_per_pattern1.push_back(13000);
  similarity_score_per_pattern1.push_back(54000);

  vector<float> similarity_score_per_pattern2;
  similarity_score_per_pattern2.push_back(10000);
  similarity_score_per_pattern2.push_back(11000);
  similarity_score_per_pattern2.push_back(12000);
  similarity_score_per_pattern2.push_back(13000);
  similarity_score_per_pattern2.push_back(14000);
  similarity_score_per_pattern2.push_back(15000);

  vector<float> similarity_score_per_pattern3;
  similarity_score_per_pattern3.push_back(10000);
  similarity_score_per_pattern3.push_back(11002);
  similarity_score_per_pattern3.push_back(12003);
  similarity_score_per_pattern3.push_back(13004);
  similarity_score_per_pattern3.push_back(14004);
  similarity_score_per_pattern3.push_back(15005);

  vector<int> sequence_patterns1;
  sequence_patterns1.push_back(10);
  sequence_patterns1.push_back(11);
  sequence_patterns1.push_back(12);
  sequence_patterns1.push_back(13);
  sequence_patterns1.push_back(14);

  vector<int> sequence_patterns2;
  sequence_patterns2.push_back(20);
  sequence_patterns2.push_back(21);
  sequence_patterns2.push_back(22);
  sequence_patterns2.push_back(13);
  sequence_patterns2.push_back(24);
  sequence_patterns2.push_back(25);

  vector<int> sequence_patterns3;
  sequence_patterns3.push_back(20);
  sequence_patterns3.push_back(21);
  sequence_patterns3.push_back(22);
  sequence_patterns3.push_back(13);
  sequence_patterns3.push_back(24);
  sequence_patterns3.push_back(25);

  vector<vector<int> > patterns_to_merge;
  patterns_to_merge.push_back(sequence_patterns1);
  patterns_to_merge.push_back(sequence_patterns2);
  patterns_to_merge.push_back(sequence_patterns3);

  vector<vector<float>  >similarity_score_per_patterns;
  similarity_score_per_patterns.push_back(similarity_score_per_pattern1);
  similarity_score_per_patterns.push_back(similarity_score_per_pattern2);
  similarity_score_per_patterns.push_back(similarity_score_per_pattern3);

  fc->setSimilarityScorePerPattern(similarity_score_per_patterns);
  fc->setDiscoveredPatterns(discovered_patterns);
  fc->setPatternsToMerge(patterns_to_merge);
}

TEST(FeatureProcessor, createFeatureProcessorObject)
{

  string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/FeatureProcessorTester/result/";
  vector<FeatureContainer*> featuresContainer;
  FeatureProcessor* featureProcessor =createSUT(tmp_result_path,featuresContainer);
  int expectedNumberOfDays = 1;

  ASSERT_TRUE(featureProcessor != NULL);
  ASSERT_EQ(expectedNumberOfDays, featuresContainer.size());

  for(int i = 0; i<featuresContainer.size();i++)
    {
      delete featuresContainer[i];
    }

  delete featureProcessor;

}

TEST(FeatureProcessor, computeAverageDuration)
{

  string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/FeatureProcessorTester/result/";
  vector<FeatureContainer*> featuresContainer;
  FeatureProcessor* featureProcessor =createSUT(tmp_result_path,featuresContainer);
  int expectedNumberOfPatterns = 2;
  int expectedNumberOfSensorsPerWindow = 35;
  int expectedAverageDuration = 60000;
  int expectedNumberOfDays = 1;

  ASSERT_TRUE(featureProcessor != NULL);
  ASSERT_EQ(expectedNumberOfDays, featuresContainer.size());

  FeatureContainer* featureContainer =  featuresContainer[0];
  featureProcessor->computeAverageSensorDurationPerPattern(featureContainer);



  vector<vector<float> > averageSensorDuratoinsPerPattern = featureContainer->getAverageSensorDurationsPerPattern();

  ASSERT_EQ(expectedNumberOfPatterns, averageSensorDuratoinsPerPattern.size());

  ASSERT_EQ(expectedNumberOfSensorsPerWindow,averageSensorDuratoinsPerPattern[0].size());
  ASSERT_EQ(expectedNumberOfSensorsPerWindow,averageSensorDuratoinsPerPattern[1].size());

  ASSERT_EQ(expectedAverageDuration,averageSensorDuratoinsPerPattern[0][2]);
  ASSERT_EQ(expectedAverageDuration,averageSensorDuratoinsPerPattern[1][3]);


  for(int i = 0; i<featuresContainer.size();i++)
    {
      delete featuresContainer[i];
    }

  delete featureProcessor;

}

TEST(FeatureProcessor, computeMostCommonActivity)
{

  string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/FeatureProcessorTester/result/";
  vector<FeatureContainer*> featuresContainer;
  FeatureProcessor* featureProcessor =createSUT(tmp_result_path,featuresContainer);
  int expectedNumberOfDays = 1;
  int expectedNumberOfPatterns = 2;
  int expectedNumberOfActivitiesPerWindow = 12;
  int expectedActivityLabelForFirstPattern = 4;
  int expectedActivityLabelForSecondPattern = 6;

  ASSERT_TRUE(featureProcessor != NULL);
  ASSERT_EQ(expectedNumberOfDays, featuresContainer.size());

  FeatureContainer* featureContainer =  featuresContainer[0];
  featureProcessor->computeMostCommonActivityLabelPerPattern(featureContainer);

  vector<int> mostCommonActivityLabelPerPattern = featureContainer->getMostCommonActivityLabelPerPattern();

  ASSERT_EQ(expectedNumberOfPatterns, mostCommonActivityLabelPerPattern.size());

  ASSERT_EQ(expectedActivityLabelForFirstPattern,mostCommonActivityLabelPerPattern[0]);
  ASSERT_EQ(expectedActivityLabelForSecondPattern,mostCommonActivityLabelPerPattern[1]);

  vector<vector<int> > activityLabelsPerPattern = featureContainer->getActivityLabelsPerPattern();

  ASSERT_EQ(expectedNumberOfPatterns, activityLabelsPerPattern.size());

  ASSERT_EQ(expectedNumberOfActivitiesPerWindow,activityLabelsPerPattern[0].size());
  ASSERT_EQ(expectedNumberOfActivitiesPerWindow,activityLabelsPerPattern[1].size());


  for(int i = 0; i<featuresContainer.size();i++)
    {
      delete featuresContainer[i];
    }

  delete featureProcessor;

}
TEST(FeatureProcessor, sortPatterns)
{

  string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/FeatureProcessorTester/result/";
  vector<FeatureContainer*> featuresContainer;
  FeatureProcessor* featureProcessor =createSUT(tmp_result_path,featuresContainer);
  int expectedNumberOfDays = 1;
  int expectedFirstPatternToMerge = 10;
  int expectedSecondPatternToMerge =20;
  int expectedFirstDiscoveredPattern = 1;
  int expectedSecondDiscoveredPattern = 2;


  ASSERT_TRUE(featureProcessor != NULL);
  ASSERT_EQ(expectedNumberOfDays, featuresContainer.size());

  FeatureContainer* featureContainer =  featuresContainer[0];

  prepareSimilarityData1(featureContainer);

  ASSERT_EQ(expectedFirstPatternToMerge,featureContainer->getPatternsToMerge()[0][0]);
  ASSERT_EQ(expectedSecondPatternToMerge,featureContainer->getPatternsToMerge()[1][0]);

  ASSERT_EQ(expectedFirstDiscoveredPattern,featureContainer->getDiscoveredPatterns()[0]);
  ASSERT_EQ(expectedSecondDiscoveredPattern,featureContainer->getDiscoveredPatterns()[1]);

  featureProcessor->sortPatternsUsingSimilarityScores(featureContainer);

  ASSERT_EQ(expectedSecondPatternToMerge,featureContainer->getSortedPatternsToMerge()[0][0]);
  ASSERT_EQ(expectedFirstPatternToMerge,featureContainer->getSortedPatternsToMerge()[1][0]);

  ASSERT_EQ(expectedSecondDiscoveredPattern,featureContainer->getSortedDiscoveredPatterns()[0]);
  ASSERT_EQ(expectedFirstDiscoveredPattern,featureContainer->getSortedDiscoveredPatterns()[1]);


  for(int i = 0; i<featuresContainer.size();i++)
    {
      delete featuresContainer[i];
    }

  delete featureProcessor;

}

TEST(FeatureProcessor, sortAndRemoveDuplicatePatterns)
{

  string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/FeatureProcessorTester/result/";
  vector<FeatureContainer*> featuresContainer;
  FeatureProcessor* featureProcessor =createSUT(tmp_result_path,featuresContainer);
  int expectedNumberOfDays = 1;
  int expectedFirstPatternToMerge = 10;
  int expectedSecondPatternToMerge =20;
  int expectedFirstDiscoveredPattern = 1;
  int expectedSecondDiscoveredPattern = 2;


  ASSERT_TRUE(featureProcessor != NULL);
  ASSERT_EQ(expectedNumberOfDays, featuresContainer.size());

  FeatureContainer* featureContainer =  featuresContainer[0];

  prepareSimilarityData2(featureContainer);

  ASSERT_EQ(expectedFirstPatternToMerge,featureContainer->getPatternsToMerge()[0][0]);
  ASSERT_EQ(expectedSecondPatternToMerge,featureContainer->getPatternsToMerge()[1][0]);

  ASSERT_EQ(expectedFirstDiscoveredPattern,featureContainer->getDiscoveredPatterns()[0]);
  ASSERT_EQ(expectedSecondDiscoveredPattern,featureContainer->getDiscoveredPatterns()[1]);

  featureProcessor->sortPatternsUsingSimilarityScores(featureContainer);


  ASSERT_EQ(expectedSecondPatternToMerge,featureContainer->getSortedPatternsToMerge()[0][0]);
  ASSERT_EQ(expectedFirstPatternToMerge,featureContainer->getSortedPatternsToMerge()[2][0]);


  ASSERT_EQ(expectedSecondDiscoveredPattern,featureContainer->getSortedDiscoveredPatterns()[0]);
  ASSERT_EQ(expectedFirstDiscoveredPattern,featureContainer->getSortedDiscoveredPatterns()[2]);


  for(int i = 0; i<featuresContainer.size();i++)
    {
      delete featuresContainer[i];
    }

  delete featureProcessor;

}
