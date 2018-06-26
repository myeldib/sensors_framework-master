#include<similarity-measure.h>
using namespace std;
using namespace cv;
/**
 * @brief SimilarityMeasure::SimilarityMeasure
 */
SimilarityMeasure::SimilarityMeasure()
{
    COUT<<"SimilarityMeasure"<<endl;
    logging::INFO("SimilarityMeasure");
}
/**
 * @brief SimilarityMeasure::~SimilarityMeasure
 */
SimilarityMeasure::~SimilarityMeasure()
{
    COUT<<"~SimilarityMeasure"<<endl;
    logging::INFO("~SimilarityMeasure");
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
 * @brief SimilarityMeasure::normalize
 * @param arr
 * @return
 */
vector<float> SimilarityMeasure::normalize(vector<int> arr)
{
    COUT<<"normalize"<<endl;
    logging::INFO("normalize");

    vector<float> normalized_arr (arr.size(), 0.0);

    auto max = max_element(begin(arr), end(arr));
    auto min = min_element(begin(arr), end(arr));

    float denominator = 1.0*(*max-*min);

    for(int i = 0; i<arr.size(); i++)
    {
        float val = 1.0*(arr[i] - *min) / denominator;
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
