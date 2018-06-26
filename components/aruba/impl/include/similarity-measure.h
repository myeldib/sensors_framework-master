#ifndef EMD_H
#define EMD_H
#include <vector>
#include <iostream>
#include<opencv/cv.h>
#include<opencv/cxcore.h>
#include "opencv2/imgproc/imgproc.hpp"
#include<logger.h>
#include<constants.h>

class SimilarityMeasure {
public:
    SimilarityMeasure();
    ~SimilarityMeasure();
    void computeEMDUsingOpenCV(std::vector<int> arr1,std::vector<int> arr2,float& dist);
    void computeManhattanDistance(std::vector<int> arr1,std::vector<int> arr2,float& dist);
    void computeEuclideanDistance(std::vector<int> arr1,std::vector<int> arr2,float& dist);
    void compute1DEMD(std::vector<float> arr1,std::vector<float> arr2,float& dist);
private:
    cv::Mat computeSignature(std::vector<int> arr);
    std::vector<float> normalize(std::vector<int> arr);
};

#endif // EMD_H
