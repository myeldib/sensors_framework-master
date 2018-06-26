#ifndef EMD_H
#define EMD_H
#include <iostream>
#include<opencv/cv.h>
#include<opencv/cxcore.h>
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

namespace deprecated
{

class EarthMoverDistance
{
public:
    EarthMoverDistance(vector<int> mat1,vector<int> mat2);
    float computeEMD();
private:
    Mat computeSignature(vector< int >heatmap_mat);
private:
    vector<int> mat1,mat2;

};
}

#endif // EMD_H
