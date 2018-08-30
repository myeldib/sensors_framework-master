//#include<within-day-clustering.h>
//#include<between-day-clustering.h>
//#include<hierarchal-agglomerative-clustering.h>
//#include<heatmap.h>
//#include<feature-reader.h>
//#include<cluster-recognition.h>
//#include<model-recognition.h>
//#include<evaluation.h>
//#include<logger.h>
//#include <ctime>
//#include <chrono>

////Within day clustering
//string sensors_data_file="../data_sets/aruba/config/data";
//string preprocessed_data="../data_sets/aruba/config/processed_data";
//string with_day_folder_path="../data_sets/aruba/within_day_cluster_result/";
//bool is_data_preprocessed = true;
//bool success = true;

////home configuration
//string home_setup_file="../data_sets/aruba/config/home_setup";
//string time_window_config="../data_sets/aruba/config/time-window-config";
//string config_path = "../data_sets/aruba/config/";

////Between day clustering
//string betweem_day_folder_path = "../data_sets/aruba/between_day_cluster_result/";

////Hierachal clustering
//string hierachal_cluster_folder_path = "../data_sets/aruba/hierachal_cluste_result/";

////patterns statistics
//string pattern_stat_folder_path = "../data_sets/aruba/patterns_statistics_result/";
//string logger_file = "../data_sets/aruba/log/log.txt";

////recognition using discovered clustered (based on similarity distance)
//string cluster_rec_path = "../data_sets/aruba/cluster_recognition/";

////recognition using a trained model
//string model_rec_path = "../data_sets/aruba/model_recognition/";

////evaluation
//string evaluation_path = "../data_sets/aruba/evaluation/";

///**
// * @brief runWithinDayPattern
// */
//void runWithinDayPattern()
//{
//  WithinDayClustering* withinDayClustering = new WithinDayClustering(home_setup_file,
//                                                                     sensors_data_file,
//                                                                     preprocessed_data,
//                                                                     time_window_config,
//                                                                     with_day_folder_path,
//                                                                     is_data_preprocessed);
//  withinDayClustering->run();
//  withinDayClustering->destroy();

//}

///**
// * @brief runBetweenDayPattern
// */
//void runBetweenDayPattern()
//{
//  BetweenDayClustering* betweenDayClustering = new BetweenDayClustering(with_day_folder_path,
//                                                                        betweem_day_folder_path,
//                                                                        pattern_stat_folder_path,
//                                                                        home_setup_file,
//                                                                        time_window_config,
//                                                                        success);

//  if(!success)
//    {
//      betweenDayClustering->destroy();
//      std::exit(0);

//    }

//  betweenDayClustering->run();

//  betweenDayClustering->destroy();
//}

///**
// * @brief runHierachalCluster
// */
//void runHierachalCluster()
//{

//  HierarchalAgglomerativeClustering* hierarchalAgglomerativeClustering = new HierarchalAgglomerativeClustering(betweem_day_folder_path,
//                                                                                                               hierachal_cluster_folder_path,
//                                                                                                               home_setup_file,
//                                                                                                               time_window_config,
//                                                                                                               success);

//  if(!success)
//    {
//      hierarchalAgglomerativeClustering->destroy();
//      std::exit(0);
//    }

//  hierarchalAgglomerativeClustering->run();


//  hierarchalAgglomerativeClustering->destroy();
//}

//void runClusterRecognition()
//{

//  ClusterRecognition* clusterRecognition = new ClusterRecognition(hierachal_cluster_folder_path,
//                                                                  config_path,
//                                                                  cluster_rec_path,
//                                                                  with_day_folder_path,
//                                                                  home_setup_file,
//                                                                  time_window_config,
//                                                                  success);

//  if(!success)
//    {
//      clusterRecognition->destroy();
//      std::exit(0);
//    }

//  clusterRecognition->run();

//  clusterRecognition->destroy();
//}

//void runModelRecognition()
//{

//  ModelRecognition* modelRecognition = new ModelRecognition(hierachal_cluster_folder_path,
//                                                            config_path,
//                                                            model_rec_path,
//                                                            with_day_folder_path,
//                                                            home_setup_file,
//                                                            time_window_config,
//                                                            success);

