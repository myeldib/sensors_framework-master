#ifndef CLUSTERPATTERNS_H
#define CLUSTERPATTERNS_H
#include<math.h>
#include<numeric>
#include <algorithm>
#include<FeatureContainer.h>
#include "SensorReader.h"
# define M_PI           3.14159265358979323846  /* pi */
using namespace std;
class ClusterPatterns
{

public:
    ClusterPatterns(FeatureContrainer* fc,vector<string> activity_names,vector<int> activity_indexes);
    void run();
    FeatureContrainer* getClusteredPatterns(){return fc;}
    void setClusterPattern(FeatureContrainer* fc);
    ~ClusterPatterns()
    {

    }

private:
    float computeTimeSimilarity(int& start_time1,int& start_time2);
    float computeStructureSimilarity(vector<int>& xvec, vector<int>& yvec);
    float computeDurationSimilarity(vector<int>& dur1,vector<int>& dur2);
    vector<vector<float> > computeProximityMatrix(vector<int>& start_times,vector<vector<int> >& durations, vector<vector<int> >& structure_sensors);
    void prepareMeasures(vector<int>& start_times,vector<vector<int> >& durations, vector<vector<int> >& structure_sensors,FeatureContrainer* fc);
    void computeHierarchalAgglomerativeClustering(FeatureContrainer* fc);
    FeatureContrainer* mergePatterns(int p1,int p2,FeatureContrainer* fc,bool which_index, int& new_pos,vector<vector<float> >& proximity_matrix);
    void generateChiSquareFeatures(FeatureContrainer* fc);
    void findMostCommonActivityPattern(FeatureContrainer* fc);
    FeatureContrainer* computeFeature(FeatureContrainer* dataset_feature);
    void displayDiscoveredPatternInfo(FeatureContrainer* fc);
    void updateProximityMatrix(vector<int>& start_times,vector<vector<int> >& durations, vector<vector<int> >& structure_sensors,vector<vector<float> >& proximity_matrix,int pos2,int new_pos);
    vector<float> getMaxProximityPositions(vector<vector<float> >& proximity_matrix);

private:

    FeatureContrainer* fc;
    float clust_thre;
    bool verbose;
    vector<string> activity_names;
    vector<int> activity_indexes;

};

#endif // CLUSTERPATTERNS_H
