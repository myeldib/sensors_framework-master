#include "sensor-event-processor.h"
#include "test-constants.h"
#include <gtest/gtest.h>

vector<string> createDoorSensors(int size)
{
  vector<string> door_sensors;

  for(int i =0; i<size; i++)
    {
      door_sensors.push_back("D");
    }
  return door_sensors;
}

vector<string> createMotionSensors(int size)
{
  vector<string> motion_sensors;

  for(int i =0; i<size; i++)
    {
      motion_sensors.push_back("M");
    }
  return motion_sensors;
}

vector<string> createMoreMotionThanDoorSensors(int size)
{
  vector<string> mixed_sensors;

  for(int  i =0; i<size; i++)
    {
      mixed_sensors.push_back("M");
    }

  mixed_sensors[size-1] = "D";

  return mixed_sensors;
}

vector<string> createMoreDoorThanMotionSensors(int size)
{
  vector<string> mixed_sensors;

  for(int  i =0; i<size; i++)
    {
      mixed_sensors.push_back("D");
    }

  mixed_sensors[size-1] = "M";

  return mixed_sensors;
}

vector<int> createSensorDuration(int size)
{
  vector<int> sensor_durations;

  for(int i = 0 ; i <size; i++)
    {
      sensor_durations.push_back(0);
    }

  sensor_durations[0]= 60000;

  return sensor_durations;
}

TEST(SensorEventProcessor, createSensorEventProcessorObject) {

  string home_setup_file ="../../../../components/aruba/tests/test_data/config_per_test/SensorEventProcessorTester/default_to_out_case/home_setup";
  string time_window_file="../../../../components/aruba/tests/test_data/config_per_test/SensorEventProcessorTester/default_to_out_case/time-window-config";

  Home* home = new Home(home_setup_file,time_window_file);

  ASSERT_TRUE(home != NULL);

  home->readHomeSetup();

  SensorEventProcessor* sensorEventProcessor = new SensorEventProcessor();

  ASSERT_TRUE(sensorEventProcessor != NULL);

  delete sensorEventProcessor;
  delete home;

}


TEST(SensorEventProcessor, defaultToOutCase) {

  string home_setup_file ="../../../../components/aruba/tests/test_data/config_per_test/SensorEventProcessorTester/default_to_out_case/home_setup";
  string time_window_file="../../../../components/aruba/tests/test_data/config_per_test/SensorEventProcessorTester/default_to_out_case/time-window-config";

  Home* home = new Home(home_setup_file,time_window_file);

  ASSERT_TRUE(home != NULL);

  home->readHomeSetup();

  int window_size = 10;
  vector<string> sensor_events1 = createMotionSensors(window_size);
  vector<int> sensor_durations1 = createSensorDuration(home->getHomeSensorsId().size());

  SensorEventProcessor* sensorEventProcessor = new SensorEventProcessor();

  ASSERT_TRUE(sensorEventProcessor != NULL);

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);

  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-1]);

  sensor_events1 = createDoorSensors(window_size);
  sensor_durations1 = createSensorDuration(home->getHomeSensorsId().size());

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);


  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(20000,sensor_durations1[sensor_durations1.size()-1]);


  delete sensorEventProcessor;
  delete home;
}

TEST(SensorEventProcessor, outToOutCase) {

  string home_setup_file ="../../../../components/aruba/tests/test_data/config_per_test/SensorEventProcessorTester/default_to_out_case/home_setup";
  string time_window_file="../../../../components/aruba/tests/test_data/config_per_test/SensorEventProcessorTester/default_to_out_case/time-window-config";

  Home* home = new Home(home_setup_file,time_window_file);

  ASSERT_TRUE(home != NULL);

  home->readHomeSetup();

  int window_size = 10;
  vector<string> sensor_events1 = createMotionSensors(window_size);
  vector<int> sensor_durations1 = createSensorDuration(home->getHomeSensorsId().size());

  SensorEventProcessor* sensorEventProcessor = new SensorEventProcessor();

  ASSERT_TRUE(sensorEventProcessor != NULL);

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);

  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-1]);

  sensor_events1 = createDoorSensors(window_size);
  sensor_durations1 = createSensorDuration(home->getHomeSensorsId().size());

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);


  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(20000,sensor_durations1[sensor_durations1.size()-1]);

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);


  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(20000,sensor_durations1[sensor_durations1.size()-1]);


  delete sensorEventProcessor;
  delete home;
}

