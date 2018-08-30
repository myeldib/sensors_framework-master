cmake_minimum_required(VERSION 2.6)

set(MODULE_NAME "tests")


##GTest
find_package(GTest REQUIRED)


# from library dependencies
include_directories(
                    ${GTEST_INCLUDE_DIRS}
)

set(main_component_INCLUDE_DIR
    "${PROJECT_SOURCE_DIR}/components/aruba/impl/include"
    )

set(main_component_SOURCE_DIR
   "${PROJECT_SOURCE_DIR}/components/aruba/impl/src"
    )



include_directories(
     "${CMAKE_CURRENT_SOURCE_DIR}/src"
     "${CMAKE_CURRENT_SOURCE_DIR}/inc"
      ${main_component_INCLUDE_DIR}
      ${main_component_SOURCE_DIR}

)

SET(${MODULE_NAME}_SOURCES
    src/main.cpp
    src/HomeTester.cpp
    src/PreprocessingStageTester.cpp
    src/FileSeparatorTester.cpp
    src/TimeWindowApproachTester.cpp
    src/HistClusterTester.cpp
    src/FeatureReaderTester.cpp
    src/FeatureProcessorTester.cpp
    src/SimilarityMeasureTester.cpp
    src/BetweenDayClusteringTester.cpp
    src/HierarchalAgglomerativeClusteringTester.cpp
    src/SensorEventProcessorTester.cpp
    src/SorterProcessorTester.cpp

)

SET(${MODULE_NAME}_HEADERS
    inc/test-constants.h
)


add_library(${MODULE_NAME}  ${${MODULE_NAME}_SOURCES} ${${MODULE_NAME}_HEADERS})


# Link runTests with what we want to test and the GTest and pthread library
add_executable(runTests src/main.cpp)

target_link_libraries(runTests
                        gtest
                        pthread
                        mainComponent
                        ${GTEST_LIBRARIES}
                        ${Boost_LIBRARIES}
                        ${OpenCV_LIBS}
                        )

