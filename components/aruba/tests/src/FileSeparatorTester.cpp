#include<file-separator.h>
#include<home.h>
#include<test-constants.h>
#include <gtest/gtest.h>


void getDurationFromFile(string& filename,int& duration)
{
    ifstream input_stream( filename );

    while (input_stream)
    {
        string line;
        vector<string> elem;

        if (!getline( input_stream, line )) break;

        Common::split(line,',',elem);

        duration+=atoi(elem[1].c_str());
    }
}

TEST(FileSeparator, creatFileSeparatorObject)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/FileSeparatorTester/result/";
    FileSeparator* fs = new FileSeparator(TestConstants::PROCESSED_SENSOR_DATA_OUTPUT_FILE,tmp_result_path);

    ASSERT_TRUE(fs != NULL);

    delete fs;

}


TEST(FileSeparator, splitProcessedFileTFolders)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/FileSeparatorTester/result/";
    int expectedFolderCount = 3;

    bool isFolderCreated = Common::createFolder(tmp_result_path);

    ASSERT_TRUE(isFolderCreated);

    FileSeparator* fs = new FileSeparator(TestConstants::PROCESSED_SENSOR_DATA_OUTPUT_FILE,tmp_result_path);

    ASSERT_TRUE(fs != NULL);

    fs->writeDailySensorData();

    vector<string> folders = Common::getFolderFileNames(tmp_result_path,"");


    ASSERT_EQ(folders.size(), expectedFolderCount);

    delete fs;

    bool isFolderDeleted = Common::deleteFolder(tmp_result_path);

    ASSERT_TRUE(isFolderDeleted);


}

TEST(FileSeparator, checkSplittedFoldersDuration)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/FileSeparatorTester/result/";
    string tmp_input = "../../../../components/aruba/tests/test_data/config_per_test/FileSeparatorTester/sub_processed";
    int expectedFolderCount = 5;
    int expectedDuration = 86400000;
    int expectedDurationDiff = 1000;

    bool isFolderCreated = Common::createFolder(tmp_result_path);

    ASSERT_TRUE(isFolderCreated);

    FileSeparator* fs = new FileSeparator(tmp_input,tmp_result_path);

    ASSERT_TRUE(fs != NULL);

    fs->writeDailySensorData();

    vector<string> folders = Common::getFolderFileNames(tmp_result_path,"");
    ASSERT_EQ(expectedFolderCount,folders.size());

    for(int i=0;i<folders.size();i++)
    {
        int duration = 0;
        getDurationFromFile(folders[i],duration);
        float diff = abs(expectedDuration-duration );

        ASSERT_TRUE(diff<expectedDurationDiff);

    }

    delete fs;

    bool isFolderDeleted = Common::deleteFolder(tmp_result_path);

    ASSERT_TRUE(isFolderDeleted);


}
