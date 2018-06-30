#ifndef EMD_H
#define EMD_H
#include <vector>
#include <iostream>
#include<opencv/cv.h>
#include<opencv/cxcore.h>
#include "opencv2/imgproc/imgproc.hpp"
#include<logger.h>
#include<home.h>
#include<constants.h>

class SimilarityMeasure {
public:
    SimilarityMeasure(string home_setup_file, string time_window_config);
    ~SimilarityMeasure();
    void computeEMDUsingOpenCV(std::vector<int> arr1,std::vector<int> arr2,float& dist);
    void computeManhattanDistance(std::vector<int> arr1,std::vector<int> arr2,float& dist);
    void computeEuclideanDistance(std::vector<int> arr1,std::vector<int> arr2,float& dist);
    void compute1DEMD(std::vector<float> arr1,std::vector<float> arr2,float& dist);
    void computeNormalized1DEMD(std::vector<float> arr1,std::vector<float> arr2,float& dist);
    void computeDurationSimilarity(const vector<float> &avg_sensor_durations1, const vector<float> &avg_sensor_durations2, float &result);
    void computeTimeSimilarity(int time_index1, int time_index2, float &result);
    void computeJaccardSimilarity(const vector<int> &xvec, const vector<int> &yvec, float &result);
private:
    void normalizeEMD_(float& emd_val,vector<float>& vec, float& normalized_emd);
private:
    Home* home_;
    cv::Mat computeSignature(std::vector<int> arr);
    std::vector<float> normalize(std::vector<int> arr);

    bool has_normalized_emd_;
    float normalized_emd_val_;
};

#endif // EMD_H
