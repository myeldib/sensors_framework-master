#include<test-constants.h>
#include<hierarchal-agglomerative-clustering.h>
#include<home.h>
#include<constants.h>
#include<feature-reader.h>
#include <gtest/gtest.h>


TEST(HierarchalAgglomerativeClustering, createHierarchalAgglomerativeClusteringObject)
{

  string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/HierarchalAgglomerativeClusteringTester/between_day_cluster_different_two_patterns/";
  bool success = true;
  HierarchalAgglomerativeClustering* hierarchalAgglomerativeClustering = new HierarchalAgglomerativeClustering(tmp_result_path,"",TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG,success);

  hierarchalAgglomerativeClustering->destroy();
}



TEST(HierarchalAgglomerativeClustering, clusterTwoDifferentPatterns)
{

  string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/HierarchalAgglomerativeClusteringTester/between_day_cluster_different_two_patterns/";
  bool success = true;
  HierarchalAgglomerativeClustering* hierarchalAgglomerativeClustering = new HierarchalAgglomerativeClustering(tmp_result_path,"",TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG,success);
  int expectedNumberOfDiscoveredPatterns = 2;

  hierarchalAgglomerativeClustering->run();

  FeatureContainer* featureContainer = hierarchalAgglomerativeClustering->getClusteredPatterns();

  ASSERT_EQ(expectedNumberOfDiscoveredPatterns,featureContainer->getDiscoveredPatterns().size());

  hierarchalAgglomerativeClustering->destroy();
}


TEST(HierarchalAgglomerativeClustering, clusterSameTwoPatterns)
{

  string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/HierarchalAgglomerativeClusteringTester/between_day_cluster_same_two_patterns/";
  bool success = true;
  HierarchalAgglomerativeClustering* hierarchalAgglomerativeClustering = new HierarchalAgglomerativeClustering(tmp_result_path,"",TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG,success);
  int expectedNumberOfDiscoveredPatterns = 1;

  hierarchalAgglomerativeClustering->run();

  FeatureContainer* featureContainer = hierarchalAgglomerativeClustering->getClusteredPatterns();

  ASSERT_EQ(expectedNumberOfDiscoveredPatterns,featureContainer->getDiscoveredPatterns().size());

  hierarchalAgglomerativeClustering->destroy();
}


TEST(HierarchalAgglomerativeClustering, clusterSameFourPatterns)
{

  string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/HierarchalAgglomerativeClusteringTester/between_day_cluster_same_four_patterns/";
  bool success = true;
  HierarchalAgglomerativeClustering* hierarchalAgglomerativeClustering = new HierarchalAgglomerativeClustering(tmp_result_path,"",TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG,success);
  int expectedNumberOfDiscoveredPatterns = 1;
  int expectedNumberOfSequencePatterns = 90;
  int expectedFinalPattern = 1;

  hierarchalAgglomerativeClustering->run();

  FeatureContainer* featureContainer = hierarchalAgglomerativeClustering->getClusteredPatterns();
  vector<int> sequence_patterns = featureContainer->getSequencePatterns();
  vector<int> discovered_patterns =featureContainer->getDiscoveredPatterns();

  ASSERT_EQ(expectedNumberOfDiscoveredPatterns,discovered_patterns.size());
  ASSERT_EQ(expectedNumberOfSequencePatterns,sequence_patterns.size());



  for(int i =0;i<sequence_patterns.size();i++)
    {
      ASSERT_EQ(expectedFinalPattern,sequence_patterns[i]);
    }

  hierarchalAgglomerativeClustering->destroy();
}


TEST(HierarchalAgglomerativeClustering, samePatternsInsideState)
{

  string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/HierarchalAgglomerativeClusteringTester/same_patterns_inside_state/";
  string home_setup_file = "../../../../components/aruba/tests/test_data/config_per_test/HierarchalAgglomerativeClusteringTester/same_patterns_inside_state/_6000/home_setup";
  string time_window_file = "../../../../components/aruba/tests/test_data/config_per_test/HierarchalAgglomerativeClusteringTester/same_patterns_inside_state/_6000/time-window-config";
  bool success = true;
  HierarchalAgglomerativeClustering* hierarchalAgglomerativeClustering = new HierarchalAgglomerativeClustering(tmp_result_path,"",home_setup_file,time_window_file,success);
  int expectedNumberOfDiscoveredPatterns = 1;
  int expectedNumberOfSequencePatterns = 90;
  int expectedFinalPattern = 1;

  hierarchalAgglomerativeClustering->run();

  FeatureContainer* featureContainer = hierarchalAgglomerativeClustering->getClusteredPatterns();
  vector<int> sequence_patterns = featureContainer->getSequencePatterns();
  vector<int> discovered_patterns =featureContainer->getDiscoveredPatterns();

  ASSERT_EQ(expectedNumberOfDiscoveredPatterns,discovered_patterns.size());
  ASSERT_EQ(expectedNumberOfSequencePatterns,sequence_patterns.size());



  for(int i =0;i<sequence_patterns.size();i++)
    {
      ASSERT_EQ(expectedFinalPattern,sequence_patterns[i]);
    }

  hierarchalAgglomerativeClustering->destroy();
}

