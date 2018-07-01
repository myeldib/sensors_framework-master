#include<feature-writer.h>

/**
 * @brief FeatureWriter::FeatureWriter
 * @param FeatureContainer
 * @param file_name
 */
FeatureWriter::FeatureWriter(string folder_path,string extension,bool& success)
{
  logging::INFO("FeatureWriter");


  if(folder_path !="")
    {

      this->folder_path = folder_path+
          "_"+
          extension+
          "/";

      logging::INFO(this->folder_path);

      bool is_created =Common::createFolder(this->folder_path);

      if(!is_created)
        {
          logging::ERROR("folder "+this->folder_path+" has already been processed");
          success= false;
          return;
        }
    }
}

/**
 * @brief FeatureWriter::~FeatureWriter
 */
FeatureWriter::~FeatureWriter()
{
  logging::INFO("~FeatureWriter");

}

/**
 * @brief FeatureWriter::writePatternsStatistics
 * @param num_of_discovered_patterns
 * @param num_of_pruned_patterns
 * @param num_of_discovered_activities
 */
void FeatureWriter::writePatternsStatistics(vector<int> num_of_discovered_patterns, vector<int> num_of_pruned_patterns, vector<int> num_of_discovered_activities)
{
  logging::INFO("writePatternsStatistics");

  writePatternStatistics_("num_discovered_patterns.txt",num_of_discovered_patterns);
  writePatternStatistics_("num_pruned_patterns.txt",num_of_pruned_patterns);
  writePatternStatistics_("num_discovered_activities.txt",num_of_discovered_activities);
}

/**
 * @brief FeatureWriter::writeNumOfDiscoveredActivities
 * @param num_of_discovered_activities
 */
void FeatureWriter::writePatternStatistics_(string file,vector<int> entries)
{
  logging::INFO("writePatternStatistics_");
  string file_name = this->folder_path+file;

  fstream output_file(file_name.c_str(),std::fstream::out);

  for(int i = 0; i<entries.size();i++)
    {
      output_file<<entries[i]<<endl;
    }

  output_file.close();
}

/**
 * @brief FeatureWriter::writeBetweenDayPatterns
 * @param fc
 */
void FeatureWriter::writeBetweenDayPatterns_(FeatureContainer *featureContainer)
{

  logging::INFO("writeBetweenDayPatterns");

  writeSensorDurations(featureContainer);
  writeDiscoveredPatterns(featureContainer);
  writeSequencePatterns(featureContainer);
  writeDayNames(featureContainer);
  writeTimeIndex(featureContainer);
  writeActivityLabels(featureContainer);

}

/**
 * @brief FeatureWriter::writeFeatures
 * @param type
 */
void FeatureWriter::writeFeatures(FeatureContainer* featureContainer, Constants::Cluster_Type type)
{
  logging::INFO("writeFeatures");

  if(type == Constants::Cluster_Type::between_day_cluster)
    {
      writeBetweenDayPatterns_(featureContainer);
    }
  else if (type == Constants::Cluster_Type::hierarchal_cluster)
    {
      writeBetweenDayPatterns_(featureContainer);
    }
  else if( type == Constants::Cluster_Type::cluster_recognition)
    {
      writeClusterRecognition_(featureContainer);
    }

}

/**
 * @brief FeatureWriter::writeClusterRecognition
 * @param fc
 */
void FeatureWriter::writeClusterRecognition_(FeatureContainer *fc)
{
  logging::INFO("writeClusterRecognition");

  writeActualActivityLabels_(fc);
  writePredictedActivityLabels_(fc);
  writePredictedDiscoveredPatterns_(fc);
  writeRecognitionAccuracy_(fc);

}

/**
 * @brief FeatureWriter::writePredictedDiscoveredPatterns_
 * @param fc
 */
void FeatureWriter::writePredictedDiscoveredPatterns_(FeatureContainer *fc)
{
  logging::INFO("writePredictedDiscoveredPatterns_");

  string file_name = this->folder_path+"predicted_discovered_patterns.txt";
  fstream output_file(file_name.c_str(),std::fstream::out);

  vector<int> predicted_discovered_patterns = fc->getPredictedDiscoveredPatterns();

  for(int i = 0; i<predicted_discovered_patterns.size();i++)
    {
      output_file<<predicted_discovered_patterns[i]<<endl;
    }

  output_file.close();
}

void FeatureWriter::writeRecognitionAccuracy_(FeatureContainer *fc)
{
  logging::INFO("writeRecognitionAccuracy_");

  string file_name = this->folder_path+"recognition_accuracy.txt";
  fstream output_file(file_name.c_str(),std::fstream::out);

  output_file<<fc->getAccuracyResultsMessage()<<endl;
}
/**
 * @brief FeatureWriter::writeActualActivityLabels_
 * @param fc
 */
void FeatureWriter::writeActualActivityLabels_(FeatureContainer *fc)
{

  logging::INFO("writeActualActivityLabels_");

  string file_name = this->folder_path+"actual_activity_labels.txt";
  fstream output_file(file_name.c_str(),std::fstream::out);

  vector<int> actual_activity_labels = fc->getActualActivityLabels();

  for(int i = 0; i<actual_activity_labels.size();i++)
    {
      output_file<<actual_activity_labels[i]<<endl;
    }

  output_file.close();
}

/**
 * @brief FeatureWriter::writePredictedActivityLabels_
 * @param fc
 */
