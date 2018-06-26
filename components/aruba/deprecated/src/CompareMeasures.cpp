#include "CompareMeasures.h"

CompareMeasures::CompareMeasures()
{

}

vector<double> CompareMeasures::normalizeRow(vector<double>& row)
{
    //  cout<<"normalizeRow"<<endl;
    vector<double> normalize_row;

    double max=*std::max_element(row.begin(),row.end());
    int count_segment=0;




    for(int m=0; m<row.size();m++)
    {
        if(row[m]==mr.getTimeSlot())
            count_segment++;
        else
            break;
    }

    if(max!=mr.getConstantCost() && count_segment!=mr.getSegmentCount())
    {
        if(max==0)
            max=1;

        for(int w=0;w<row.size();w++)
        {
            if(row[w]==mr.getConstantCost())
                row[w]=max*2;
        }

        double min=*std::min_element(row.begin(),row.end());
        max=*std::max_element(row.begin(),row.end());


        if(max!=min)
        {
            for(int i=0;i<row.size();i++)
            {
                double result=(row[i]-min)/(max-min);
                normalize_row.push_back(result);

            }
        }
        else{
            for(int i=0;i<row.size();i++)
                normalize_row.push_back(1);
        }
    }
    else
    {
        for(int i=0;i<row.size();i++)
            normalize_row.push_back(1);
    }
    return normalize_row;
}

vector<vector <double> > CompareMeasures::combineMeasures(vector<vector<double> >& emd_table, vector<vector<double> >& loa_table, vector<vector<double> >& lom_table)
{
    cout<<"combineMeasures"<<endl;
    vector<vector <double> >combined_measures_table;
    double max_emd=getMax(emd_table);
    double max_lom=getMax(lom_table);
    double max_loa=getMax(loa_table);

    vector<double> local_min;
    for(int i=0;i<emd_table.size();i++)
    {
        vector<double> combined_measures_row;
        vector<double> emd_row=emd_table[i];
        vector<double> loa_row=loa_table[i];
        vector<double> lom_row=lom_table[i];


        for(int j=0;j<emd_row.size();j++)
            combined_measures_row.push_back((emd_row[j]/max_emd)+(lom_row[j]/max_lom)+(loa_row[j]/max_loa));


        local_min.push_back(findMin(combined_measures_row,i));

        combined_measures_table.push_back(combined_measures_row);
    }

    this->global_min.push_back(local_min);

    return combined_measures_table;
}

vector<vector <double> >CompareMeasures::compareLoM(vector<vector<string> > rid_1, vector<vector<string> > rid_2,vector<vector<int> > lom_1,vector<vector<int> >lom_2)
{
    cout<<"compareLoM"<<endl;
    vector<vector <double> > lom_table;

    for(int i=0; i<lom_1.size();i++)
    {
        vector<int> line_lom_1=lom_1[i];
        vector<string> line_rid_1=rid_1[i];

        vector<double > lom_row;

        for(int j=0; j<lom_2.size();j++)
        {
            vector<int> line_lom_2=lom_2[j];
            vector<string> line_rid_2=rid_2[j];


            int result=0;


            for(int m=0;m<line_rid_1.size();m++)
            {
                string rid_val1=line_rid_1[m];
                bool found_match=false;
                for(int n=0;n<line_rid_2.size();n++)
                {
                    string rid_val2=line_rid_2[n];
                    cout<<rid_val1<<" "<<rid_val2<<endl;
                    if(rid_val1==rid_val2)
                    {
                        result+=abs(line_lom_1[m]-line_lom_2[n]);
                        found_match=true;
                    }
                }

                if(!found_match)
                    result+=line_lom_1[m];
            }

//            if(line_rid_1==line_rid_2)
//            {
//                for(int m=0; m<line_rid_1.size();m++)
//                {
//                    result+=abs(line_lom_1[m]-line_lom_2[m]);

//                }

//            }
//            else
//                result=mr.getConstantCost();

            lom_row.push_back(result);

        }

        //        vector<double> normalize_row=normalizeRow(lom_row);
        findMin(lom_row,i);

        lom_table.push_back(lom_row);

    }
    return lom_table;
}

