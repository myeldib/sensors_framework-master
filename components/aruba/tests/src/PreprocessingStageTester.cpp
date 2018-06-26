#include<preprocessing-stage.h>
#include<home.h>
#include<test-constants.h>
#include <gtest/gtest.h>

void removeOutputFile(const string& filename)
{
    bool output_exist = Common::fileExists(filename);

    if(output_exist)
    {
        std::remove(filename.c_str());

    }
}

PreprocessingStage* createSUT(const string& inputfile, const string& output_file)
{

    removeOutputFile(output_file);

    Home* home = new Home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);


    if(!home)
    {
        return NULL;
    }


    PreprocessingStage* preposStage = new PreprocessingStage(inputfile,output_file,home);

    if(!preposStage)
    {
        return NULL;
    }

    return preposStage;

}

TEST(PreprocessingStage, creatPreprocessingStageObject) {

    Home* home = new Home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);

    ASSERT_TRUE(home != NULL);

    PreprocessingStage* preposStage = new PreprocessingStage(TestConstants::SENSORS_DATA_FILE,TestConstants::PROCESSED_SENSOR_DATA_OUTPUT_FILE,home);

    ASSERT_TRUE(preposStage != NULL);

    delete preposStage;

}


TEST(PreprocessingStage, readTemperaureOnlySensorData) {

    string tmp ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/temperature_sensor_only";
    string output ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/output";
    int expectedValue = 0;
    int num_lines = 0;

    PreprocessingStage* preposStage = createSUT(tmp,output);

    ASSERT_TRUE(preposStage != NULL);

    preposStage->extractSensorInfo();

    ifstream input_stream( output );

    while (input_stream)
    {
        string line;

        if (!getline( input_stream, line )) break;
        num_lines++;
    }

    input_stream.close();

    ASSERT_EQ(num_lines,expectedValue);


    delete preposStage;

}


TEST(PreprocessingStage, readMotionSensorData_OFF_Only) {


    string tmp ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/off_sensor_only";
    string output ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/output";
    int expectedValue = 0;
    int num_lines = 0;

    PreprocessingStage* preposStage = createSUT(tmp,output);

    ASSERT_TRUE(preposStage != NULL);

    preposStage->extractSensorInfo();

    ifstream input_stream( output );

    while (input_stream)
    {
        string line;

        if (!getline( input_stream, line )) break;
        num_lines++;
    }

    input_stream.close();

    ASSERT_EQ(num_lines,expectedValue);


    delete preposStage;

}


TEST(PreprocessingStage, readArtificialMotionSensorData_Seconds) {


    string tmp ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/artificial_sensor_events_seconds";
    string output ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/output";
    int expectedNumOfLines = 59;
    int expectedDuration = 60000;
    int num_lines = 0;
    int duration=0;

    PreprocessingStage* preposStage = createSUT(tmp,output);

    ASSERT_TRUE(preposStage != NULL);

    preposStage->extractSensorInfo();

    ifstream input_stream( output );

    while (input_stream)
    {
        string line;
        vector<string> elem;

        if (!getline( input_stream, line )) break;
        num_lines++;

        Common::split(line,',',elem);

        duration+=atoi(elem[1].c_str());
    }

    input_stream.close();

    ASSERT_EQ(num_lines,expectedNumOfLines);
    ASSERT_EQ(duration,expectedDuration);



    delete preposStage;

}


TEST(PreprocessingStage, readArtificialMotionSensorData_MiliSeconds) {


    string tmp ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/artificial_sensor_events_mili_seconds";
    string output ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/output";
    int expectedNumOfLines = 60;
    int expectedDuration = 60600;
    int num_lines = 0;
    int duration=0;

    PreprocessingStage* preposStage = createSUT(tmp,output);

    ASSERT_TRUE(preposStage != NULL);

    preposStage->extractSensorInfo();

    ifstream input_stream( output );

    while (input_stream)
    {
        string line;
        vector<string> elem;

        if (!getline( input_stream, line )) break;
        num_lines++;

        Common::split(line,',',elem);

        duration+=atoi(elem[1].c_str());
    }

    input_stream.close();

    ASSERT_EQ(num_lines,expectedNumOfLines);
    ASSERT_EQ(duration,expectedDuration);



    delete preposStage;

}

TEST(PreprocessingStage, readMotionSensorDataActivity_ON_Status) {

    string tmp ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/activity_ON_sensor";
    string output ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/output";
    int expectedNumOfLines = 119;
    int expectedDuration = 120000;
    string expectedActivity="Sleeping";
    int num_lines = 0;
    int duration=0;
    string activity="";

    PreprocessingStage* preposStage = createSUT(tmp,output);

    ASSERT_TRUE(preposStage != NULL);

    preposStage->extractSensorInfo();

    ifstream input_stream( output );

    while (input_stream)
    {
        string line;
        vector<string> elem;

        if (!getline( input_stream, line )) break;
        num_lines++;

        Common::split(line,',',elem);

        duration+=atoi(elem[1].c_str());
        activity=elem[4];

    }

    input_stream.close();

    ASSERT_EQ(num_lines,expectedNumOfLines);
    ASSERT_EQ(duration,expectedDuration);
    ASSERT_EQ(activity,expectedActivity);


    delete preposStage;

}

