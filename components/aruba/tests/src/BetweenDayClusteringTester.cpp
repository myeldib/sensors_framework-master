#include<test-constants.h>
#include<between-day-clustering.h>
#include <gtest/gtest.h>


TEST(BetweenDayClustering, createBetweenDayClusteringObject)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/BetweenDayClusteringTester/two_days_three_patterns/";

    bool success=true;
    BetweenDayClustering* betweenDayClustering = new BetweenDayClustering(tmp_result_path,"","",TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG,success);

    ASSERT_TRUE(betweenDayClustering);

    betweenDayClustering->destroy();

}

TEST(BetweenDayClustering, clusterPatternsBetweenTwoDaysWithThreePatterns)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/BetweenDayClusteringTester/two_days_three_patterns/";

    bool success=true;
    BetweenDayClustering* betweenDayClustering = new BetweenDayClustering(tmp_result_path,"","",TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG,success);

    int expectedFirstMergedDiscoveredSequence = 10000;
    int expectedSecondMergedDiscoveredSequence = 10001;
    int expectedThirdMergedDiscoveredSequence = 10002;
    int expectedNumberOfSequencePatternsLength = 75;
    string expectedWorkActivity = "Work";
    string expectedSleepActivity = "Sleeping";
    string expectedBedToToiletActivity = "Bed_to_Toilet";
    string expectedOtherLabel ="-";

    ASSERT_TRUE(betweenDayClustering);

    betweenDayClustering->run();

    FeatureContainer* merged_fc = betweenDayClustering->getMergedFeatureContainerPatterns();

    vector<int> discovered_patterns = merged_fc->getDiscoveredPatterns();
    vector<int> sequence_patterns = merged_fc->getSequencePatterns();
    vector<float> time_index_per_pattern= merged_fc->getTimeIndexPerPattern();
    vector<string> day_name_per_pattern = merged_fc->getDayNamePerPattern();
    vector<string> activity_label=merged_fc->getActivityLabel();

    ASSERT_EQ(expectedFirstMergedDiscoveredSequence,discovered_patterns[0]);
    ASSERT_EQ(expectedSecondMergedDiscoveredSequence,discovered_patterns[1]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,discovered_patterns[2]);


    //check sequence patterns
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,sequence_patterns.size());
    ASSERT_EQ(expectedFirstMergedDiscoveredSequence,sequence_patterns[0]);
    ASSERT_EQ(expectedSecondMergedDiscoveredSequence,sequence_patterns[15]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,sequence_patterns[30]);
    ASSERT_EQ(expectedSecondMergedDiscoveredSequence,sequence_patterns[45]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,sequence_patterns[60]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,sequence_patterns[74]);

    //check activity labels
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,activity_label.size());
    ASSERT_EQ(expectedWorkActivity,activity_label[0]);
    ASSERT_EQ(expectedOtherLabel,activity_label[15]);
    ASSERT_EQ(expectedSleepActivity,activity_label[29]);
    ASSERT_EQ(expectedOtherLabel,activity_label[45]);
    ASSERT_EQ(expectedBedToToiletActivity,activity_label[60]);
    ASSERT_EQ(expectedBedToToiletActivity,activity_label[74]);

    //check time index
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,time_index_per_pattern.size());

    //check day name
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,day_name_per_pattern.size());


    betweenDayClustering->destroy();

}