vector<vector <double> > CompareMeasures::compareLoA(vector<vector<string> > rid_1, vector<vector<string> > rid_2,vector<int> loa_1, vector<int> loa_2)
{

    cout<<"compareLoA"<<endl;

    vector<vector <double> > loa_table;

    for(int i=0; i<rid_1.size();i++)
    {
        vector<string> line_rid_1=rid_1[i];
        vector<double > loa_row;


        for(int j=0; j<rid_2.size();j++)
        {
            vector<string> line_rid_2=rid_2[j];

            //   cout<<(i)<<" ";
            //   cout<<(j)<<" ";


            //           if(line_rid_1==line_rid_2)
            {

                int loa_val_1=loa_1[i];
                int loa_val_2=loa_2[j];
                double result;

                if(loa_val_1==0 && loa_val_2==0)
                    result=mr.getConstantCost();
                else
                    result=abs(1.0*loa_val_1-1.0*loa_val_2)/abs(1.0*loa_val_1+1.0*loa_val_2);

                loa_row.push_back(result);

            }
            //            else
            //                loa_row.push_back(mr.getConstantCost());

        }


        //        vector<double> nomalize_row=normalizeRow(loa_row);
        findMin(loa_row,i);
        loa_table.push_back(loa_row);

    }

    return loa_table;
}

vector<vector <double> >  CompareMeasures::compareHeatMaps(vector<vector<string> > rid_1, vector<vector<string> > rid_2,vector<string> & heatmap_files_1,vector<string> & heatmap_files_2)
{
    cout<<"compareHeatMaps"<<endl;
    vector<vector <double> > emd_table;

    for(int i=0; i<rid_1.size();i++)
    {
        vector<string> line_rid_1=rid_1[i];
        vector<double > emd_row;

        cout<<(i)<<" ";
        //cout<<(j)<<" ";
        cout<<endl;

        for(int j=0; j<rid_2.size();j++)
        {
            vector<string> line_rid_2=rid_2[j];



            if(line_rid_1.size()==line_rid_2.size())
            {
                if(line_rid_1==line_rid_2)
                {


                    Mat img1=imread(heatmap_files_1[i]);
                    Mat img2=imread(heatmap_files_2[j]);

                    EarthMoverDistance emd(img1,img2);
                    emd.run();
                    double emd_val=emd.getEMDVal();
                    emd_row.push_back(emd_val);

                    img1.release();
                    img2.release();

                    //              cout<<emd_val<<" ";
                    //             cout<<"same room activity"<<endl;

                }
                else
                {
                    emd_row.push_back(mr.getConstantCost());
                    //             cout<<mr.getConstantCost()<<" ";
                    //            cout<<"different room activity"<<endl;
                }

            }
            else
            {
                emd_row.push_back(mr.getConstantCost());
                //          cout<<mr.getConstantCost()<<" ";
                //          cout<<"not same size"<<endl;
            }
        }
        findMin(emd_row,i);
        emd_table.push_back((emd_row));
        //emd_table.push_back(normalizeRow(emd_row));
    }

    return emd_table;
}

vector< string> CompareMeasures::readMat(string path)
{

    vector< string > heatmap_mat;
    std::ifstream infile( path.c_str());

    while (infile)
    {
        string line;


        if (!getline( infile, line )) break;


        heatmap_mat.push_back(line);


    }
    return heatmap_mat;

}
vector<vector <double> >  CompareMeasures::compareHeatMaps2(vector<vector<string> > rid_1, vector<vector<string> > rid_2,vector<string> & heatmap_files_1,vector<string> & heatmap_files_2)
{
    cout<<"compareHeatMaps"<<endl;

    vector<vector <double> > emd_table;

    for(int i=0; i<rid_1.size();i++)
    {
        vector<string> line_rid_1=rid_1[i];
        vector<double > emd_row;

        for(int j=0; j<rid_2.size();j++)
        {
            vector<string> line_rid_2=rid_2[j];

            // cout<<(i)<<" ";
            // cout<<(j)<<" ";

            //            if(line_rid_1.size()==line_rid_2.size())

            if ( !(std::find(line_rid_1.begin(), line_rid_1.end(), "-1") != line_rid_1.end() ) && !(std::find(line_rid_2.begin(), line_rid_2.end(), "-1") != line_rid_2.end()))
            {
                //                if(line_rid_1==line_rid_2)
                {

                    vector<string> mat1=readMat(heatmap_files_1[i]);
                    vector<string> mat2=readMat(heatmap_files_2[j]);

                    // same room(s) activity
                    if(mat1.size() != 0 && mat2.size()!=0)
                    {

                        EarthMoverDistance emd(mat1,mat2);
                        emd.run2();
                        float emd_val=emd.getEMDVal2();

                        emd_row.push_back(emd_val);
                    }
                    //one of time slots has no activity
                    else
                        emd_row.push_back(mr.getConstantCost());
                }
                //  different room activity
                //                else
                //                    emd_row.push_back(mr.getConstantCost());

            }
            // not same size
            else
                emd_row.push_back(mr.getConstantCost());

        }


        //        vector<double> normalize_row=normalizeRow(emd_row);
        findMin(emd_row,i);
        emd_table.push_back(emd_row);

    }

    return emd_table;
}

