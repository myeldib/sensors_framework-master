#ifndef SORTERPROCESSOR_H
#define SORTERPROCESSOR_H
#include<algorithm>
#include<vector>
#include<vector>
#include<iostream>
#include<logger.h>
#include<feature-container.h>
using namespace std;
class SorterProcessor {
public:
  SorterProcessor();
  ~SorterProcessor();
  void destroy();
  void radixSort(FeatureContainer* featureContainer);
  void mergeSort(FeatureContainer* featureContainer);
private:
  void radixSort_(vector<int>& sequence_patterns,
                 vector<vector<float> >& sensors_durations,
                 vector<string>& activity_labels,
                 vector<string>& days_names,
                 vector<float>& time_index);
  void countSort_(vector<int> &sequence_patterns,
                  vector<vector<float> > &sensors_durations,
                  vector<string> &activity_labels,
                  vector<string> &days_names,
                  vector<float> &time_index,
                  int exp);
  void mergeSort_(vector<int>& sequence_patterns,
                 vector<vector<float> >& sensors_durations,
                 vector<string>& activity_labels,
                 vector<string>& days_names,
                 vector<float>& time_index);
private:


};
#endif // SORTERPROCESSOR_H
