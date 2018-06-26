#include<time-window-approach.h>
#include<file-separator.h>
#include<home.h>
#include<test-constants.h>
#include <gtest/gtest.h>


TimeWindow* createSUT(string& folder_path)
{

    bool isFolderCreated = Common::createFolder(folder_path);

    if(!isFolderCreated)
    {
        return NULL;
    }

    Home* home = new Home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);


    if(!home)
    {
        return NULL;
    }

    FileSeparator* fs = new FileSeparator(TestConstants::PROCESSED_SENSOR_DATA_OUTPUT_FILE,folder_path);

    if(!fs)
    {
        return NULL;
    }


    fs->writeDailySensorData();

    TimeWindow* timeWindow = new TimeWindow(home,folder_path);

    if(!timeWindow)
    {
        return NULL;
    }

    delete fs;

    return timeWindow;

}

TEST(TimeWindow, creatTimeWindowObject)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/TimeWindowApproachTester/result/";
    TimeWindow* tw = createSUT(tmp_result_path);

    ASSERT_TRUE(tw != NULL);

    delete tw;

    bool isFolderDeleted = Common::deleteFolder(tmp_result_path);

    ASSERT_TRUE(isFolderDeleted);

}

TEST(TimeWindow, checkSensorHistogramFiles)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/TimeWindowApproachTester/result/";
    TimeWindow* tw = createSUT(tmp_result_path);
    int expectedNumberOfFiles = 3;
    int expectedNumOfWindows = 1440;
    int expectedNumOfSensorsInWindow = 35;
    int expectedDurationPerWindow = 60000;

    ASSERT_TRUE(tw != NULL);

    tw->run();

    vector<string> sensor_hist_files=Common::getFolderFileNames(tmp_result_path,"sensors_duration");

    ASSERT_EQ(expectedNumberOfFiles,sensor_hist_files.size());

    for(int i=0;i<sensor_hist_files.size();i++)
    {

       vector<vector<string> > fileContents = Common::readFile(sensor_hist_files[i],',');

       ASSERT_EQ(expectedNumOfWindows,fileContents.size());

       ASSERT_EQ(expectedNumOfSensorsInWindow,fileContents[0].size());

       for(int j=0;j<fileContents.size();j++)
       {
           vector<string> lineContent = fileContents[j];


           int durationPerWindow  = 0;
           for(int l = 0; l <lineContent.size();l++)
           {
               durationPerWindow += atoi(lineContent[l].c_str());
           }

           ASSERT_EQ(expectedDurationPerWindow,durationPerWindow);
       }

    }

    delete tw;

    bool isFolderDeleted = Common::deleteFolder(tmp_result_path);

    ASSERT_TRUE(isFolderDeleted);

}

TEST(TimeWindow, checkLocationHistogramFiles)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/TimeWindowApproachTester/result/";
    TimeWindow* tw = createSUT(tmp_result_path);
    int expectedNumberOfFiles = 3;
    int expectedNumOfWindows = 96;
    int expectedNumOfSensorsInWindow = 9;
    int expectedDurationPerWindow = 900000;

    ASSERT_TRUE(tw != NULL);

    tw->run();

    vector<string> sensor_hist_files=Common::getFolderFileNames(tmp_result_path,"locations_duration");

    ASSERT_EQ(expectedNumberOfFiles,sensor_hist_files.size());

    for(int i=0;i<sensor_hist_files.size();i++)
    {

       vector<vector<string> > fileContents = Common::readFile(sensor_hist_files[i],',');

       ASSERT_EQ(expectedNumOfWindows,fileContents.size());

       ASSERT_EQ(expectedNumOfSensorsInWindow,fileContents[0].size());

       for(int j=0;j<fileContents.size();j++)
       {
           vector<string> lineContent = fileContents[j];


           int durationPerWindow  = 0;
           for(int l = 0; l <lineContent.size();l++)
           {
               durationPerWindow += atoi(lineContent[l].c_str());
           }

           ASSERT_EQ(expectedDurationPerWindow,durationPerWindow);
       }

    }

    delete tw;

    bool isFolderDeleted = Common::deleteFolder(tmp_result_path);

    ASSERT_TRUE(isFolderDeleted);

}

TEST(TimeWindow, checkActivityPerWindowFromFiles)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/TimeWindowApproachTester/result/";
    TimeWindow* tw = createSUT(tmp_result_path);
    int expectedNumberOfFiles = 3;
    int expectedNumOfWindows = 1440;
    string expectedFivethWindowActivity = "Sleeping";


    ASSERT_TRUE(tw != NULL);

    tw->run();

    vector<string> activity_per_window_files=Common::getFolderFileNames(tmp_result_path,"activity_per_window");

    ASSERT_EQ(expectedNumberOfFiles,activity_per_window_files.size());


    vector<vector<string> > fileContents = Common::readFile(activity_per_window_files[0],',');

    ASSERT_EQ(expectedNumOfWindows,fileContents.size());

    vector<string> lineContent = fileContents[5];

    ASSERT_EQ(expectedFivethWindowActivity,lineContent[0]);

    delete tw;

    bool isFolderDeleted = Common::deleteFolder(tmp_result_path);

    ASSERT_TRUE(isFolderDeleted);

}