TEST(PreprocessingStage, readMotionSensorDataActivity_OFF_Status) {

    string tmp ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/activity_OFF_sensor";
    string output ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/output";
    int expectedNumOfLines = 119;
    int expectedDuration = 120000;
    string expectedActivity="Sleeping";
    int num_lines = 0;
    int duration=0;
    string activity="";

    PreprocessingStage* preposStage = createSUT(tmp,output);

    ASSERT_TRUE(preposStage != NULL);

    preposStage->extractSensorInfo();

    ifstream input_stream( output );

    while (input_stream)
    {
        string line;
        vector<string> elem;

        if (!getline( input_stream, line )) break;
        num_lines++;

        Common::split(line,',',elem);

        duration+=atoi(elem[1].c_str());
        activity=elem[4];

    }

    input_stream.close();

    ASSERT_EQ(num_lines,expectedNumOfLines);
    ASSERT_EQ(duration,expectedDuration);
    ASSERT_EQ(activity,expectedActivity);


    delete preposStage;

}

TEST(PreprocessingStage, readMotionSensorDataActivity_ON_OFF_Status) {

    string tmp ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/activity_ON_OFF_sensor";
    string output ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/output";
    int expectedNumOfLines = 119;
    int expectedDuration = 120000;
    string expectedActivity="Sleeping";
    int num_lines = 0;
    int duration=0;
    string activity="";

    PreprocessingStage* preposStage = createSUT(tmp,output);

    ASSERT_TRUE(preposStage != NULL);

    preposStage->extractSensorInfo();

    ifstream input_stream( output );

    while (input_stream)
    {
        string line;
        vector<string> elem;

        if (!getline( input_stream, line )) break;
        num_lines++;

        Common::split(line,',',elem);

        duration+=atoi(elem[1].c_str());
        activity=elem[4];

    }

    input_stream.close();

    ASSERT_EQ(num_lines,expectedNumOfLines);
    ASSERT_EQ(duration,expectedDuration);
    ASSERT_EQ(activity,expectedActivity);


    delete preposStage;

}

TEST(PreprocessingStage, readMotionSensorDataActivity_OFF_ON_Status) {

    string tmp ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/activity_OFF_ON_sensor";
    string output ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/output";
    int expectedNumOfLines = 119;
    int expectedDuration = 120000;
    string expectedActivity="Sleeping";
    int num_lines = 0;
    int duration=0;
    string activity="";

    PreprocessingStage* preposStage = createSUT(tmp,output);

    ASSERT_TRUE(preposStage != NULL);

    preposStage->extractSensorInfo();

    ifstream input_stream( output );

    while (input_stream)
    {
        string line;
        vector<string> elem;

        if (!getline( input_stream, line )) break;
        num_lines++;

        Common::split(line,',',elem);

        duration+=atoi(elem[1].c_str());
        activity=elem[4];

    }

    input_stream.close();

    ASSERT_EQ(num_lines,expectedNumOfLines);
    ASSERT_EQ(duration,expectedDuration);
    ASSERT_EQ(activity,expectedActivity);


    delete preposStage;

}


TEST(PreprocessingStage, readMotionSensorDataActivity_OneFullDay) {

    string tmp ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/one_full_day";
    string output ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/output";
    int expectedDuration = 86400000;
    int duration=0;

    PreprocessingStage* preposStage = createSUT(tmp,output);

    ASSERT_TRUE(preposStage != NULL);

    preposStage->extractSensorInfo();

    ifstream input_stream( output );

    while (input_stream)
    {
        string line;
        vector<string> elem;

        if (!getline( input_stream, line )) break;

        Common::split(line,',',elem);

        duration+=atoi(elem[1].c_str());

    }

    input_stream.close();

    ASSERT_EQ(duration,expectedDuration);


    delete preposStage;

}

TEST(PreprocessingStage, readMotionSensorDataActivity_TwoFullDays) {

    string tmp ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/two_full_days";
    string output ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/output";
    int expectedDuration = 86400000*2;
    int duration=0;

    PreprocessingStage* preposStage = createSUT(tmp,output);

    ASSERT_TRUE(preposStage != NULL);

    preposStage->extractSensorInfo();

    ifstream input_stream( output );

    while (input_stream)
    {
        string line;
        vector<string> elem;

        if (!getline( input_stream, line )) break;

        Common::split(line,',',elem);

        duration+=atoi(elem[1].c_str());

    }

    input_stream.close();

    ASSERT_EQ(duration,expectedDuration);


    delete preposStage;

}

TEST(PreprocessingStage, readMotionSensorDataActivity_ThreeFullDays) {

    string tmp ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/three_full_days";
    string output ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/output";
    int expectedDuration = 86400000*3;
    int duration=0;

    PreprocessingStage* preposStage = createSUT(tmp,output);

    ASSERT_TRUE(preposStage != NULL);

    preposStage->extractSensorInfo();

    ifstream input_stream( output );

    while (input_stream)
    {
        string line;
        vector<string> elem;

        if (!getline( input_stream, line )) break;

        Common::split(line,',',elem);

        duration+=atoi(elem[1].c_str());

    }

    input_stream.close();

    ASSERT_EQ(duration,expectedDuration);


    delete preposStage;

}

TEST(PreprocessingStage, readMotionSensorDataActivity_FiveFullDays) {

    string tmp ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/five_full_days";
    string output ="../../../../components/aruba/tests/test_data/config_per_test/PreprocessingStageTester/output";
    int expectedDuration = 86400000*5;
    int duration=0;

    PreprocessingStage* preposStage = createSUT(tmp,output);

    ASSERT_TRUE(preposStage != NULL);

    preposStage->extractSensorInfo();

    ifstream input_stream( output );

    while (input_stream)
    {
        string line;
        vector<string> elem;

        if (!getline( input_stream, line )) break;

        Common::split(line,',',elem);

        duration+=atoi(elem[1].c_str());

    }

    input_stream.close();

    ASSERT_EQ(duration,expectedDuration);


    delete preposStage;

}
