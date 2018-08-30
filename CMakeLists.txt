CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)
#------------------------------------------------------------------------
# project name
#------------------------------------------------------------------------
PROJECT(sensors_workflow)


#------------------------------------------------------------------------
# build defenitions (on top for fast access)
#------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#------------------------------------------------------------------------
# library dependencies
#------------------------------------------------------------------------
## cmake module path (make sure to pull the cmake subrepository)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_VERBOSE_MAKEFILE TRUE)


###Boost
find_package( Boost REQUIRED COMPONENTS date_time
                                        filesystem
                                        iostreams
                                        system
                                        thread
                                        timer


)

### Python
find_package(PythonLibs 2.7 REQUIRED)
### NumPy
find_package(NumPy REQUIRED)


### OpenCV
find_package( OpenCV REQUIRED )


##------------------------------------------------------------------------
## include & link directories
##------------------------------------------------------------------------

## from library dependencies
include_directories(
                    ${OpenCV_INCLUDE_DIR}
                    ${Boost_LIBRARY_DIRS}
                    ${PYTHON_INCLUDE_DIRS}
                    ${NUMPY_INCLUDE_DIRS}


)

link_directories( ${OpenCV_LIBRARY_DIRS}
                   ${Boost_LIBRARIES}

)


add_subdirectory(${PROJECT_SOURCE_DIR}/components/aruba/tests)
## from this project
include_directories(
    "${PROJECT_SOURCE_DIR}/components/aruba/impl/include"
    "${PROJECT_SOURCE_DIR}/components/aruba/impl/src"
    "${PROJECT_SOURCE_DIR}/components/aruba/deprecated/include"
    "${PROJECT_SOURCE_DIR}/components/aruba/deprecated/src"
)


INCLUDE_DIRECTORIES(${tests_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${tests_SOURCE_DIRS})
#------------------------------------------------------------------------
# build
#------------------------------------------------------------------------
SET(HEADERS
    components/aruba/impl/include/sensor.h
    components/aruba/impl/include/preprocessing-stage.h
    components/aruba/impl/include/utilities.h
    components/aruba/impl/include/home.h
    components/aruba/impl/include/constants.h
    components/aruba/impl/include/file-separator.h
    components/aruba/impl/include/time-window-approach.h
    components/aruba/impl/include/heatmap.h
    components/aruba/impl/include/hist-cluster.h
    components/aruba/impl/include/feature-container.h
    components/aruba/impl/include/feature-reader.h
    components/aruba/impl/include/within-day-clustering.h
    components/aruba/impl/include/between-day-clustering.h
    components/aruba/impl/include/feature-processor.h
    components/aruba/impl/include/similarity-measure.h
    components/aruba/impl/include/hierarchal-agglomerative-clustering.h
    components/aruba/impl/include/logger.h
    components/aruba/impl/include/feature-writer.h
    components/aruba/impl/include/patterns-statistics.h
    components/aruba/impl/include/cluster-recognition.h
    components/aruba/impl/include/evaluation.h
    components/aruba/impl/include/model-recognition.h
    components/aruba/impl/include/python-runner.h
    components/aruba/impl/include/sensor-event-processor.h
    components/aruba/impl/include/sorter-processor.h


    components/aruba/deprecated/include/SensorReader.h
    components/aruba/deprecated/include/GenerateMeasures.h
    components/aruba/deprecated/include/Sensor.h
    components/aruba/deprecated/include/Home.h
    components/aruba/deprecated/include/Common.h
    components/aruba/deprecated/include/HeatMap.h
    components/aruba/deprecated/include/SensorFeature.h
    components/aruba/deprecated/include/ActivityDiscovery.h
    components/aruba/deprecated/include/DayContainer.h
    components/aruba/deprecated/include/EMD.h
    components/aruba/deprecated/include/InferentialDiscovery.h
    components/aruba/deprecated/include/FeatureContainer.h
    components/aruba/deprecated/include/ClusterPatterns.h
    components/aruba/deprecated/include/ConfusionMatrix.h
    components/aruba/deprecated/include/Recognition.h

)


SET(SOURCES components/aruba/impl/src/main.cpp
            components/aruba/impl/src/preprocessing-stage.cpp
            components/aruba/impl/src/utilities.cpp
            components/aruba/impl/src/home.cpp
            components/aruba/impl/src/file-separator.cpp
            components/aruba/impl/src/time-window-approach.cpp
            components/aruba/impl/src/heatmap.cpp
            components/aruba/impl/src/hist-cluster.cpp
            components/aruba/impl/src/feature-container.cpp
            components/aruba/impl/src/feature-reader.cpp
            components/aruba/impl/src/within-day-clustering.cpp
            components/aruba/impl/src/between-day-clustering.cpp
            components/aruba/impl/src/feature-processor.cpp
            components/aruba/impl/src/similarity-measure.cpp
            components/aruba/impl/src/hierarchal-agglomerative-clustering.cpp
            components/aruba/impl/src/feature-writer.cpp
            components/aruba/impl/src/patterns-statistics.cpp
            components/aruba/impl/src/cluster-recognition.cpp
            components/aruba/impl/src/model-recognition.cpp
            components/aruba/impl/src/python-runner.cpp
            components/aruba/impl/src/evaluation.cpp
            components/aruba/impl/src/sensor-event-processor.cpp
            components/aruba/impl/src/sorter-processor.cpp


            components/aruba/deprecated/src/SensorReader.cpp
            components/aruba/deprecated/src/GenerateMeasures.cpp
            components/aruba/deprecated/src/Home.cpp
            components/aruba/deprecated/src/Common.cpp
            components/aruba/deprecated/src/HeatMap.cpp
            components/aruba/deprecated/src/SensorFeature.cpp
            components/aruba/deprecated/src/ActivityDiscovery.cpp
            components/aruba/deprecated/src/EMD.cpp
            components/aruba/deprecated/src/InferentialDiscovery.cpp
            components/aruba/deprecated/src/ClusterPatterns.cpp
            components/aruba/deprecated/src/ConfusionMatrix.cpp
            components/aruba/deprecated/src/Recognition.cpp
            components/aruba/deprecated/src/main.cpp
)

add_library(mainComponent ${SOURCES} ${HEADERS})

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES} ${HEADERS})


TARGET_LINK_LIBRARIES(  ${PROJECT_NAME}
                        ${tests_LIBRARIES}
                        ${Boost_FILESYSTEM_LIBRARIES}
                        ${Boost_SYSTEM_LIBRARIES}
                        ${Boost_LIBRARIES}
                        ${OpenCV_LIBS}
                        ${PYTHON_LIBRARIES}
)

