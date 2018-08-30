//#include "HomeTester.cpp"
//#include "PreprocessingStageTester.cpp"
//#include "FileSeparatorTester.cpp"
//#include "TimeWindowApproachTester.cpp"
//#include "HistClusterTester.cpp"
//#include "FeatureReaderTester.cpp"
//#include "FeatureProcessorTester.cpp"
//#include "SimilarityMeasureTester.cpp"
//#include "BetweenDayClusteringTester.cpp"
//#include "HierarchalAgglomerativeClusteringTester.cpp"
//#include "SensorEventProcessorTester.cpp"
#include "SorterProcessorTester.cpp"
#include <gtest/gtest.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
