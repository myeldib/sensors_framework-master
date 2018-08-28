#include<sensor-event-processor.h>

int PATTERN_STATE_SIZE = 2;
int DEFAULT_PATTERN_STATE = 0;
int FACTOR_WINDOW_MULTIPLIER = 2*1000; // in miliseconds

/**
 * @brief SensorEventProcessor::SensorEventProcessor
 */
SensorEventProcessor::SensorEventProcessor()
{
  logging::INFO("SensorEventProcessor");
  init_();
}

/**
 * @brief SensorEventProcessor::init_
 */
void SensorEventProcessor::init_()
{
  logging::INFO("init_");

  //index 0 : inside pattern state
  //index 1: outside pattern state
  for(int i =0; i<PATTERN_STATE_SIZE; i++)
    {
      window_pattern_state_.push_back(0);
    }
}

/**
 * @brief SensorEventProcessor::~SensorEventProcessor
 */
SensorEventProcessor::~SensorEventProcessor()
{
  logging::INFO("~SensorEventProcessor");

}

/**
 * @brief SensorEventProcessor::destroy
 */
void SensorEventProcessor::destroy()
{
  logging::INFO("destroy");

  delete this;
}

/**
 * @brief SensorEventProcessor::checkPatternState
 * @param sensor_events
 * @param window_size
 * @param sensor_durations
 */
void SensorEventProcessor::checkPatternState(vector<string>& sensor_events,int window_size,vector<int>& sensor_durations)
{
  logging::INFO("checkPatternState");

  float door_ratio=0.0;
  float motion_ratio=0.0;
  computeMotionToDoorSensorRatio_(sensor_events,door_ratio,motion_ratio);
  determinePatternState_(door_ratio,motion_ratio,window_size,sensor_durations);
}


/**
 * @brief SensorEventProcessor::computeMotionToDoorSensorRatio_
 * @param sensor_events
 * @param door_ratio
 * @param motion_ratio
 */
void SensorEventProcessor::computeMotionToDoorSensorRatio_(vector<string> &sensor_events, float &door_ratio, float &motion_ratio)
{
  logging::INFO("computeMotionToDoorSensorRatio_");
  float tmp_motion_ratio=0.0;
  float tmp_door_ratio=0.0;
  for(int i =0;i <sensor_events.size();i++)
    {
      if(sensor_events[i].find("M") != std::string::npos)
        {
          tmp_motion_ratio++;
        }
      else if (sensor_events[i].find("D") != std::string::npos)
        {
          tmp_door_ratio++;
        }
    }

  door_ratio=(1.0*tmp_door_ratio)/(sensor_events.size()*1.0);
  motion_ratio=(1.0*tmp_motion_ratio)/(sensor_events.size()*1.0);

}

/**
 * @brief SensorEventProcessor::determinePatternState_
 * @param door_ratio
 * @param motion_ratio
 * @param window_size
 * @param sensor_durations
 */