TEST(BetweenDayClustering, clusterPatternsBetweenThreeDaysWithFourPatterns)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/BetweenDayClusteringTester/three_days_four_patterns/";

    bool success= true;
    BetweenDayClustering* betweenDayClustering = new BetweenDayClustering(tmp_result_path,"","",TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG,success);
    int expectedFirstMergedDiscoveredSequence = 20000;
    int expectedSecondMergedDiscoveredSequence = 20001;
    int expectedThirdMergedDiscoveredSequence = 20002;
    int expectedForthMergedDiscoveredSequence = 20003;
    int expectedFivethMergedDiscoveredSequence = 20004;
    int expectedNumberOfSequencePatternsLength = 105;
    int expectedNumberOfDiscoveredPatternsLength = 5;
    int expectedNumberOfActiveSensors = 1;
    float expectedSensorDuration = 60000;
    string expectedWorkActivity = "Work";
    string expectedSleepActivity = "Sleeping";
    string expectedBedToToiletActivity = "Bed_to_Toilet";
    string expectedEatActivity = "Eating";
    string expectedMealActivity = "Meal_Preparation";
    string expectedOtherLabel ="-";

    ASSERT_TRUE(betweenDayClustering);

    betweenDayClustering->run();

    FeatureContainer* merged_fc = betweenDayClustering->getMergedFeatureContainerPatterns();

    vector<int> discovered_patterns = merged_fc->getDiscoveredPatterns();
    vector<int> sequence_patterns = merged_fc->getSequencePatterns();
    vector<float> time_index_per_pattern= merged_fc->getTimeIndexPerPattern();
    vector<string> day_name_per_pattern = merged_fc->getDayNamePerPattern();
    vector<string> activity_label=merged_fc->getActivityLabel();
    vector<vector<int> > active_sensors_per_pattern = merged_fc->getActiveSensorsPerPattern();
    vector<vector<float> > avg_sensor_durations_per_pattern = merged_fc->getAverageSensorDurationsPerPattern();

    ASSERT_EQ(expectedNumberOfDiscoveredPatternsLength,discovered_patterns.size());
    ASSERT_EQ(expectedFirstMergedDiscoveredSequence,discovered_patterns[0]);
    ASSERT_EQ(expectedSecondMergedDiscoveredSequence,discovered_patterns[1]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,discovered_patterns[2]);
    ASSERT_EQ(expectedForthMergedDiscoveredSequence,discovered_patterns[3]);
    ASSERT_EQ(expectedFivethMergedDiscoveredSequence,discovered_patterns[4]);


    //check sequence patterns
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,sequence_patterns.size());
    ASSERT_EQ(expectedFirstMergedDiscoveredSequence,sequence_patterns[0]);
    ASSERT_EQ(expectedSecondMergedDiscoveredSequence,sequence_patterns[15]);
    ASSERT_EQ(expectedSecondMergedDiscoveredSequence,sequence_patterns[30]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,sequence_patterns[45]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,sequence_patterns[60]);
    ASSERT_EQ(expectedForthMergedDiscoveredSequence,sequence_patterns[89]);
    ASSERT_EQ(expectedFivethMergedDiscoveredSequence,sequence_patterns[104]);

    //check activity labels
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,activity_label.size());
    ASSERT_EQ(expectedWorkActivity,activity_label[0]);
    ASSERT_EQ(expectedOtherLabel,activity_label[15]);
    ASSERT_EQ(expectedSleepActivity,activity_label[29]);
    ASSERT_EQ(expectedBedToToiletActivity,activity_label[45]);
    ASSERT_EQ(expectedBedToToiletActivity,activity_label[60]);
    ASSERT_EQ(expectedBedToToiletActivity,activity_label[74]);
    ASSERT_EQ(expectedEatActivity,activity_label[89]);
    ASSERT_EQ(expectedMealActivity,activity_label[104]);

    //check time index
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,time_index_per_pattern.size());

    //check day name
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,day_name_per_pattern.size());

    //check active sensors
    for(int i = 0; i<active_sensors_per_pattern.size() ;i++)
    {
        vector<int> active_sensors = active_sensors_per_pattern[i];
        vector<float> avg_sensor_duration = avg_sensor_durations_per_pattern[i];

        ASSERT_EQ(expectedNumberOfActiveSensors, active_sensors.size());

        ASSERT_DOUBLE_EQ(expectedSensorDuration,avg_sensor_duration[active_sensors[0]]);
    }


    betweenDayClustering->destroy();

}