TEST(HierarchalAgglomerativeClustering, diffPatternsInsidOutsideState)
{

  string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/HierarchalAgglomerativeClusteringTester/diff_patterns_inside_outside_state/";
  string home_setup_file = "../../../../components/aruba/tests/test_data/config_per_test/HierarchalAgglomerativeClusteringTester/diff_patterns_inside_outside_state/_6000/home_setup";
  string time_window_file = "../../../../components/aruba/tests/test_data/config_per_test/HierarchalAgglomerativeClusteringTester/diff_patterns_inside_outside_state/_6000/time-window-config";
  bool success = true;
  HierarchalAgglomerativeClustering* hierarchalAgglomerativeClustering = new HierarchalAgglomerativeClustering(tmp_result_path,"",home_setup_file,time_window_file,success);
  int expectedNumberOfDiscoveredPatterns = 2;
  int expectedNumberOfSequencePatterns = 90;
  int expectedSecondFinalPattern = 2;
  int expectedFirstFinalPattern = 1;

  hierarchalAgglomerativeClustering->run();

  FeatureContainer* featureContainer = hierarchalAgglomerativeClustering->getClusteredPatterns();
  vector<int> sequence_patterns = featureContainer->getSequencePatterns();
  vector<int> discovered_patterns =featureContainer->getDiscoveredPatterns();


  ASSERT_EQ(expectedNumberOfDiscoveredPatterns,discovered_patterns.size());
  ASSERT_EQ(expectedNumberOfSequencePatterns,sequence_patterns.size());


  for(int i =0;i<sequence_patterns.size();i++)
    {

      if(i < 15)
        ASSERT_EQ(expectedFirstFinalPattern,sequence_patterns[i]);
      else
        ASSERT_EQ(expectedSecondFinalPattern,sequence_patterns[i]);
    }


  hierarchalAgglomerativeClustering->destroy();
}


TEST(HierarchalAgglomerativeClustering, diffPatternsInsideState)
{

  string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/HierarchalAgglomerativeClusteringTester/diff_patterns_inside_state/";
  string home_setup_file = "../../../../components/aruba/tests/test_data/config_per_test/HierarchalAgglomerativeClusteringTester/diff_patterns_inside_state/_6000/home_setup";
  string time_window_file = "../../../../components/aruba/tests/test_data/config_per_test/HierarchalAgglomerativeClusteringTester/diff_patterns_inside_state/_6000/time-window-config";
  bool success = true;
  HierarchalAgglomerativeClustering* hierarchalAgglomerativeClustering = new HierarchalAgglomerativeClustering(tmp_result_path,"",home_setup_file,time_window_file,success);
  int expectedNumberOfDiscoveredPatterns = 2;
  int expectedNumberOfSequencePatterns = 90;
  int expectedSecondFinalPattern = 1;
  int expectedFirstFinalPattern = 6;

  hierarchalAgglomerativeClustering->run();

  FeatureContainer* featureContainer = hierarchalAgglomerativeClustering->getClusteredPatterns();
  vector<int> sequence_patterns = featureContainer->getSequencePatterns();
  vector<int> discovered_patterns =featureContainer->getDiscoveredPatterns();


  ASSERT_EQ(expectedNumberOfDiscoveredPatterns,discovered_patterns.size());
  ASSERT_EQ(expectedNumberOfSequencePatterns,sequence_patterns.size());


  for(int i =0;i<sequence_patterns.size();i++)
    {

      if(i < 15)
        ASSERT_EQ(expectedFirstFinalPattern,sequence_patterns[i]);
      else
        ASSERT_EQ(expectedSecondFinalPattern,sequence_patterns[i]);
    }


  hierarchalAgglomerativeClustering->destroy();
}

TEST(HierarchalAgglomerativeClustering, samePatternsInsideOutsideState)
{

  string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/HierarchalAgglomerativeClusteringTester/same_patterns_inside_outside_state/";
  string home_setup_file = "../../../../components/aruba/tests/test_data/config_per_test/HierarchalAgglomerativeClusteringTester/same_patterns_inside_outside_state/_6000/home_setup";
  string time_window_file = "../../../../components/aruba/tests/test_data/config_per_test/HierarchalAgglomerativeClusteringTester/same_patterns_inside_outside_state/_6000/time-window-config";
  bool success = true;
  HierarchalAgglomerativeClustering* hierarchalAgglomerativeClustering = new HierarchalAgglomerativeClustering(tmp_result_path,"",home_setup_file,time_window_file,success);
  int expectedNumberOfDiscoveredPatterns = 2;
  int expectedNumberOfSequencePatterns = 90;
  int expectedSecondFinalPattern = 1;
  int expectedFirstFinalPattern = 6;

  hierarchalAgglomerativeClustering->run();

  FeatureContainer* featureContainer = hierarchalAgglomerativeClustering->getClusteredPatterns();
  vector<int> sequence_patterns = featureContainer->getSequencePatterns();
  vector<int> discovered_patterns =featureContainer->getDiscoveredPatterns();


  ASSERT_EQ(expectedNumberOfDiscoveredPatterns,discovered_patterns.size());
  ASSERT_EQ(expectedNumberOfSequencePatterns,sequence_patterns.size());


  for(int i =0;i<sequence_patterns.size();i++)
    {

      if(i < 15)
        ASSERT_EQ(expectedFirstFinalPattern,sequence_patterns[i]);
      else
        ASSERT_EQ(expectedSecondFinalPattern,sequence_patterns[i]);
    }


  hierarchalAgglomerativeClustering->destroy();
}
