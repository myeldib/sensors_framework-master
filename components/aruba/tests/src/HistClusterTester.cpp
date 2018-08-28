#include<home.h>
#include<hist-cluster.h>
#include<test-constants.h>
#include <gtest/gtest.h>


HistogramClustering* createSUT(string& folder_path)
{

    Home* home = new Home(TestConstants::HOME_SETUP_FILE,TestConstants::TIME_WINDOW_CONFIG);


    if(!home)
    {
        return NULL;
    }

    HistogramClustering* hc = new HistogramClustering(folder_path,home);

    if(!hc)
    {
        return NULL;
    }

    return hc;

}

HistogramClustering* createSUT2(string& folder_path,string& home_setup_file, string& time_window_config_file)
{

    Home* home = new Home(home_setup_file,time_window_config_file);


    if(!home)
    {
        return NULL;
    }

    HistogramClustering* hc = new HistogramClustering(folder_path,home);

    if(!hc)
    {
        return NULL;
    }

    return hc;

}

TEST(HistogramClustering, createHistogramClusteringObject)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/HistClusterTester/result/";
    HistogramClustering* hc = createSUT(tmp_result_path);

    ASSERT_TRUE(hc != NULL);

    delete hc;

}

TEST(HistogramClustering, sameAssignedCluster)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/HistClusterTester/same_cluster/";
    HistogramClustering* hc = createSUT(tmp_result_path);
    int expectedNumDays = 1;
    int expectedNumSensorWindows = 30;
    string expectedAssignedCluster = "1";

    ASSERT_TRUE(hc != NULL);

    hc->run();

    vector<string> sequence_patterns_file=Common::getFolderFileNames(tmp_result_path,"sequence_patterns");

    vector<vector<string> > fileContents = Common::readFile(sequence_patterns_file[0],',');

    ASSERT_EQ(expectedNumDays, sequence_patterns_file.size());


    ASSERT_EQ(expectedNumSensorWindows,fileContents.size());

    ASSERT_EQ(expectedAssignedCluster,fileContents[0][0]);

    delete hc;

}

TEST(HistogramClustering, differentAssignedCluster)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/HistClusterTester/different_cluster/";
    HistogramClustering* hc = createSUT(tmp_result_path);
    int expectedNumDays = 1;
    int expectedNumSensorWindows = 30;
    string expectedFirstAssignedCluster = "1";
    string expectedSecondAssignedCluster = "2";

    ASSERT_TRUE(hc != NULL);

    hc->run();

    //get folder names
    vector<string> sequence_patterns_file=Common::getFolderFileNames(tmp_result_path,"sequence_patterns");

    vector<vector<string> > fileContents = Common::readFile(sequence_patterns_file[0],',');


    ASSERT_EQ(expectedNumDays, sequence_patterns_file.size());
    ASSERT_EQ(expectedNumSensorWindows,fileContents.size());

    ASSERT_EQ(expectedFirstAssignedCluster,fileContents[0][0]);
    ASSERT_EQ(expectedSecondAssignedCluster,fileContents[29][0]);


    delete hc;

}

TEST(HistogramClustering, compareSequencePatternWithDiscoveredPatternAssignment)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/HistClusterTester/different_cluster/";
    HistogramClustering* hc = createSUT(tmp_result_path);
    int expectedNumDays = 1;
    int expectedSequencePatterns = 30;
    int expectedDiscoveredPatterns = 2;


    ASSERT_TRUE(hc != NULL);

    hc->run();

    //get folder names
    vector<string> sequence_patterns_file=Common::getFolderFileNames(tmp_result_path,"sequence_patterns");
    vector<string> discovered_patterns_file=Common::getFolderFileNames(tmp_result_path,"discovered_patterns");

    vector<vector<string> > sequencePatternsContents = Common::readFile(sequence_patterns_file[0],',');
    vector<vector<string> > discoverePatternsContents = Common::readFile(discovered_patterns_file[0],',');


    ASSERT_EQ(expectedNumDays, sequence_patterns_file.size());
    ASSERT_EQ(expectedNumDays, discovered_patterns_file.size());

    ASSERT_EQ(expectedSequencePatterns,sequencePatternsContents.size());
    ASSERT_EQ(expectedDiscoveredPatterns,discoverePatternsContents.size());

    for(int i =0 ;i<discoverePatternsContents.size();i++)
    {
        bool isMatched = false;
        for(int j= 0 ; j<sequencePatternsContents.size();j++)
        {
            if(discoverePatternsContents[i][0]==sequencePatternsContents[j][0])
            {
                isMatched = true ;
            }
        }
        ASSERT_TRUE(isMatched);
    }

    delete hc;

}