double CompareMeasures::getMax(vector<vector< double> > table)
{
    vector<double> max_elem;
    double max=0.0;

    for(int i=0;i<table.size();i++)
    {
        max= *std::max_element(table[i].begin(),table[i].end());
        max_elem.push_back(max);
    }
    max=*std::max_element(max_elem.begin(),max_elem.end());
    return max;
}

void CompareMeasures::writeEMDTable(vector<vector<double> >& emd_table,string day_1,string day_2)
{

    cout<<"writeEMDTable"<<endl;

    stringstream path,path_corr_slot;
    path<<createEMDFolder(day_1)<<day_1<<"_"<<day_2;
    path_corr_slot<<path.str()<<"_slot.txt";
    path<<"_h.txt";


    std::ofstream file_emd(path.str().c_str());

    for(int i=0;i<emd_table.size();i++)
    {
        vector<double> emd_row=emd_table[i];

        for(int j=0; j<emd_row.size();j++)
            file_emd<<emd_row[j]<<" ";

        file_emd<<"\n";
    }

    file_emd.close();

    std::ofstream file_slot(path_corr_slot.str().c_str());


    for(int c=0;c<correspond_slot.size();c++)
        file_slot<<correspond_slot[c]<<endl;

    file_emd.close();
    file_slot.close();
    correspond_slot.clear();
}
void CompareMeasures::writeGlobalMin(string day)
{

    stringstream full_path;
    //full_path<<mr.getBaseDirectory()<<day<<"/Measures/"<<day<<"_gm.txt";
    full_path<<mr.getBaseDirectory()<<"gm.txt";

    std::ofstream file_gm(full_path.str().c_str(), std::ios_base::app);

    //    for(int i=0;i<this->global_min.size();i++)
    //    {
    //        vector<double> gm_row=this->global_min[i];

    //        for(int j=0;j<gm_row.size();j++)
    //        file_gm<<gm_row[j]<<" ";

    //        file_gm<<endl;

    //    }


    for(int i=0;i<mr.getSegmentCount();i++)
    {
        vector<double> row2;
        for(int j=0;j<global_min.size();j++)
        {
            vector<double> row=global_min[j];
            row2.push_back(row[i]);
        }

        int pos=(std::min_element(row2.begin(), row2.end()) - row2.begin());
        double min_val=row2[pos];
        file_gm<<min_val<<" ";

    }

    file_gm<<endl;
    file_gm.close();

    global_min.clear();
}

void CompareMeasures::writeLoATable(vector<vector<double> >& loa_table,string day_1,string day_2)
{

    cout<<"writeLoATable"<<endl;

    stringstream path,path_corr_slot;
    path<<createLoAFolder(day_1)<<day_1<<"_"<<day_2;
    path_corr_slot<<path.str()<<"_slot.txt";
    path<<"_loa.txt";

    std::ofstream file_loa(path.str().c_str());

    for(int i=0;i<loa_table.size();i++)
    {
        vector<double> loa_row=loa_table[i];

        for(int j=0; j<loa_row.size();j++)
            file_loa<<loa_row[j]<<" ";

        file_loa<<"\n";
    }

    file_loa.close();

    std::ofstream file_slot(path_corr_slot.str().c_str());


    for(int c=0;c<correspond_slot.size();c++)
        file_slot<<correspond_slot[c]<<endl;


    file_slot.close();
    correspond_slot.clear();
}

