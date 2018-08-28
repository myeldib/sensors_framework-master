#include "home.h"
#include "test-constants.h"
#include <gtest/gtest.h>


TEST(Home, createHomeObject) {
  Home* home = new Home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

   ASSERT_TRUE(home != NULL);

   delete home;

}

TEST(Home, shortDurationLambda) {
  Home home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

  home.readHomeSetup();
  ASSERT_EQ(5,home.getShortDurationLambda());

}


TEST(Home, longDurationLambda) {
  Home home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

  home.readHomeSetup();
  ASSERT_EQ(10,home.getLongDurationLambda());

}


TEST(Home, numberOfLocations) {
  Home home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

  home.readHomeSetup();
  ASSERT_EQ(9,home.getNumerOfLocations());

}


TEST(Home, sensorTimeInterval) {
  Home home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

  home.readHomeSetup();
  ASSERT_EQ(60,home.getSensorTimeInterval());

}

TEST(Home, locationTimeInterval) {
  Home home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

  home.readHomeSetup();
  ASSERT_EQ(900,home.getLocationTimeInterval());

}

TEST(Home, timeConstraint) {
  Home home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

  home.readHomeSetup();
  ASSERT_EQ(3600,home.getTimeConstraint());

}

TEST(Home, histogramSimilarityMeasure) {
  Home home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

  home.readHomeSetup();
  ASSERT_EQ(0.50,home.getHistogramSimilarityThreshold());

}

TEST(Home, EmdSimilarityMeasure) {
  Home home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

  home.readHomeSetup();
  ASSERT_EQ(6000,home.getEmdSimilarityThreshold());

}

TEST(Home, HieracleSimilarityMeasure) {
  Home home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

  home.readHomeSetup();
  ASSERT_FLOAT_EQ(0.90,home.getHierarchalClusteringThreshold());

}

TEST(Home, IncludeOtherActivityClassFlag) {
  Home home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

  home.readHomeSetup();
  ASSERT_FLOAT_EQ(0,home.getIncludeOtherActivityClass());

}

TEST(Home, checkSensorInformation) {
  Home home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

  home.readHomeSetup();

  ASSERT_EQ(35,home.getHomeSensorsId().size());
  ASSERT_EQ(35,home.getHomeSensorsPosition().size());
  ASSERT_EQ(35,home.getHomeSesnorLocation().size());
  ASSERT_EQ(35,home.getHomeSensorsType().size());


}

TEST(Home, checkActivityLabelInformation) {
  Home home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);
  int numOfExpectedActivityLabels = 12;
  int expectedFirstActivityIndex = 0;
  int expectedSixthActivityIndex = 11;
  string expectedFirstActivityValue ="Meal_Preparation";
  string expectedSixthActivityValue ="";

  home.readHomeSetup();

  ASSERT_EQ(numOfExpectedActivityLabels,home.getActivityLabelIntStringMap().size());
  ASSERT_EQ(numOfExpectedActivityLabels,home.getActivityLabelStringIntMap().size());

  ASSERT_EQ(expectedFirstActivityIndex,home.getActivityLabelStringIntMap().at("Meal_Preparation"));
  ASSERT_EQ(expectedSixthActivityIndex,home.getActivityLabelStringIntMap().at(""));

  ASSERT_EQ(expectedFirstActivityValue,home.getActivityLabelIntStringMap().at(0));
  ASSERT_EQ(expectedSixthActivityValue,home.getActivityLabelIntStringMap().at(11));
}


TEST(Home, checkPatternStates) {
  string home_setup_file = "../../../../components/aruba/tests/test_data/config_per_test/HomeTester/pattern_states/home_setup";
  string time_window_config_file = "../../../../components/aruba/tests/test_data/config_per_test/HomeTester/pattern_states/time-window-config";

  Home home(home_setup_file,time_window_config_file);

  home.readHomeSetup();

  ASSERT_EQ(38,home.getHomeSensorsId().size());
  ASSERT_EQ(38,home.getHomeSensorsPosition().size());
  ASSERT_EQ(38,home.getHomeSesnorLocation().size());
  ASSERT_EQ(38,home.getHomeSensorsType().size());

  ASSERT_EQ("S001",home.getHomeSensorsId()[35]);
  ASSERT_EQ("S002",home.getHomeSensorsId()[36]);
  ASSERT_EQ("S003",home.getHomeSensorsId()[37]);


  ASSERT_EQ("inside",home.getHomeSesnorLocation()[35]);
  ASSERT_EQ("outside",home.getHomeSesnorLocation()[36]);
  ASSERT_EQ("default",home.getHomeSesnorLocation()[37]);

  ASSERT_EQ("pattern_state",home.getHomeSensorsType()[35]);
  ASSERT_EQ("pattern_state",home.getHomeSensorsType()[36]);
  ASSERT_EQ("pattern_state",home.getHomeSensorsType()[37]);



}