TEST(SensorEventProcessor, outToInCase) {

  string home_setup_file ="../../../../components/aruba/tests/test_data/config_per_test/SensorEventProcessorTester/default_to_out_case/home_setup";
  string time_window_file="../../../../components/aruba/tests/test_data/config_per_test/SensorEventProcessorTester/default_to_out_case/time-window-config";

  Home* home = new Home(home_setup_file,time_window_file);

  ASSERT_TRUE(home != NULL);

  home->readHomeSetup();

  int window_size = 10;
  vector<string> sensor_events1 = createMotionSensors(window_size);
  vector<int> sensor_durations1 = createSensorDuration(home->getHomeSensorsId().size());

  SensorEventProcessor* sensorEventProcessor = new SensorEventProcessor();

  ASSERT_TRUE(sensorEventProcessor != NULL);

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);

  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-1]);

  sensor_events1 = createDoorSensors(window_size);
  sensor_durations1 = createSensorDuration(home->getHomeSensorsId().size());

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);


  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(20000,sensor_durations1[sensor_durations1.size()-1]);

  sensor_events1 = createMoreMotionThanDoorSensors(window_size);
  sensor_durations1 = createSensorDuration(home->getHomeSensorsId().size());

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);


  ASSERT_FLOAT_EQ(20000,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-1]);


  delete sensorEventProcessor;
  delete home;
}

TEST(SensorEventProcessor, InToOutCase) {

  string home_setup_file ="../../../../components/aruba/tests/test_data/config_per_test/SensorEventProcessorTester/default_to_out_case/home_setup";
  string time_window_file="../../../../components/aruba/tests/test_data/config_per_test/SensorEventProcessorTester/default_to_out_case/time-window-config";

  Home* home = new Home(home_setup_file,time_window_file);

  ASSERT_TRUE(home != NULL);

  home->readHomeSetup();

  SensorEventProcessor* sensorEventProcessor = new SensorEventProcessor();

  ASSERT_TRUE(sensorEventProcessor != NULL);

  int window_size = 10;
  vector<string> sensor_events1 = createMotionSensors(window_size);
  vector<int> sensor_durations1 = createSensorDuration(home->getHomeSensorsId().size());

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);

  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-1]);

  sensor_events1 = createDoorSensors(window_size);
  sensor_durations1 = createSensorDuration(home->getHomeSensorsId().size());

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);


  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(20000,sensor_durations1[sensor_durations1.size()-1]);

  sensor_events1 = createMoreMotionThanDoorSensors(window_size);
  sensor_durations1 = createSensorDuration(home->getHomeSensorsId().size());

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);


  ASSERT_FLOAT_EQ(20000,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-1]);

  sensor_events1 = createMoreDoorThanMotionSensors(window_size);
  sensor_durations1 = createSensorDuration(home->getHomeSensorsId().size());

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);


  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(20000,sensor_durations1[sensor_durations1.size()-1]);


  delete sensorEventProcessor;
  delete home;
}

TEST(SensorEventProcessor, InToDefaultCase) {

  string home_setup_file ="../../../../components/aruba/tests/test_data/config_per_test/SensorEventProcessorTester/default_to_out_case/home_setup";
  string time_window_file="../../../../components/aruba/tests/test_data/config_per_test/SensorEventProcessorTester/default_to_out_case/time-window-config";

  Home* home = new Home(home_setup_file,time_window_file);

  ASSERT_TRUE(home != NULL);

  home->readHomeSetup();

  SensorEventProcessor* sensorEventProcessor = new SensorEventProcessor();

  ASSERT_TRUE(sensorEventProcessor != NULL);

  int window_size = 10;
  vector<string> sensor_events1 = createMotionSensors(window_size);
  vector<int> sensor_durations1 = createSensorDuration(home->getHomeSensorsId().size());

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);

  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-1]);

  sensor_events1 = createDoorSensors(window_size);
  sensor_durations1 = createSensorDuration(home->getHomeSensorsId().size());

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);


  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(20000,sensor_durations1[sensor_durations1.size()-1]);

  sensor_events1 = createMoreMotionThanDoorSensors(window_size);
  sensor_durations1 = createSensorDuration(home->getHomeSensorsId().size());

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);


  ASSERT_FLOAT_EQ(20000,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-1]);

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);


  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-1]);

  delete sensorEventProcessor;
  delete home;
}

TEST(SensorEventProcessor, defaultToDefaultCase) {

  string home_setup_file ="../../../../components/aruba/tests/test_data/config_per_test/SensorEventProcessorTester/default_to_out_case/home_setup";
  string time_window_file="../../../../components/aruba/tests/test_data/config_per_test/SensorEventProcessorTester/default_to_out_case/time-window-config";

  Home* home = new Home(home_setup_file,time_window_file);

  ASSERT_TRUE(home != NULL);

  home->readHomeSetup();

  SensorEventProcessor* sensorEventProcessor = new SensorEventProcessor();

  ASSERT_TRUE(sensorEventProcessor != NULL);

  int window_size = 10;
  vector<string> sensor_events1 = createMotionSensors(window_size);
  vector<int> sensor_durations1 = createSensorDuration(home->getHomeSensorsId().size());

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);

  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-1]);

  sensorEventProcessor->checkPatternState(sensor_events1,window_size,sensor_durations1);

  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-2]);
  ASSERT_FLOAT_EQ(0.0,sensor_durations1[sensor_durations1.size()-1]);


  delete sensorEventProcessor;
  delete home;
}