TEST(BetweenDayClustering, samePatternsOutsideState)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/BetweenDayClusteringTester/same_patterns_outside_state/";
    string home_setup_file = "../../../../components/aruba/tests/test_data/config_per_test/BetweenDayClusteringTester/same_patterns_outside_state/_5_10/2010-11-04/home_setup";
    string time_window_config_file = "../../../../components/aruba/tests/test_data/config_per_test/BetweenDayClusteringTester/same_patterns_outside_state/_5_10/2010-11-04/time-window-config";

    bool success=true;
    BetweenDayClustering* betweenDayClustering = new BetweenDayClustering(tmp_result_path,"","",home_setup_file,time_window_config_file,success);

    int expectedFirstMergedDiscoveredSequence = 10000;
    int expectedSecondMergedDiscoveredSequence = 10001;
    int expectedThirdMergedDiscoveredSequence = 10002;
    int expectedNumberOfSequencePatternsLength = 75;
    string expectedWorkActivity = "Work";
    string expectedSleepActivity = "Sleeping";
    string expectedBedToToiletActivity = "Bed_to_Toilet";
    string expectedOtherLabel ="-";

    ASSERT_TRUE(betweenDayClustering);

    betweenDayClustering->run();

    FeatureContainer* merged_fc = betweenDayClustering->getMergedFeatureContainerPatterns();

    vector<int> discovered_patterns = merged_fc->getDiscoveredPatterns();
    vector<int> sequence_patterns = merged_fc->getSequencePatterns();
    vector<float> time_index_per_pattern= merged_fc->getTimeIndexPerPattern();
    vector<string> day_name_per_pattern = merged_fc->getDayNamePerPattern();
    vector<string> activity_label=merged_fc->getActivityLabel();

    ASSERT_EQ(expectedFirstMergedDiscoveredSequence,discovered_patterns[0]);
    ASSERT_EQ(expectedSecondMergedDiscoveredSequence,discovered_patterns[1]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,discovered_patterns[2]);


    //check sequence patterns
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,sequence_patterns.size());
    ASSERT_EQ(expectedFirstMergedDiscoveredSequence,sequence_patterns[0]);
    ASSERT_EQ(expectedSecondMergedDiscoveredSequence,sequence_patterns[15]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,sequence_patterns[30]);
    ASSERT_EQ(expectedSecondMergedDiscoveredSequence,sequence_patterns[45]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,sequence_patterns[60]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,sequence_patterns[74]);

    //check activity labels
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,activity_label.size());
    ASSERT_EQ(expectedWorkActivity,activity_label[0]);
    ASSERT_EQ(expectedOtherLabel,activity_label[15]);
    ASSERT_EQ(expectedSleepActivity,activity_label[29]);
    ASSERT_EQ(expectedOtherLabel,activity_label[45]);
    ASSERT_EQ(expectedBedToToiletActivity,activity_label[60]);
    ASSERT_EQ(expectedBedToToiletActivity,activity_label[74]);

    //check time index
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,time_index_per_pattern.size());

    //check day name
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,day_name_per_pattern.size());


    betweenDayClustering->destroy();

}

TEST(BetweenDayClustering, samePatternsDefaultState)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/BetweenDayClusteringTester/same_patterns_default_state/";
    string home_setup_file = "../../../../components/aruba/tests/test_data/config_per_test/BetweenDayClusteringTester/same_patterns_default_state/_5_10/2010-11-04/home_setup";
    string time_window_config_file = "../../../../components/aruba/tests/test_data/config_per_test/BetweenDayClusteringTester/same_patterns_default_state/_5_10/2010-11-04/time-window-config";

    bool success=true;
    BetweenDayClustering* betweenDayClustering = new BetweenDayClustering(tmp_result_path,"","",home_setup_file,time_window_config_file,success);

    int expectedFirstMergedDiscoveredSequence = 10000;
    int expectedSecondMergedDiscoveredSequence = 10001;
    int expectedThirdMergedDiscoveredSequence = 10002;
    int expectedNumberOfSequencePatternsLength = 75;
    string expectedWorkActivity = "Work";
    string expectedSleepActivity = "Sleeping";
    string expectedBedToToiletActivity = "Bed_to_Toilet";
    string expectedOtherLabel ="-";

    ASSERT_TRUE(betweenDayClustering);

    betweenDayClustering->run();

    FeatureContainer* merged_fc = betweenDayClustering->getMergedFeatureContainerPatterns();

    vector<int> discovered_patterns = merged_fc->getDiscoveredPatterns();
    vector<int> sequence_patterns = merged_fc->getSequencePatterns();
    vector<float> time_index_per_pattern= merged_fc->getTimeIndexPerPattern();
    vector<string> day_name_per_pattern = merged_fc->getDayNamePerPattern();
    vector<string> activity_label=merged_fc->getActivityLabel();

    ASSERT_EQ(expectedFirstMergedDiscoveredSequence,discovered_patterns[0]);
    ASSERT_EQ(expectedSecondMergedDiscoveredSequence,discovered_patterns[1]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,discovered_patterns[2]);


    //check sequence patterns
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,sequence_patterns.size());
    ASSERT_EQ(expectedFirstMergedDiscoveredSequence,sequence_patterns[0]);
    ASSERT_EQ(expectedSecondMergedDiscoveredSequence,sequence_patterns[15]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,sequence_patterns[30]);
    ASSERT_EQ(expectedSecondMergedDiscoveredSequence,sequence_patterns[45]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,sequence_patterns[60]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,sequence_patterns[74]);

    //check activity labels
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,activity_label.size());
    ASSERT_EQ(expectedWorkActivity,activity_label[0]);
    ASSERT_EQ(expectedOtherLabel,activity_label[15]);
    ASSERT_EQ(expectedSleepActivity,activity_label[29]);
    ASSERT_EQ(expectedOtherLabel,activity_label[45]);
    ASSERT_EQ(expectedBedToToiletActivity,activity_label[60]);
    ASSERT_EQ(expectedBedToToiletActivity,activity_label[74]);

    //check time index
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,time_index_per_pattern.size());

    //check day name
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,day_name_per_pattern.size());


    betweenDayClustering->destroy();

}