void SensorEventProcessor::determinePatternState_(float door_ratio, float motion_ratio,int window_size,vector<int> &sensor_durations)
{
  logging::INFO("determinePatternState_");

  //reset flags
  is_entered_pattern_state_ = false;
  is_default_pattern_state_ = false;
  //case 1
  if( (door_ratio > motion_ratio) &&
      (window_pattern_state_[0] == DEFAULT_PATTERN_STATE) &&
      (window_pattern_state_[1] == DEFAULT_PATTERN_STATE) )
    {
      logging::INFO("case 1");
      //update current pattern state
      sensor_durations[sensor_durations.size()-2] = DEFAULT_PATTERN_STATE;
      sensor_durations[sensor_durations.size()-1] = window_size*FACTOR_WINDOW_MULTIPLIER;

      //save pattern state for next checks
      window_pattern_state_[0] = DEFAULT_PATTERN_STATE;
      window_pattern_state_[1] = window_size*FACTOR_WINDOW_MULTIPLIER;
    }

  //case 2
  else if( (door_ratio > motion_ratio) &&
          ( window_pattern_state_[0] == (window_size*FACTOR_WINDOW_MULTIPLIER)) &&
          ( window_pattern_state_[1] == DEFAULT_PATTERN_STATE) )
    {
      logging::INFO("case 2");
      //update current pattern state
      sensor_durations[sensor_durations.size()-2] = DEFAULT_PATTERN_STATE;
      sensor_durations[sensor_durations.size()-1] = window_size*FACTOR_WINDOW_MULTIPLIER;

      //save pattern state for next checks
      window_pattern_state_[0] = DEFAULT_PATTERN_STATE;
      window_pattern_state_[1] = window_size*FACTOR_WINDOW_MULTIPLIER;
    }

  //case 3
  else if( (door_ratio > motion_ratio) &&
          ( window_pattern_state_[0] == DEFAULT_PATTERN_STATE) &&
          ( window_pattern_state_[1] == (window_size*FACTOR_WINDOW_MULTIPLIER)) )
    {
      logging::INFO("case 3");
      //update current pattern state
      sensor_durations[sensor_durations.size()-2] = DEFAULT_PATTERN_STATE;
      sensor_durations[sensor_durations.size()-1] = window_size*FACTOR_WINDOW_MULTIPLIER;

      //save pattern state for next checks
      window_pattern_state_[0] = DEFAULT_PATTERN_STATE;
      window_pattern_state_[1] = window_size*FACTOR_WINDOW_MULTIPLIER;
    }

  //case 4
  else if( (motion_ratio >= door_ratio) &&
          ( window_pattern_state_[0] == DEFAULT_PATTERN_STATE) &&
          ( window_pattern_state_[1] == DEFAULT_PATTERN_STATE) )
    {
      logging::INFO("case 4");
      //update current pattern state
      sensor_durations[sensor_durations.size()-2] = DEFAULT_PATTERN_STATE;
      sensor_durations[sensor_durations.size()-1] = DEFAULT_PATTERN_STATE;

      //save pattern state for next checks
      window_pattern_state_[0] = DEFAULT_PATTERN_STATE;
      window_pattern_state_[1] = DEFAULT_PATTERN_STATE;
    }

  //case 5
  else if( (motion_ratio >= door_ratio) &&
          ( window_pattern_state_[0] == DEFAULT_PATTERN_STATE) &&
          ( window_pattern_state_[1] == (window_size*FACTOR_WINDOW_MULTIPLIER)) )
    {
      logging::INFO("case 5");
      //update current pattern state
      sensor_durations[sensor_durations.size()-2] = window_size*FACTOR_WINDOW_MULTIPLIER;
      sensor_durations[sensor_durations.size()-1] = DEFAULT_PATTERN_STATE;

      //save pattern state for next checks
      window_pattern_state_[0] = window_size*FACTOR_WINDOW_MULTIPLIER;
      window_pattern_state_[1] = DEFAULT_PATTERN_STATE;

      //flag to sync activity label with feature
      is_entered_pattern_state_ = true;
    }


  //case 6
  else if( (motion_ratio >= door_ratio) &&
          ( window_pattern_state_[0] == (window_size*FACTOR_WINDOW_MULTIPLIER)) &&
          ( window_pattern_state_[1] == DEFAULT_PATTERN_STATE) )
    {
      logging::INFO("case 6");
      //update current pattern state
      sensor_durations[sensor_durations.size()-2] = DEFAULT_PATTERN_STATE;
      sensor_durations[sensor_durations.size()-1] = DEFAULT_PATTERN_STATE;

      //save pattern state for next checks
      window_pattern_state_[0] = DEFAULT_PATTERN_STATE;
      window_pattern_state_[1] = DEFAULT_PATTERN_STATE;

      //flag to sync activity label with feature
      is_default_pattern_state_ = true;
    }

  //case 7
  else if( (motion_ratio >= door_ratio) &&
          ( window_pattern_state_[1] == (window_size*FACTOR_WINDOW_MULTIPLIER)) &&
          ( window_pattern_state_[0] == DEFAULT_PATTERN_STATE) )
    {
      logging::INFO("case 7");
      //update current pattern state
      sensor_durations[sensor_durations.size()-2] = DEFAULT_PATTERN_STATE;
      sensor_durations[sensor_durations.size()-1] = DEFAULT_PATTERN_STATE;

      //save pattern state for next checks
      window_pattern_state_[0] = DEFAULT_PATTERN_STATE;
      window_pattern_state_[1] = DEFAULT_PATTERN_STATE;

      //flag to sync activity label with feature
      is_default_pattern_state_ = true;
    }
}
