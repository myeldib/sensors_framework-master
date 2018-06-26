#include "EMD.h"
using namespace deprecated;

EarthMoverDistance::EarthMoverDistance(vector<int> mat1,vector<int> mat2)
{
    this->mat1=mat1;
    this->mat2=mat2;
}


Mat EarthMoverDistance::computeSignature(vector< int >heatmap_mat)
{

    int num_rows =  heatmap_mat.size();

    Mat signature(num_rows, 2, CV_32FC1);

    auto biggest1 = std::max_element(std::begin(heatmap_mat), std::end(heatmap_mat));


    for(int i=0; i<num_rows;i++)
    {


        signature.at<float>(i,0)=(1.0*heatmap_mat[i])/(*biggest1*1.0);

        signature.at<float>(i,1)=(i+1);

    }


    return signature;
}

float EarthMoverDistance::computeEMD()
{

    Mat sig1=computeSignature(this->mat1);
    Mat sig2=computeSignature(this->mat2);

    float emd = cv::EMD(sig1,sig2,CV_DIST_L2);

    //cout<<"emd:"<<emd<<endl;

    return emd;
}



