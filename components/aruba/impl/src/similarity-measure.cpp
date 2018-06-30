#include<similarity-measure.h>
using namespace std;
using namespace cv;
/**
 * @brief SimilarityMeasure::SimilarityMeasure
 */
SimilarityMeasure::SimilarityMeasure(string home_setup_file, string time_window_config)
{
  COUT<<"SimilarityMeasure"<<endl;
  logging::INFO("SimilarityMeasure");

  home_ =  new Home(home_setup_file,time_window_config);
  home_->readHomeSetup();

  normalized_emd_val_=0.0;
  has_normalized_emd_ = false;
}
/**
 * @brief SimilarityMeasure::~SimilarityMeasure
 */
SimilarityMeasure::~SimilarityMeasure()
{
  COUT<<"~SimilarityMeasure"<<endl;
  logging::INFO("~SimilarityMeasure");
  if(home_)
    {
      delete home_;
    }

}
/**
 * @brief SimilarityMeasure::computeSignature
 * @param arr
 * @return
 */
cv::Mat SimilarityMeasure::computeSignature(vector<int> arr)
{
  COUT<<"computeSignature"<<endl;
  logging::INFO("computeSignature");

  vector<float> normalized_arr = normalize(arr);

  int num_rows =  normalized_arr.size();

  cv::Mat signature(num_rows, 2, CV_32FC1);

  for(int i=0; i<num_rows;i++)
    {
      signature.at<float>(i,0)=normalized_arr[i]*1.0;
      signature.at<float>(i,1)=i*1.0;
    }


  return signature;
}

/**
 * @brief SimilarityMeasure::normalizeEMD_
 * @param emd_val
 * @param vec
 * @param normalized_emd
 */
void SimilarityMeasure::normalizeEMD_(float &emd_val, vector<float>& vec,float &normalized_emd)
{

  if(!has_normalized_emd_)
    {
      vector<float> tmp_vec1(vec.size(),0);
      vector<float> tmp_vec2(vec.size(),0);

      tmp_vec1[0] = home_->getSensorTimeInterval()*1000;
      tmp_vec2[tmp_vec2.size()-1] = home_->getSensorTimeInterval()*1000;

      compute1DEMD(tmp_vec1,tmp_vec2,normalized_emd_val_);

      has_normalized_emd_= true;
    }

  normalized_emd = 1-(emd_val/normalized_emd_val_);

}
/**
 * @brief SimilarityMeasure::normalize
 * @param arr
 * @return
 */
vector<float> SimilarityMeasure::normalize(vector<int> arr)
{
  COUT<<"normalize"<<endl;
  logging::INFO("normalize");

  vector<float> normalized_arr (arr.size(), 0.0);

  auto max_val = max_element(std::begin(arr), std::end(arr));
  auto min_val = min_element(std::begin(arr), std::end(arr));

  float denominator = 1.0*(*max_val-*min_val);

  for(int i = 0; i<arr.size(); i++)
    {
      float val = 1.0*(arr[i] - *min_val) / denominator;
      //float val = 1.0*(arr[i] )/60000.0;
      normalized_arr.push_back(val);
    }

  return normalized_arr;
}

/**
 * @brief SimilarityMeasure::computeEMDUsingOpenCV
 * @param arr1
 * @param arr2
 * @param dist
 */
void SimilarityMeasure::computeEMDUsingOpenCV(std::vector<int> arr1,std::vector<int> arr2,float& dist)
{
  COUT<<"computeEMDUsingOpenCV"<<endl;
  logging::INFO("computeEMDUsingOpenCV");

  Mat sig1 = computeSignature(arr1);
  Mat sig2 = computeSignature(arr2);

  dist = cv::EMD(sig1,sig2,CV_DIST_L2);

}

/**
 * @brief SimilarityMeasure::computeManhattanDistance
 * @param arr1
 * @param arr2
 * @param dist
 */
void SimilarityMeasure::computeManhattanDistance(std::vector<int> arr1,std::vector<int> arr2,float& dist)
{
  COUT<<"computeManhattanDistance"<<endl;
  logging::INFO("computeManhattanDistance");

  dist = 0.0;
  for(int i =0;i<arr1.size();i++)
    {
      dist+=abs(arr1[i]-arr2[i]);
    }
}

/**
 * @brief SimilarityMeasure::computeEuclideanDistance
 * @param arr1
 * @param arr2
 * @param dist
 */
void SimilarityMeasure::computeEuclideanDistance(std::vector<int> arr1,std::vector<int> arr2,float& dist)
{
  COUT<<"computeEuclideanDistance"<<endl;
  logging::INFO("computeEuclideanDistance");
  dist = 0.0;

  for(int i =0;i<arr1.size();i++)
    {
      dist+=pow((arr1[i]-arr2[i]),2);
    }

  dist = std::sqrt(dist);
}