//  if(!success)
//    {
//      modelRecognition->destroy();
//      std::exit(0);
//    }

//  modelRecognition->run();

//  modelRecognition->destroy();


//}

///**
// * @brief runEvaluation
// */
//void  runEvaluation()
//{
//  Evaluation* evaluation = new Evaluation(evaluation_path,
//                                          config_path,
//                                          cluster_rec_path,
//                                          model_rec_path,
//                                          home_setup_file,
//                                          time_window_config,
//                                          success);

//  if(!success)
//    {
//      evaluation->destroy();
//    }

//  evaluation->run();

//  evaluation->destroy();
//}


//int main(int argc, char ** argv)
//{
//  int start_s=clock();
//  // the code you wish to time goes here

//  logging::configure({ {"type", "file"}, {"file_name", logger_file.c_str()} });
//  //runWithinDayPattern();
//  runBetweenDayPattern();
//  //runHierachalCluster();
//  //runClusterRecognition();
//  //runModelRecognition();
//  //runEvaluation();

//  int stop_s=clock();
//  stringstream message;

//  message<< fixed;
//  message << "time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << endl;
//  logging::INFO(message.str());
//  cout<<message.str()<<endl;

////  /std::system("systemctl suspend");
//  //string test="../result/2010-12-26/locations_duration.txt";

//  //HeatMap* hp = new HeatMap(test,home);
//  //hp->drawMostOccuppiedTimeIntervalHeatMap();

//}

// C++ implementation of Radix Sort
#include<iostream>
#include <algorithm>
#include<vector>
using namespace std;

// A utility function to get maximum value in arr[]
int getMax(int arr[], int n)
{
    int mx = arr[0];
    for (int i = 1; i < n; i++)
        if (arr[i] > mx)
            mx = arr[i];
    return mx;
}

// A function to do counting sort of arr[] according to
// the digit represented by exp.
void countSort(int arr[], string names [],int n, int exp)
{
    int output[n]; // output array
    string output_names[n];

    int i, count[10] = {0};

    // Store count of occurrences in count[]
    for (i = 0; i < n; i++)
        count[ (arr[i]/exp)%10 ]++;

    // Change count[i] so that count[i] now contains actual
    //  position of this digit in output[]
    for (i = 1; i < 10; i++)
        count[i] += count[i - 1];

    // Build the output array
    for (i = n - 1; i >= 0; i--)
    {
        output[count[ (arr[i]/exp)%10 ] - 1] = arr[i];
        output_names[count[ (arr[i]/exp)%10 ] - 1] = names[i];
        count[ (arr[i]/exp)%10 ]--;
    }

    // Copy the output array to arr[], so that arr[] now
    // contains sorted numbers according to current digit
      cout<<"exp:"<<exp<<endl;

    for (i = 0; i < n; i++)
      {
        cout<<"arr:"<<arr[i]<<"\t"<<"names:"<<names[i]<<endl;
        cout<<"names:"<<names[i]<<"\t"<<"output_names:"<<output_names[i]<<endl;
        arr[i] = output[i];
        names[i] = output_names[i];
      }
    cout<<endl;
}

void countSort_vec(vector<int>& arr, vector<string>& names ,int exp)
{
  int n = arr.size();
    vector<int> output(n,0); // output array
    vector<string> output_names(n,"");

    int i, count[10] = {0};

    // Store count of occurrences in count[]
    for (i = 0; i < n; i++)
        count[ (arr[i]/exp)%10 ]++;

    // Change count[i] so that count[i] now contains actual
    //  position of this digit in output[]
    for (i = 1; i < 10; i++)
        count[i] += count[i - 1];

    // Build the output array
    for (i = n - 1; i >= 0; i--)
    {
        output[count[ (arr[i]/exp)%10 ] - 1] = arr[i];
        output_names[count[ (arr[i]/exp)%10 ] - 1] = names[i];
        count[ (arr[i]/exp)%10 ]--;
    }

    // Copy the output array to arr[], so that arr[] now
    // contains sorted numbers according to current digit
      cout<<"exp:"<<exp<<endl;

    for (i = 0; i < n; i++)
      {
        cout<<"arr:"<<arr[i]<<"\t"<<"names:"<<names[i]<<endl;
        cout<<"names:"<<names[i]<<"\t"<<"output_names:"<<output_names[i]<<endl;
        arr[i] = output[i];
        names[i] = output_names[i];
      }
    cout<<endl;
}
// The main function to that sorts arr[] of size n using
// Radix Sort
void radixsort(int arr[],string names [], int n)
{
    // Find the maximum number to know number of digits
    int m = getMax(arr, n);

    // Do counting sort for every digit. Note that instead
    // of passing digit number, exp is passed. exp is 10^i
    // where i is current digit number
    for (int exp = 1; m/exp > 0; exp *= 10)
        countSort(arr, names,n, exp);
}

