#include<sorter-processor.h>
const int NUM_SIGNIFICANT_DIGITS = 10;
/**
 * @brief SorterProcessor::SorterProcessor
 */
SorterProcessor::SorterProcessor()
{
  logging::INFO("SorterProcessor");
}

/**
 * @brief SorterProcessor::~SorterProcessor
 */
SorterProcessor::~SorterProcessor()
{
  logging::INFO("~SorterProcessor");
}

/**
 * @brief SorterProcessor::destroy
 */
void SorterProcessor::destroy()
{
  logging::INFO("destroy");
  delete this;
}

/**
 * @brief SorterProcessor::radixSort
 * @param featureContainer
 */
void SorterProcessor::radixSort(FeatureContainer* featureContainer)
{
  logging::INFO("radixSort");

  vector<int> sequence_patterns = featureContainer->getSequencePatterns();
  vector<vector<float> > sensors_durations = featureContainer->getSensorDurations();
  vector<float>  time_index = featureContainer->getTimeIndexPerPattern();
  vector<string> activity_labels = featureContainer->getActivityLabel();
  vector<string> days_names = featureContainer->getDayNamePerPattern();

  stringstream message;
  message<<"featureContainer sizes:"<<sequence_patterns.size()<<","<<sensors_durations.size()<<","<<time_index.size()<<","<<activity_labels.size()<<","<<days_names.size()<<endl;

  logging::INFO(message.str());

  radixSort_(sequence_patterns,sensors_durations,activity_labels,days_names,time_index);

  featureContainer->setSequencePatterns(sequence_patterns);
  featureContainer->setSensorDurations(sensors_durations);
  featureContainer->setTimeIndexPerPattern(time_index);
  featureContainer->setActivityLabel(activity_labels);
  featureContainer->setDayNamePerPattern(days_names);
}

/**
 * @brief SorterProcessor::countSort_
 * @param sequence_patterns
 * @param sensors_durations
 * @param activity_labels
 * @param days_names
 * @param time_index
 * @param exp
 */
void SorterProcessor::countSort_(vector<int> &sequence_patterns, vector<vector<float> > &sensors_durations, vector<string> &activity_labels, vector<string> &days_names, vector<float> &time_index ,int exp)
{
  int n = sequence_patterns.size();

  //output holders
  vector<int> output_sequence_patterns(n,0);
  vector<vector<float> > output_sensors_durations(n,sensors_durations[0]);
  vector<string> output_activity_labels(n,"");
  vector<string> output_days_names(n,"");
  vector<float> output_time_index(n,0.0);

  int count_sequence_patterns[NUM_SIGNIFICANT_DIGITS] = {0};

  //store count of occurrences in count_sequence_patterns vector
  for (int i = 0; i < n; i++)
    {
      count_sequence_patterns[ (sequence_patterns[i]/exp)%NUM_SIGNIFICANT_DIGITS ]++;
    }

  //change count_sequence_patterns[i] so that count_sequence_patterns[i] now contains actual
  //position of this digit in output_sequence_patterns[]
  for (int i = 1; i < NUM_SIGNIFICANT_DIGITS; i++)
    {
      count_sequence_patterns[i] += count_sequence_patterns[i - 1];
    }

  //build the output vectors
  for (int i = n - 1; i >= 0; i--)
    {
      output_sequence_patterns[count_sequence_patterns[ (sequence_patterns[i]/exp)%NUM_SIGNIFICANT_DIGITS ] - 1] = sequence_patterns[i];
      output_sensors_durations[count_sequence_patterns[ (sequence_patterns[i]/exp)%NUM_SIGNIFICANT_DIGITS ] - 1] = sensors_durations[i];
      output_activity_labels[count_sequence_patterns[ (sequence_patterns[i]/exp)%NUM_SIGNIFICANT_DIGITS ] - 1] = activity_labels[i];
      output_days_names[count_sequence_patterns[ (sequence_patterns[i]/exp)%NUM_SIGNIFICANT_DIGITS ] - 1] = days_names[i];
      output_time_index[count_sequence_patterns[ (sequence_patterns[i]/exp)%NUM_SIGNIFICANT_DIGITS ] - 1] = time_index[i];

      count_sequence_patterns[ (sequence_patterns[i]/exp)%NUM_SIGNIFICANT_DIGITS ]--;
    }

  //copy the output vectors to original holders, so that original holders now
  //contains sorted numbers according to current digit
  for (int i = 0; i < n; i++)
    {
      sequence_patterns[i] = output_sequence_patterns[i];
      sensors_durations[i] = output_sensors_durations[i];
      activity_labels[i] = output_activity_labels[i];
      days_names[i] = output_days_names[i];
      time_index[i] = output_time_index[i];
    }

}

/**
 * @brief ::SorterProcessor::radixSort_
 * @param sequence_patterns
 * @param sensors_durations
 * @param activity_labels
 * @param days_names
 * @param time_index
 */
void::SorterProcessor::radixSort_(vector<int> &sequence_patterns, vector<vector<float> > &sensors_durations, vector<string> &activity_labels, vector<string> &days_names, vector<float> &time_index)
{
  logging::INFO("radixSort_");
  //find the maximum number to know number of digits
  int m = *std::max_element(std::begin(sequence_patterns), std::end(sequence_patterns));

  //do counting sort for every digit. Note that instead
  //of passing digit number, exp is passed. exp is 10^i
  //where i is current digit number
  for (int exp = 1; m/exp > 0; exp *= NUM_SIGNIFICANT_DIGITS)
    {
      countSort_(sequence_patterns, sensors_durations,activity_labels,days_names,time_index,exp);
    }
}