/**
 * @brief SimilarityMeasure::compute1DEMD
 * @param arr1
 * @param arr2
 * @param dist
 */
void SimilarityMeasure::compute1DEMD(std::vector<float> arr1,std::vector<float> arr2,float& dist)
{
  COUT<<"compute1DEMD"<<endl;
  //logging::INFO("compute1DEMD");

  float tmp_sum = 0;
  float total_sum = 0;
  for(int i =0;i<arr1.size();i++)
    {
      tmp_sum = (1.0*arr1[i]+tmp_sum)-arr2[i]*1.0;
      total_sum+=tmp_sum;
      //logging::INFO(std::to_string(i)+","+std::to_string(arr1[i])+","+std::to_string(arr2[i])+","+std::to_string(total_sum));
    }

  dist =  abs(total_sum);
}

/**
 * @brief SimilarityMeasure::computeNormalized1DEMD
 * @param arr1
 * @param arr2
 * @param dist
 */
void SimilarityMeasure::computeNormalized1DEMD(std::vector<float> arr1, std::vector<float> arr2, float &dist)
{
  float normalized_emd = 0.0;

  compute1DEMD(arr1,arr2,dist);

  normalizeEMD_(dist,arr1,normalized_emd);

  dist = normalized_emd;

}
/**
 * @brief SimilarityMeasure::computeDurationSimilarity
 * @param avg_sensor_durations1
 * @param avg_sensor_durations2
 * @param result
 */
void SimilarityMeasure::computeDurationSimilarity(const vector<float> &avg_sensor_durations1, const vector<float> &avg_sensor_durations2, float &result)
{
  COUT<<"computeDurationSimilarity"<<endl;
  //logging::INFO("computeDurationSimilarity_");

  vector<float> duration_similarity;
  for(int i =0; i<avg_sensor_durations1.size();i++)
    {
      float max=0.0;
      if(avg_sensor_durations1[i]>avg_sensor_durations2[i])
        {
          max = avg_sensor_durations1[i];
        }
      else
        {
          max= avg_sensor_durations2[i];
        }

      if(avg_sensor_durations1[i] !=0 || avg_sensor_durations2[i]!=0)
        {
          //            logging::INFO(std::to_string(avg_sensor_durations1[i])+
          //                          "\t"+
          //                          std::to_string(avg_sensor_durations2[i])+
          //                          "\t"+
          //                          std::to_string(max));
          float sim=1-abs(avg_sensor_durations1[i] - avg_sensor_durations2[i])/max;
          duration_similarity.push_back(sim);
        }
    }


  auto maxSimilarityValue = std::max_element(std::begin(duration_similarity), std::end(duration_similarity));
  int maxSimilarityIndex=std::distance(std::begin(duration_similarity), maxSimilarityValue);


  result = duration_similarity[maxSimilarityIndex];

  //logging::INFO("dur_sim:"+std::to_string(result));

}

/**
 * @brief SimilarityMeasure::computeTimeSimilarity
 * @param time_index1
 * @param time_index2
 * @param result
 */
void SimilarityMeasure::computeTimeSimilarity(int time_index1, int time_index2, float &result)
{
  //logging::INFO("computeTimeSimilarity");

  float radian1=(((1.0*time_index1/(1.0*24))*360*1.0)*M_PI)/(1.0*180);
  float radian2=(((1.0*time_index2/(1.0*24))*360*1.0)*M_PI)/(1.0*180);

  result=1-(abs(radian2-radian1)/(2*M_PI));

  //logging::INFO("time_sim:"+std::to_string(result));
}

/**
 * @brief SimilarityMeasure::computeJaccardSimilarity
 * @param xvec
 * @param yvec
 * @param result
 */
void SimilarityMeasure::computeJaccardSimilarity(const vector<int> &xvec, const vector<int> &yvec, float &result)
{
  //logging::INFO("computeJaccardSimilarity");

  vector<int> intersection_sensors,union_sensors;

  std::set_intersection (xvec.begin(), xvec.end(), yvec.begin(), yvec.end(), back_inserter(intersection_sensors));
  std::set_union(xvec.begin(), xvec.end(), yvec.begin(), yvec.end(), back_inserter(union_sensors));

  //logging::INFO(std::to_string(intersection_sensors.size())+"\t"+std::to_string(union_sensors.size()));

  result=(1.0*intersection_sensors.size())/(1.0*union_sensors.size());

  //logging::INFO("jaccard_sim:"+std::to_string(result));
}