TEST(BetweenDayClustering, samePatternsDefaultOutsideState)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/BetweenDayClusteringTester/same_patterns_default_outside_state/";
    string home_setup_file = "../../../../components/aruba/tests/test_data/config_per_test/BetweenDayClusteringTester/same_patterns_default_outside_state/_5_10/2010-11-04/home_setup";
    string time_window_config_file = "../../../../components/aruba/tests/test_data/config_per_test/BetweenDayClusteringTester/same_patterns_default_outside_state/_5_10/2010-11-04/time-window-config";

    bool success=true;
    BetweenDayClustering* betweenDayClustering = new BetweenDayClustering(tmp_result_path,"","",home_setup_file,time_window_config_file,success);

    int expectedFirstMergedDiscoveredSequence = 10000;
    int expectedSecondMergedDiscoveredSequence = 10001;
    int expectedThirdMergedDiscoveredSequence = 10002;
    int expectedFouthMergedDiscoveredSequence = 10003;
    int expectedNumberOfSequencePatternsLength = 75;
    string expectedWorkActivity = "Work";
    string expectedSleepActivity = "Sleeping";
    string expectedBedToToiletActivity = "Bed_to_Toilet";
    string expectedOtherLabel ="-";

    ASSERT_TRUE(betweenDayClustering);

    betweenDayClustering->run();

    FeatureContainer* merged_fc = betweenDayClustering->getMergedFeatureContainerPatterns();

    vector<int> discovered_patterns = merged_fc->getDiscoveredPatterns();
    vector<int> sequence_patterns = merged_fc->getSequencePatterns();
    vector<float> time_index_per_pattern= merged_fc->getTimeIndexPerPattern();
    vector<string> day_name_per_pattern = merged_fc->getDayNamePerPattern();
    vector<string> activity_label=merged_fc->getActivityLabel();


    ASSERT_EQ(expectedFirstMergedDiscoveredSequence,discovered_patterns[0]);
    ASSERT_EQ(expectedSecondMergedDiscoveredSequence,discovered_patterns[1]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,discovered_patterns[2]);
    ASSERT_EQ(expectedFouthMergedDiscoveredSequence,discovered_patterns[3]);


    //check sequence patterns
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,sequence_patterns.size());
    ASSERT_EQ(expectedFirstMergedDiscoveredSequence,sequence_patterns[0]);
    ASSERT_EQ(expectedSecondMergedDiscoveredSequence,sequence_patterns[15]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,sequence_patterns[30]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,sequence_patterns[45]);
    ASSERT_EQ(expectedFouthMergedDiscoveredSequence,sequence_patterns[60]);
    ASSERT_EQ(expectedFouthMergedDiscoveredSequence,sequence_patterns[74]);


    //check activity labels
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,activity_label.size());
    ASSERT_EQ(expectedOtherLabel,activity_label[0]);
    ASSERT_EQ(expectedWorkActivity,activity_label[15]);
    ASSERT_EQ(expectedWorkActivity,activity_label[29]);
    ASSERT_EQ(expectedBedToToiletActivity,activity_label[45]);
    ASSERT_EQ(expectedOtherLabel,activity_label[60]);
    ASSERT_EQ(expectedSleepActivity,activity_label[74]);

    //check time index
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,time_index_per_pattern.size());

    //check day name
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,day_name_per_pattern.size());


    betweenDayClustering->destroy();

}