TEST(HistogramClustering, differentActivityInSameWindow)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/HistClusterTester/different_cluster/";
    HistogramClustering* hc = createSUT(tmp_result_path);
    int expectedNumDays = 1;
    int expectedSequencePatterns = 30;
    int expectedDiscoveredPatterns = 2;


    ASSERT_TRUE(hc != NULL);

    hc->run();

    //get folder names
    vector<string> sequence_patterns_file=Common::getFolderFileNames(tmp_result_path,"sequence_patterns");
    vector<string> discovered_patterns_file=Common::getFolderFileNames(tmp_result_path,"discovered_patterns");

    vector<vector<string> > sequencePatternsContents = Common::readFile(sequence_patterns_file[0],',');
    vector<vector<string> > discoverePatternsContents = Common::readFile(discovered_patterns_file[0],',');


    ASSERT_EQ(expectedNumDays, sequence_patterns_file.size());
    ASSERT_EQ(expectedNumDays, discovered_patterns_file.size());

    ASSERT_EQ(expectedSequencePatterns,sequencePatternsContents.size());
    ASSERT_EQ(expectedDiscoveredPatterns,discoverePatternsContents.size());

    for(int i =0 ;i<discoverePatternsContents.size();i++)
    {
        bool isMatched = false;
        for(int j= 0 ; j<sequencePatternsContents.size();j++)
        {
            if(discoverePatternsContents[i][0]==sequencePatternsContents[j][0])
            {
                isMatched = true ;
            }
        }
        ASSERT_TRUE(isMatched);
    }

    delete hc;

}


TEST(HistogramClustering, sameSensorsOutsideInsidePatterns)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/HistClusterTester/same_sensors_inside_outside_pattern/";
    string home_setup_file = "../../../../components/aruba/tests/test_data/config_per_test/HistClusterTester/same_sensors_inside_outside_pattern/2010-11-04/home_setup";
    string time_window_config_file = "../../../../components/aruba/tests/test_data/config_per_test/HistClusterTester/same_sensors_inside_outside_pattern/2010-11-04/time-window-config";

    HistogramClustering* hc = createSUT2(tmp_result_path,home_setup_file,time_window_config_file);
    int expectedNumDays = 1;
    int expectedSequencePatterns = 30;
    int expectedDiscoveredPatterns = 2;


    ASSERT_TRUE(hc != NULL);

    hc->run();

    //get folder names
    vector<string> sequence_patterns_file=Common::getFolderFileNames(tmp_result_path,"sequence_patterns");
    vector<string> discovered_patterns_file=Common::getFolderFileNames(tmp_result_path,"discovered_patterns");

    vector<vector<string> > sequencePatternsContents = Common::readFile(sequence_patterns_file[0],',');
    vector<vector<string> > discoverePatternsContents = Common::readFile(discovered_patterns_file[0],',');


    ASSERT_EQ(expectedNumDays, sequence_patterns_file.size());
    ASSERT_EQ(expectedNumDays, discovered_patterns_file.size());

    ASSERT_EQ(expectedSequencePatterns,sequencePatternsContents.size());
    ASSERT_EQ(expectedDiscoveredPatterns,discoverePatternsContents.size());

    for(int i =0 ;i<discoverePatternsContents.size();i++)
    {
        bool isMatched = false;
        for(int j= 0 ; j<sequencePatternsContents.size();j++)
        {
            if(discoverePatternsContents[i][0]==sequencePatternsContents[j][0])
            {
                isMatched = true ;
            }
        }
        ASSERT_TRUE(isMatched);
    }

    delete hc;

}


TEST(HistogramClustering, diffSensorsOutsideInsidePatterns)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/HistClusterTester/diff_sensors_inside_outside_pattern/";
    string home_setup_file = "../../../../components/aruba/tests/test_data/config_per_test/HistClusterTester/diff_sensors_inside_outside_pattern/2010-11-04/home_setup";
    string time_window_config_file = "../../../../components/aruba/tests/test_data/config_per_test/HistClusterTester/diff_sensors_inside_outside_pattern/2010-11-04/time-window-config";

    HistogramClustering* hc = createSUT2(tmp_result_path,home_setup_file,time_window_config_file);
    int expectedNumDays = 1;
    int expectedSequencePatterns = 30;
    int expectedDiscoveredPatterns = 2;


    ASSERT_TRUE(hc != NULL);

    hc->run();

    //get folder names
    vector<string> sequence_patterns_file=Common::getFolderFileNames(tmp_result_path,"sequence_patterns");
    vector<string> discovered_patterns_file=Common::getFolderFileNames(tmp_result_path,"discovered_patterns");

    vector<vector<string> > sequencePatternsContents = Common::readFile(sequence_patterns_file[0],',');
    vector<vector<string> > discoverePatternsContents = Common::readFile(discovered_patterns_file[0],',');


    ASSERT_EQ(expectedNumDays, sequence_patterns_file.size());
    ASSERT_EQ(expectedNumDays, discovered_patterns_file.size());

    ASSERT_EQ(expectedSequencePatterns,sequencePatternsContents.size());
    ASSERT_EQ(expectedDiscoveredPatterns,discoverePatternsContents.size());

    for(int i =0 ;i<discoverePatternsContents.size();i++)
    {
        bool isMatched = false;
        for(int j= 0 ; j<sequencePatternsContents.size();j++)
        {
            if(discoverePatternsContents[i][0]==sequencePatternsContents[j][0])
            {
                isMatched = true ;
            }
        }
        ASSERT_TRUE(isMatched);
    }

    delete hc;

}