void FeatureWriter::writePredictedActivityLabels_(FeatureContainer *fc)
{
  logging::INFO("writePredictedActivityLabels_");

  string file_name = this->folder_path+"predicted_activity_labels.txt";
  fstream output_file(file_name.c_str(),std::fstream::out);

  vector<int> predicted_activity_labels = fc->getPredictedActivityLabels();

  for(int i = 0; i<predicted_activity_labels.size();i++)
    {
      output_file<<predicted_activity_labels[i]<<endl;
    }

  output_file.close();

}
/**
 * @brief FeatureWriter::writeSensorDurations
 * @param fc
 */
void FeatureWriter::writeSensorDurations(FeatureContainer *fc)
{

  logging::INFO("writeSensorDurations");

  string file_name = this->folder_path+"sensors_duration.txt";
  fstream output_file(file_name.c_str(),std::fstream::out);

  vector<vector<float> > sensor_durations_per_pattern = fc->getSensorDurations();
  vector<int> discovered_patterns = fc->getDiscoveredPatterns();
  vector<int> sequence_patterns = fc->getSequencePatterns();

  for(int i =0 ; i <discovered_patterns.size();i++)
    {
      for(int j = 0; j< sequence_patterns.size();j++)
        {
          if(discovered_patterns[i] == sequence_patterns[j])
            {
              vector<float> sensor_durations = sensor_durations_per_pattern[j];

              for(int k = 0; k<sensor_durations.size();k++)
                {
                  output_file<<sensor_durations[k]<<",";
                }

              output_file<<endl;
            }
        }
    }

  output_file.close();
}

/**
 * @brief FeatureWriter::writeDiscoveredPatterns
 * @param fc
 */
void FeatureWriter::writeDiscoveredPatterns(FeatureContainer *fc)
{
  logging::INFO("writeDiscoveredPatterns");

  string file_name = this->folder_path+"discovered_patterns.txt";
  fstream output_file(file_name.c_str(),std::fstream::out);

  vector<int> discovered_patterns = fc->getDiscoveredPatterns();

  for(int i = 0; i<discovered_patterns.size();i++)
    {
      output_file<<discovered_patterns[i]<<endl;
    }

  output_file.close();
}

/**
 * @brief FeatureWriter::writeSequencePatterns
 * @param fc
 */
void FeatureWriter::writeSequencePatterns(FeatureContainer *fc)
{
  logging::INFO("writeSequencePatterns");

  string file_name = this->folder_path+"sequence_patterns.txt";
  fstream output_file(file_name.c_str(),std::fstream::out);

  vector<int> discovered_patterns = fc->getDiscoveredPatterns();
  vector<int> sequence_patterns = fc->getSequencePatterns();

  for(int i =0 ; i <discovered_patterns.size();i++)
    {
      for(int j = 0; j< sequence_patterns.size();j++)
        {
          if(discovered_patterns[i] == sequence_patterns[j])
            {
              output_file<<sequence_patterns[j]<<endl;
            }
        }
    }

  output_file.close();
}

/**
 * @brief FeatureWriter::writeActivityLabels
 * @param fc
 */
void FeatureWriter::writeActivityLabels(FeatureContainer *fc)
{
  logging::INFO("writeActivityLabels");

  string file_name = this->folder_path+"activity_per_window.txt";
  fstream output_file(file_name.c_str(),std::fstream::out);

  vector<int> discovered_patterns = fc->getDiscoveredPatterns();
  vector<int> sequence_patterns = fc->getSequencePatterns();
  vector<string> activity_labels = fc->getActivityLabel();

  for(int i =0 ; i <discovered_patterns.size();i++)
    {
      for(int j = 0; j< sequence_patterns.size();j++)
        {
          if(discovered_patterns[i] == sequence_patterns[j])
            {
              output_file<<activity_labels[j]<<endl;
            }
        }
    }

  output_file.close();
}

/**
 * @brief FeatureWriter::writeDayNames
 * @param fc
 */
void FeatureWriter::writeDayNames(FeatureContainer *fc)
{
  logging::INFO("writeDayNames");

  string file_name = this->folder_path+"day_names.txt";
  fstream output_file(file_name.c_str(),std::fstream::out);

  vector<int> discovered_patterns = fc->getDiscoveredPatterns();
  vector<int> sequence_patterns = fc->getSequencePatterns();
  vector<string> day_names = fc->getDayNamePerPattern();

  for(int i =0 ; i <discovered_patterns.size();i++)
    {
      for(int j = 0; j< sequence_patterns.size();j++)
        {
          if(discovered_patterns[i] == sequence_patterns[j])
            {
              output_file<<day_names[j]<<endl;
            }
        }
    }

  output_file.close();
}

/**
 * @brief FeatureWriter::writeTimeIndex
 * @param fc
 */
void FeatureWriter::writeTimeIndex(FeatureContainer *fc)
{
  logging::INFO("writeTimeIndex");

  string file_name = this->folder_path+"time_index.txt";
  fstream output_file(file_name.c_str(),std::fstream::out);

  vector<int> discovered_patterns = fc->getDiscoveredPatterns();
  vector<int> sequence_patterns = fc->getSequencePatterns();
  vector<float> time_index_per_pattern = fc->getTimeIndexPerPattern();

  for(int i =0 ; i <discovered_patterns.size();i++)
    {
      for(int j = 0; j< sequence_patterns.size();j++)
        {
          if(discovered_patterns[i] == sequence_patterns[j])
            {
              output_file<<time_index_per_pattern[j]<<endl;
            }
        }
    }

  output_file.close();
}