TEST(BetweenDayClustering, diffPatternsDefaultOutsideState)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/BetweenDayClusteringTester/diff_patterns_default_outside_state/";
    string home_setup_file = "../../../../components/aruba/tests/test_data/config_per_test/BetweenDayClusteringTester/diff_patterns_default_outside_state/_5_10/2010-11-04/home_setup";
    string time_window_config_file = "../../../../components/aruba/tests/test_data/config_per_test/BetweenDayClusteringTester/diff_patterns_default_outside_state/_5_10/2010-11-04/time-window-config";

    bool success=true;
    BetweenDayClustering* betweenDayClustering = new BetweenDayClustering(tmp_result_path,"","",home_setup_file,time_window_config_file,success);

    int expectedFirstMergedDiscoveredSequence = 10000;
    int expectedSecondMergedDiscoveredSequence = 10001;
    int expectedThirdMergedDiscoveredSequence = 10002;
    int expectedFouthMergedDiscoveredSequence = 10003;

    int expectedNumberOfSequencePatternsLength = 75;
    string expectedWorkActivity = "Work";
    string expectedSleepActivity = "Sleeping";
    string expectedBedToToiletActivity = "Bed_to_Toilet";
    string expectedOtherLabel ="-";

    ASSERT_TRUE(betweenDayClustering);

    betweenDayClustering->run();

    FeatureContainer* merged_fc = betweenDayClustering->getMergedFeatureContainerPatterns();

    vector<int> discovered_patterns = merged_fc->getDiscoveredPatterns();
    vector<int> sequence_patterns = merged_fc->getSequencePatterns();
    vector<float> time_index_per_pattern= merged_fc->getTimeIndexPerPattern();
    vector<string> day_name_per_pattern = merged_fc->getDayNamePerPattern();
    vector<string> activity_label=merged_fc->getActivityLabel();

    ASSERT_EQ(expectedFirstMergedDiscoveredSequence,discovered_patterns[0]);
    ASSERT_EQ(expectedSecondMergedDiscoveredSequence,discovered_patterns[1]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,discovered_patterns[2]);
    ASSERT_EQ(expectedFouthMergedDiscoveredSequence,discovered_patterns[3]);


    //check sequence patterns
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,sequence_patterns.size());
    ASSERT_EQ(expectedFirstMergedDiscoveredSequence,sequence_patterns[0]);
    ASSERT_EQ(expectedSecondMergedDiscoveredSequence,sequence_patterns[15]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,sequence_patterns[30]);
    ASSERT_EQ(expectedThirdMergedDiscoveredSequence,sequence_patterns[45]);
    ASSERT_EQ(expectedFouthMergedDiscoveredSequence,sequence_patterns[60]);
    ASSERT_EQ(expectedFouthMergedDiscoveredSequence,sequence_patterns[74]);


    //check activity labels
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,activity_label.size());
    ASSERT_EQ(expectedOtherLabel,activity_label[0]);
    ASSERT_EQ(expectedWorkActivity,activity_label[15]);
    ASSERT_EQ(expectedWorkActivity,activity_label[29]);
    ASSERT_EQ(expectedBedToToiletActivity,activity_label[45]);
    ASSERT_EQ(expectedOtherLabel,activity_label[60]);
    ASSERT_EQ(expectedSleepActivity,activity_label[74]);

    //check time index
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,time_index_per_pattern.size());

    //check day name
    ASSERT_EQ(expectedNumberOfSequencePatternsLength,day_name_per_pattern.size());


    betweenDayClustering->destroy();

}