void CompareMeasures::writeLoMTable(vector<vector<double> >& lom_table,string day_1,string day_2)
{

    cout<<"writeLoMTable"<<endl;


    stringstream path,path_corr_slot;
    path<<createLoMFolder(day_1)<<day_1<<"_"<<day_2;
    path_corr_slot<<path.str()<<"_slot.txt";
    path<<"_lom.txt";


    std::ofstream file_lom(path.str().c_str());

    for(int i=0;i<lom_table.size();i++)
    {
        vector<double> lom_row=lom_table[i];

        for(int j=0; j<lom_row.size();j++)
            file_lom<<lom_row[j]<<" ";

        file_lom<<"\n";
    }

    file_lom.close();


    std::ofstream file_slot(path_corr_slot.str().c_str());


    for(int c=0;c<correspond_slot.size();c++)
        file_slot<<correspond_slot[c]<<endl;


    file_slot.close();
    correspond_slot.clear();
}

void CompareMeasures::writeCombinedMeasuresTable(vector<vector<double> > &combined_measures_table, string day_1, string day_2)
{
    cout<<"writeCombinedMeasuresTable"<<endl;

    stringstream path,path_corr_slot;
    path<<createCombinedMeasuresFolder(day_1)<<day_1<<"_"<<day_2;
    path_corr_slot<<path.str()<<"_slot.txt";
    path<<"_comb.txt";

    std::ofstream file_comb(path.str().c_str());

    for(int i=0;i<combined_measures_table.size();i++)
    {
        vector<double> combined_measures_row=combined_measures_table[i];

        for(int j=0; j<combined_measures_row.size();j++)
            file_comb<<combined_measures_row[j]<<" ";

        file_comb<<"\n";
    }

    std::ofstream file_slot(path_corr_slot.str().c_str());


    for(int c=0;c<correspond_slot.size();c++)
        file_slot<<correspond_slot[c]<<endl;

    file_comb.close();
    file_slot.close();
    correspond_slot.clear();
}

string CompareMeasures::createEMDFolder(string day)
{
    cout<<"createEMDFolder"<<endl;

    stringstream full_path;
    full_path<<mr.getBaseDirectory()<<day<<"/Measures/";
    Common::createFolder(full_path.str());

    full_path<<"EMD/";
    Common::createFolder(full_path.str());

    return full_path.str();
}

string CompareMeasures::createLoAFolder(string day)
{
    cout<<"createLoAFolder"<<endl;

    stringstream full_path;
    full_path<<mr.getBaseDirectory()<<day<<"/Measures/";
    Common::createFolder(full_path.str());

    full_path<<"LoA/";
    Common::createFolder(full_path.str());

    return full_path.str();
}

string CompareMeasures::createLoMFolder(string day)
{
    cout<<"createLoMFolder"<<endl;
    stringstream full_path;
    full_path<<mr.getBaseDirectory()<<day<<"/Measures/";
    Common::createFolder(full_path.str());

    full_path<<"LoM/";
    Common::createFolder(full_path.str());

    return full_path.str();
}

string CompareMeasures::createCombinedMeasuresFolder(string day)
{
    cout<<"createCombinedMeasuresFolder"<<endl;
    stringstream full_path;
    full_path<<mr.getBaseDirectory()<<day<<"/Measures/";
    Common::createFolder(full_path.str());

    full_path<<"Combined/";
    Common::createFolder(full_path.str());

    return full_path.str();
}

double CompareMeasures::findMin(vector<double>& combined_measures,int& slot)
{
    //cout<<"findMin slot:"<<slot<<endl;
    int pos=(std::min_element(combined_measures.begin(), combined_measures.end()) - combined_measures.begin());
    double min_val=combined_measures[pos];

    vector<int> nearest_min_pos;
    vector<int> diff;

    for(int i=0;i<combined_measures.size();i++)
    {


        if(min_val==combined_measures[i])
        {

            diff.push_back(abs(i-slot));
            nearest_min_pos.push_back(i);
        }

    }


    int pos2=(std::min_element(diff.begin(), diff.end()) - diff.begin());
    correspond_slot.push_back(nearest_min_pos[pos2]);
    return min_val;

}

