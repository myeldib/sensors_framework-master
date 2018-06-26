#ifndef COMPAREMEASURES_H
#define COMPAREMEASURES_H
#include "MeasureReader.h"
#include "EMD.h";

class CompareMeasures
{
private:
    MeasureReader mr;
    vector<int> correspond_slot;
    vector<vector<double> > global_min;

private:
    vector<double> normalizeRow(vector<double>& row);
    double findMin(vector<double>& combined_measures,int& slot);
    void prepareMeasures();
    void prepareMeasures2();
    void writeEMDTable(vector< vector<double> >& emd_table,string day_1,string day_2);
    void writeLoATable(vector< vector<double> >& loa_table,string day_1,string day_2);
    void writeLoMTable(vector< vector<double> >& lom_table,string day_1,string day_2);
    void writeCombinedMeasuresTable(vector< vector<double> >& combined_measures_table,string day_1,string day_2);
    void writeGlobalMin(string path);
    double getMax(vector<vector< double> > table);
    string createEMDFolder(string day);
    string createLoAFolder(string day);
    string createLoMFolder(string day);
    string createCombinedMeasuresFolder(string day);
    vector<vector <double> > compareLoA(vector<vector<string> > rid_1, vector<vector<string> > rid_2,vector<int> loa_1,vector<int> loa_2);
    vector<vector <double> > compareLoM(vector<vector<string> > rid_1, vector<vector<string> > rid_2,vector<vector<int> > lom_1,vector<vector<int> >lom_2);
    vector<vector <double> > compareHeatMaps(vector<vector<string> > rid_1, vector<vector<string> > rid_2,vector<string> & heatmap_files_1,vector<string> & heatmap_files_2);
    vector<vector <double> > compareHeatMaps2(vector<vector<string> > rid_1, vector<vector<string> > rid_2,vector<string> & heatmap_files_1,vector<string> & heatmap_files_2);
    vector<vector <double> > combineMeasures(vector<vector <double> >& emd_table, vector<vector <double> >& loa_table,vector<vector <double> >& lom_table);
    vector<string> readMat(string path);
public:
    CompareMeasures();
    void run();
};

#endif // COMPAREMEASURES_H