void radixsort_vec(vector<int>& arr,vector<string>& names)
{
    // Find the maximum number to know number of digits
    int m = *std::max_element(std::begin(arr), std::end(arr));


    // Do counting sort for every digit. Note that instead
    // of passing digit number, exp is passed. exp is 10^i
    // where i is current digit number
    for (int exp = 1; m/exp > 0; exp *= 10)
        countSort_vec(arr, names,exp);
}

// A utility function to print an array
void print(int arr[], int n)
{
    for (int i = 0; i < n; i++)
        cout << arr[i] << " ";
}

// Driver program to test above functions
int main()
{
    int arr[] = {170, 45, 75, 90, 802, 24, 2, 66};
    vector<int> vec ;
    vec.push_back(170);
    vec.push_back(45);
    vec.push_back(75);
    vec.push_back(90);
    vec.push_back(802);
    vec.push_back(24);
    vec.push_back(2);
    vec.push_back(66);

    vec.push_back(170);
    vec.push_back(45);
    vec.push_back(75);
    vec.push_back(90);
    vec.push_back(802);
    vec.push_back(24);
    vec.push_back(2);
    vec.push_back(66);

    vec.push_back(170);
    vec.push_back(45);
    vec.push_back(75);
    vec.push_back(90);
    vec.push_back(802);
    vec.push_back(24);
    vec.push_back(2);
    vec.push_back(66);

    vec.push_back(170);
    vec.push_back(45);
    vec.push_back(75);
    vec.push_back(90);
    vec.push_back(802);
    vec.push_back(24);
    vec.push_back(2);
    vec.push_back(66);

    vector<string> vec_names ;
    vec_names.push_back("g");
    vec_names.push_back("c");
    vec_names.push_back("e");
    vec_names.push_back("f");
    vec_names.push_back("h");
    vec_names.push_back("b");
    vec_names.push_back("a");
    vec_names.push_back("d");

    vec_names.push_back("g");
    vec_names.push_back("c");
    vec_names.push_back("e");
    vec_names.push_back("f");
    vec_names.push_back("h");
    vec_names.push_back("b");
    vec_names.push_back("a");
    vec_names.push_back("d");

    vec_names.push_back("g");
    vec_names.push_back("c");
    vec_names.push_back("e");
    vec_names.push_back("f");
    vec_names.push_back("h");
    vec_names.push_back("b");
    vec_names.push_back("a");
    vec_names.push_back("d");

    vec_names.push_back("g");
    vec_names.push_back("c");
    vec_names.push_back("e");
    vec_names.push_back("f");
    vec_names.push_back("h");
    vec_names.push_back("b");
    vec_names.push_back("a");
    vec_names.push_back("d");

//    string names [] = {"g","c","e","f","h","b","a","d"};
//    int n = sizeof(arr)/sizeof(arr[0]);
    radixsort_vec(vec,vec_names);

    for(int i = 0; i<vec.size();i++)
      cout<<vec[i]<<" ";

    cout<<endl;


    for(int i = 1;i<vec_names.size();i++)
      {
       if(vec[i-1] == vec[i])
          {
            cout<<"same:"<<vec_names[i-1]<<endl;
          }
       else
         {
           cout<<"diff:"<<vec_names[i-1]<<endl;
         }


      }

    return 0;
}
