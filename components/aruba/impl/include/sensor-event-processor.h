#ifndef SENSOREVENTPROCESSOR_H
#define SENSOREVENTPROCESSOR_H
#include<home.h>
#include<logger.h>
#include<feature-container.h>
class SensorEventProcessor{
public:
  SensorEventProcessor();
  ~SensorEventProcessor();
  void checkPatternState(vector<string>& sensor_events, int window_size,vector<int>& sensor_durations);
  void destroy();
  bool getEnteredPatternState() {return is_entered_pattern_state_;}
  bool getDefaultPatternState() { return is_default_pattern_state_;}
private:
  void init_();
  void computeMotionToDoorSensorRatio_(vector<string>& sensor_events,float& door_ratio,float& motion_ratio);
  void determinePatternState_(float door_ratio, float motion_ratio,int window_size, vector<int>& sensor_durations);
private:
  vector<float> window_pattern_state_;
  bool is_entered_pattern_state_;
  bool is_default_pattern_state_;

};
#endif // SENSOREVENTPROCESSOR_H
