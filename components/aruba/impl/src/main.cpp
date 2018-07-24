#include<within-day-clustering.h>
#include<between-day-clustering.h>
#include<hierarchal-agglomerative-clustering.h>
#include<heatmap.h>
#include<feature-reader.h>
#include<cluster-recognition.h>
#include<model-recognition.h>
#include<evaluation.h>
#include<logger.h>
#include <ctime>
#include <chrono>

//Within day clustering
string sensors_data_file="../data_sets/aruba/config/data";
string preprocessed_data="../data_sets/aruba/config/processed_data";
string with_day_folder_path="../data_sets/aruba/within_day_cluster_result/";
bool is_data_preprocessed = true;
bool success = true;

//home configuration
string home_setup_file="../data_sets/aruba/config/home_setup";
string time_window_config="../data_sets/aruba/config/time-window-config";
string config_path = "../data_sets/aruba/config/";

//Between day clustering
string betweem_day_folder_path = "../data_sets/aruba/between_day_cluster_result/";

//Hierachal clustering
string hierachal_cluster_folder_path = "../data_sets/aruba/hierachal_cluste_result/";

//patterns statistics
string pattern_stat_folder_path = "../data_sets/aruba/patterns_statistics_result/";
string logger_file = "../data_sets/aruba/log/log.txt";

//recognition using discovered clustered (based on similarity distance)
string cluster_rec_path = "../data_sets/aruba/cluster_recognition/";

//recognition using a trained model
string model_rec_path = "../data_sets/aruba/model_recognition/";

//evaluation
string evaluation_path = "../data_sets/aruba/evaluation/";

/**
 * @brief runWithinDayPattern
 */
void runWithinDayPattern()
{
  WithinDayClustering* withinDayClustering = new WithinDayClustering(home_setup_file,
                                                                     sensors_data_file,
                                                                     preprocessed_data,
                                                                     time_window_config,
                                                                     with_day_folder_path,
                                                                     is_data_preprocessed);
  withinDayClustering->run();
  withinDayClustering->destroy();

}

/**
 * @brief runBetweenDayPattern
 */
void runBetweenDayPattern()
{
  BetweenDayClustering* betweenDayClustering = new BetweenDayClustering(with_day_folder_path,
                                                                        betweem_day_folder_path,
                                                                        pattern_stat_folder_path,
                                                                        home_setup_file,
                                                                        time_window_config,
                                                                        success);

  if(!success)
    {
      betweenDayClustering->destroy();
      std::exit(0);

    }

  betweenDayClustering->run();

  betweenDayClustering->destroy();
}

/**
 * @brief runHierachalCluster
 */
void runHierachalCluster()
{

  HierarchalAgglomerativeClustering* hierarchalAgglomerativeClustering = new HierarchalAgglomerativeClustering(betweem_day_folder_path,
                                                                                                               hierachal_cluster_folder_path,
                                                                                                               home_setup_file,
                                                                                                               time_window_config,
                                                                                                               success);

  if(!success)
    {
      hierarchalAgglomerativeClustering->destroy();
      std::exit(0);
    }

  hierarchalAgglomerativeClustering->run();


  hierarchalAgglomerativeClustering->destroy();
}

void runClusterRecognition()
{

  ClusterRecognition* clusterRecognition = new ClusterRecognition(hierachal_cluster_folder_path,
                                                                  config_path,
                                                                  cluster_rec_path,
                                                                  with_day_folder_path,
                                                                  home_setup_file,
                                                                  time_window_config,
                                                                  success);

  if(!success)
    {
      clusterRecognition->destroy();
      std::exit(0);
    }

  clusterRecognition->run();

  clusterRecognition->destroy();
}

void runModelRecognition()
{

  ModelRecognition* modelRecognition = new ModelRecognition(hierachal_cluster_folder_path,
                                                            config_path,
                                                            model_rec_path,
                                                            with_day_folder_path,
                                                            home_setup_file,
                                                            time_window_config,
                                                            success);

  if(!success)
    {
      modelRecognition->destroy();
      std::exit(0);
    }

  modelRecognition->run();

  modelRecognition->destroy();


}

/**
 * @brief runEvaluation
 */
void  runEvaluation()
{
  Evaluation* evaluation = new Evaluation(evaluation_path,
                                          config_path,
                                          cluster_rec_path,
                                          model_rec_path,
                                          home_setup_file,
                                          time_window_config,
                                          success);

  if(!success)
    {
      evaluation->destroy();
    }

  evaluation->run();

  evaluation->destroy();
}


int main(int argc, char ** argv)
{
  int start_s=clock();
  // the code you wish to time goes here

  logging::configure({ {"type", "file"}, {"file_name", logger_file.c_str()} });
  //runWithinDayPattern();
  //runBetweenDayPattern();
  //runHierachalCluster();
  //runClusterRecognition();
  //runModelRecognition();
  runEvaluation();

  int stop_s=clock();
  stringstream message;

  message<< fixed;
  message << "time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << endl;
  logging::INFO(message.str());

  //string test="../result/2010-12-26/locations_duration.txt";

  //HeatMap* hp = new HeatMap(test,home);
  //hp->drawMostOccuppiedTimeIntervalHeatMap();

}