TEST(HistogramClustering, diffSensorsOutsidePatterns)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/HistClusterTester/diff_sensors_outside_pattern/";
    string home_setup_file = "../../../../components/aruba/tests/test_data/config_per_test/HistClusterTester/diff_sensors_outside_pattern/2010-11-04/home_setup";
    string time_window_config_file = "../../../../components/aruba/tests/test_data/config_per_test/HistClusterTester/diff_sensors_outside_pattern/2010-11-04/time-window-config";

    HistogramClustering* hc = createSUT2(tmp_result_path,home_setup_file,time_window_config_file);
    int expectedNumDays = 1;
    int expectedSequencePatterns = 30;
    int expectedDiscoveredPatterns = 2;


    ASSERT_TRUE(hc != NULL);

    hc->run();

    //get folder names
    vector<string> sequence_patterns_file=Common::getFolderFileNames(tmp_result_path,"sequence_patterns");
    vector<string> discovered_patterns_file=Common::getFolderFileNames(tmp_result_path,"discovered_patterns");

    vector<vector<string> > sequencePatternsContents = Common::readFile(sequence_patterns_file[0],',');
    vector<vector<string> > discoverePatternsContents = Common::readFile(discovered_patterns_file[0],',');


    ASSERT_EQ(expectedNumDays, sequence_patterns_file.size());
    ASSERT_EQ(expectedNumDays, discovered_patterns_file.size());

    ASSERT_EQ(expectedSequencePatterns,sequencePatternsContents.size());
    ASSERT_EQ(expectedDiscoveredPatterns,discoverePatternsContents.size());

    for(int i =0 ;i<discoverePatternsContents.size();i++)
    {
        bool isMatched = false;
        for(int j= 0 ; j<sequencePatternsContents.size();j++)
        {
            if(discoverePatternsContents[i][0]==sequencePatternsContents[j][0])
            {
                isMatched = true ;
            }
        }
        ASSERT_TRUE(isMatched);
    }

    delete hc;

}

TEST(HistogramClustering, sameSensorsOutsidePatterns)
{

    string tmp_result_path="../../../../components/aruba/tests/test_data/config_per_test/HistClusterTester/same_sensors_outside_pattern/";
    string home_setup_file = "../../../../components/aruba/tests/test_data/config_per_test/HistClusterTester/same_sensors_outside_pattern/2010-11-04/home_setup";
    string time_window_config_file = "../../../../components/aruba/tests/test_data/config_per_test/HistClusterTester/same_sensors_outside_pattern/2010-11-04/time-window-config";

    HistogramClustering* hc = createSUT2(tmp_result_path,home_setup_file,time_window_config_file);
    int expectedNumDays = 1;
    int expectedSequencePatterns = 30;
    int expectedDiscoveredPatterns = 1;


    ASSERT_TRUE(hc != NULL);

    hc->run();

    //get folder names
    vector<string> sequence_patterns_file=Common::getFolderFileNames(tmp_result_path,"sequence_patterns");
    vector<string> discovered_patterns_file=Common::getFolderFileNames(tmp_result_path,"discovered_patterns");

    vector<vector<string> > sequencePatternsContents = Common::readFile(sequence_patterns_file[0],',');
    vector<vector<string> > discoverePatternsContents = Common::readFile(discovered_patterns_file[0],',');


    ASSERT_EQ(expectedNumDays, sequence_patterns_file.size());
    ASSERT_EQ(expectedNumDays, discovered_patterns_file.size());

    ASSERT_EQ(expectedSequencePatterns,sequencePatternsContents.size());
    ASSERT_EQ(expectedDiscoveredPatterns,discoverePatternsContents.size());

    for(int i =0 ;i<discoverePatternsContents.size();i++)
    {
        bool isMatched = false;
        for(int j= 0 ; j<sequencePatternsContents.size();j++)
        {
            if(discoverePatternsContents[i][0]==sequencePatternsContents[j][0])
            {
                isMatched = true ;
            }
        }
        ASSERT_TRUE(isMatched);
    }

    delete hc;

}