void CompareMeasures::prepareMeasures()
{

    vector< vector<string> > days_heatmap_files= mr.getDaysHeatMapFiles();
    vector< vector < vector <string> > > days_rids_files=mr.getDaysRIdsFiles();
    vector< vector < vector <int> > > days_lom_files=mr.getDaysLoMFiles();
    vector< vector<int> > days_loa_files=mr.getDaysLoAFiles();
    vector<string> days_names=mr.getDaysNames();

    //days folder
    for(int i=0; i<days_heatmap_files.size();i++)
    {
        vector<string> heatmap_files_1=days_heatmap_files[i]; //first element in pair
        vector< vector <string> > file_rids_1=days_rids_files[i];
        vector< vector <int> > file_lom_1=days_lom_files[i];
        vector<int> file_loa_1=days_loa_files[i];
        string day_1=days_names[i];

        for(int j=0; j<days_heatmap_files.size();j++)
            //for(int j=0; j<days_heatmap_files.size();j++)
        {
            //   if(i!=j)
            {
                vector<string> heatmap_files_2=days_heatmap_files[j]; //second element in pair
                vector< vector <string> > file_rids_2=days_rids_files[j];
                vector< vector <int> > file_lom_2=days_lom_files[j];
                vector<int> file_loa_2=days_loa_files[j];
                string day_2=days_names[j];

                cout<<i<<" "<<j<<" "<<day_1<<" "<<day_2<<endl;

                vector<vector <double> >emd_table=compareHeatMaps(file_rids_1,file_rids_2,heatmap_files_1,heatmap_files_2);
                writeEMDTable(emd_table,day_1,day_2);

                vector<vector <double> >loa_table=compareLoA(file_rids_1,file_rids_2,file_loa_1,file_loa_2);
                writeLoATable(loa_table,day_1,day_2);

                vector<vector <double> >lom_table=compareLoM(file_rids_1,file_rids_2,file_lom_1,file_lom_2);
                writeLoMTable(lom_table,day_1,day_2);

                // vector<vector <double> >combined_measures_table=combineMeasures(emd_table,loa_table,lom_table);
                // writeCombinedMeasuresTable(combined_measures_table,day_1,day_2);
            }
        }

    }
}

void CompareMeasures::prepareMeasures2()
{

    vector< vector<string> > days_heatmap_files= mr.getDaysHeatMapMatFiles();
    vector< vector < vector <string> > > days_rids_files=mr.getDaysRIdsFiles();
    vector< vector < vector <int> > > days_lom_files=mr.getDaysLoMFiles();
    vector< vector<int> > days_loa_files=mr.getDaysLoAFiles();
    vector<string> days_names=mr.getDaysNames();

    //days folder
    for(int i=0; i<days_heatmap_files.size();i++)
    {
        vector<string> heatmap_files_1=days_heatmap_files[i]; //first element in pair
        vector< vector <string> > file_rids_1=days_rids_files[i];
        vector< vector <int> > file_lom_1=days_lom_files[i];
        vector<int> file_loa_1=days_loa_files[i];
        string day_1=days_names[i];

        //  for(int j=i+1; j<days_heatmap_files.size();j++)
        for(int j=0; j<days_heatmap_files.size();j++)
        {
            if(i!=j)
            {
                vector<string> heatmap_files_2=days_heatmap_files[j]; //second element in pair
                vector< vector <string> > file_rids_2=days_rids_files[j];
                vector< vector <int> > file_lom_2=days_lom_files[j];
                vector<int> file_loa_2=days_loa_files[j];
                string day_2=days_names[j];

                cout<<i<<" "<<j<<" "<<day_1<<" "<<day_2<<endl;

                vector<vector <double> >emd_table=compareHeatMaps2(file_rids_1,file_rids_2,heatmap_files_1,heatmap_files_2);
                writeEMDTable(emd_table,day_1,day_2);

                vector<vector <double> >loa_table=compareLoA(file_rids_1,file_rids_2,file_loa_1,file_loa_2);
                writeLoATable(loa_table,day_1,day_2);

                vector<vector <double> >lom_table=compareLoM(file_rids_1,file_rids_2,file_lom_1,file_lom_2);
                writeLoMTable(lom_table,day_1,day_2);

                vector<vector <double> >combined_measures_table=combineMeasures(emd_table,loa_table,lom_table);
                writeCombinedMeasuresTable(combined_measures_table,day_1,day_2);
            }
        }
        writeGlobalMin(day_1);
    }
}

void CompareMeasures::run()
{
    mr.run();
    prepareMeasures2();
}
