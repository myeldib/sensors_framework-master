#include<ClusterPatterns.h>


ClusterPatterns::ClusterPatterns(FeatureContrainer* fc,vector<string> activity_names,vector<int> activity_indexes)
{

    setClusterPattern(fc);

    this->clust_thre=0.90;
    this->verbose=false;
    this->activity_names=activity_names;
    this->activity_indexes=activity_indexes;


}
void ClusterPatterns::setClusterPattern(FeatureContrainer *fc)
{
    this->fc=fc;
    vector<vector<int> > chi_square_features=this->fc->getProcessedFeatures()[0];
    vector<vector<int> >new_chi_square_features;
    vector<vector<vector<int> > > new__processed_features;
    for(int i=0;i<chi_square_features.size();i++)
    {
        vector<int> feature=chi_square_features[i];
        for(int j=0;j<feature.size();j++)
        {
            if(feature[j]==1)
                feature[j]=0;
            if(feature[j]==2)
                feature[j]=1;
        }
        new_chi_square_features.push_back(feature);
    }

    new__processed_features.push_back(new_chi_square_features);
    this->fc->setProcessedFeatures(new__processed_features);
}

void ClusterPatterns::run()
{
    computeHierarchalAgglomerativeClustering(fc);
}
FeatureContrainer* ClusterPatterns::mergePatterns(int pos1,int pos2,FeatureContrainer* fc,bool which_index,int& new_pos,vector<vector<float> >& proximity_matrix)
{
    cout<<"mergePatterns"<<endl;

    vector<int> discovered_patterns=fc->getDiscoveredPatterns();
    vector<int> sequence_pattern=fc->getSequencePatterns();
    vector<int> start_times=fc->getPatternDayParts();



    for(int i=0;i<sequence_pattern.size();i++)
    {
        if(sequence_pattern[i]==discovered_patterns[pos2])
            sequence_pattern[i]=discovered_patterns[pos1];

    }


    int val=discovered_patterns[pos1];
    int old_pos=pos1;

    discovered_patterns.erase(discovered_patterns.begin() + pos2);
    start_times.erase(start_times.begin() + pos2);
    proximity_matrix.erase(proximity_matrix.begin() + pos2);

    fc->setDiscoveredPatterns(discovered_patterns);
    fc->setSequencePatterns(sequence_pattern);
    fc->setPatternDayParts(start_times);


    new_pos = find(discovered_patterns.begin(), discovered_patterns.end(), val) - discovered_patterns.begin();

    if(discovered_patterns[old_pos]!=discovered_patterns[new_pos])
        new_pos=new_pos+1;


    fc=computeFeature(fc);

    cout<<discovered_patterns.size()<<"\t"<<proximity_matrix.size()<<"\t"<<start_times.size()<<endl;

    return fc;
}



void ClusterPatterns::updateProximityMatrix(vector<int> &start_times, vector<vector<int> > &durations, vector<vector<int> > &structure_sensors, vector<vector<float> > &proximity_matrix,int pos2,int new_pos)
{

    cout<<"updateProximityMatrix"<<endl;



    int start_time1=start_times[new_pos];
    vector<int> dur1=durations[new_pos];
    vector<int> structure_sensor1=structure_sensors[new_pos];
    bool is_first=false;


    for(int i=0;i<start_times.size();i++)
    {


        float final_similarity=0.0;
        int start_time2=start_times[i];
        vector<int> dur2=durations[i];
        vector<int> structure_sensor2=structure_sensors[i];


        //update each row with merged index
        vector<float> prox_elem2=proximity_matrix[i];


        //update each column in merged_index row
        vector<float> prox_elem1=proximity_matrix[new_pos];

        if(!is_first)
        {
            prox_elem1.erase(prox_elem1.begin()+pos2);
            is_first=true;

        }


        float tmp_time_similarity=computeTimeSimilarity(start_time1,start_time2);

        float tmp_duration_similarity=computeDurationSimilarity(dur1,dur2);

        float tmp_structure_sensor_similarity=0.0;


        if(structure_sensor1.size()!=0 && structure_sensor2.size()!=0)
        {

            tmp_structure_sensor_similarity=computeStructureSimilarity(structure_sensor1,structure_sensor2);

            final_similarity=(tmp_time_similarity+tmp_duration_similarity+tmp_structure_sensor_similarity)/3;
        }
        else
            final_similarity=(tmp_time_similarity+tmp_duration_similarity)/2;


        if(new_pos!=i)
            prox_elem2.erase(prox_elem2.begin()+pos2);

        if(prox_elem1.size()!=prox_elem2.size())
            cout<<"a7a"<<"\t"<<prox_elem1.size()<<"\t"<<prox_elem2.size()<<endl;


        prox_elem2[new_pos]=final_similarity;
        proximity_matrix[i]=prox_elem2;


        prox_elem1[i]=final_similarity;
        proximity_matrix[new_pos]=prox_elem1;


    }
}


FeatureContrainer* ClusterPatterns::computeFeature(FeatureContrainer* fc)
{
    fc->clearProcessedFeatures();

    //process features to create chi_sqaure, ANOVA features
    generateChiSquareFeatures(fc);
    findMostCommonActivityPattern(fc);

    return fc;

}


void ClusterPatterns::findMostCommonActivityPattern(FeatureContrainer* fc)
{
    //cout<<"findMostCommonActivityPattern"<<endl;
    vector<int> activity_labels=fc->getActivityLabels();
    vector<int>  discovered_patterns=fc->getDiscoveredPatterns();
    vector<int>  fine_grain_seq=fc->getSequencePatterns();

    //default is "-", no activity assigned or labeled
    vector<int> most_common_act(discovered_patterns.size(),14);
    for (int j=0; j<discovered_patterns.size();j++)
    {
        vector<int> tmp_activity_label(activity_indexes.size(),0);

        for(int i=0;i<fine_grain_seq.size();i++)
        {
            if(discovered_patterns[j]==fine_grain_seq[i])
            {
                tmp_activity_label[activity_labels[i]]++;

            }
        }
        fc->insertProcessedActivityLabels(tmp_activity_label);

        auto biggest = std::max_element(std::begin(tmp_activity_label), std::end(tmp_activity_label));
        int pos=std::distance(std::begin(tmp_activity_label), biggest);

        most_common_act[j]=pos;

    }

    fc->setMostCommonActivityPattern(most_common_act);

}
void ClusterPatterns::generateChiSquareFeatures(FeatureContrainer* fc)
{
    //cout<<"generateChiSquareFeatures"<<endl;
    vector<vector<int> > f_day_sensor_ellapsed_time=fc->getSensorEllapsedTime();
    vector<int>  discovered_patterns=fc->getDiscoveredPatterns();
    vector<int>  fine_grain_seq=fc->getSequencePatterns();



    vector<vector<int> > sum_pattern_ellapsed_time;
    vector<int> patterns_len;


    for (int j=0; j<discovered_patterns.size();j++)
    {
        int count=0;

        vector<int> sum_sensor_ellapsed_time(f_day_sensor_ellapsed_time[0].size(),0);

        for(int i=0;i<fine_grain_seq.size();i++)
        {
            if(discovered_patterns[j]==fine_grain_seq[i])
            {
                vector<int> day_sensor_m_level=f_day_sensor_ellapsed_time[i];
                count++;

                for(int c=0;c<day_sensor_m_level.size();c++)
                {

                    sum_sensor_ellapsed_time[c]+=(1.0*day_sensor_m_level[c]);
                }
            }

        }
        patterns_len.push_back(count);

        for(int c=0;c<sum_sensor_ellapsed_time.size();c++)
            sum_sensor_ellapsed_time[c]= ceil((sum_sensor_ellapsed_time[c]*1.0)/(count*1.0));


        for(int c=0;c<sum_sensor_ellapsed_time.size();c++)
        {
            if(sum_sensor_ellapsed_time[c]==1)
                sum_sensor_ellapsed_time[c]=2;

            if(sum_sensor_ellapsed_time[c]==0)
                sum_sensor_ellapsed_time[c]=1;

        }



        sum_pattern_ellapsed_time.push_back(sum_sensor_ellapsed_time);


    }



    //fc->insertProcessedFeature(sum_pattern_ellapsed_time);
    vector<vector<vector<int> > > tmp;

    tmp.push_back(sum_pattern_ellapsed_time);

    fc->setProcessedFeatures(tmp);
    fc->setPatternsLen(patterns_len);

}

vector<float> ClusterPatterns::getMaxProximityPositions(vector<vector<float> > &proximity_matrix)
{
    vector<int> src_index,target_index;
    vector<float> max_values,cluster_indexes;

    // if(verbose)
    cout<<"getMaxProximityPositions"<<endl;

    for(int i=0;i<proximity_matrix.size();i++)
    {
        vector<float> prox_elem=proximity_matrix[i];

        auto biggest = std::max_element(std::begin(prox_elem), std::end(prox_elem));
        int pos=std::distance(std::begin(prox_elem), biggest);

        if(i==pos)
        {
            prox_elem[pos]=0;
            biggest = std::max_element(std::begin(prox_elem), std::end(prox_elem));
            pos=std::distance(std::begin(prox_elem), biggest);
        }

        max_values.push_back(*biggest);
        src_index.push_back(i);
        target_index.push_back(pos);
    }

    auto biggest = std::max_element(std::begin(max_values), std::end(max_values));
    int pos=std::distance(std::begin(max_values), biggest);



    cluster_indexes.push_back(src_index[pos]);
    cluster_indexes.push_back(target_index[pos]);
    cluster_indexes.push_back(*biggest);

    return cluster_indexes;

}

void ClusterPatterns::computeHierarchalAgglomerativeClustering(FeatureContrainer* merged_fc)
{

    cout<<"computeHierarchalAgglomerativeClustering"<<endl;

    float sim=0.0;
    bool is_first=true;
    int pos1,pos2,new_pos;
    vector<vector<float> > proximity_matrix;


    do{
        vector<int> start_times;
        vector<vector<int> >structure_sensors,durations;

        prepareMeasures(  start_times,durations, structure_sensors,merged_fc);

        if(is_first)
        {
            proximity_matrix= computeProximityMatrix(  start_times,durations, structure_sensors);
            is_first=false;
        }
        else
            updateProximityMatrix(start_times,durations, structure_sensors,proximity_matrix,pos2,new_pos);


        vector<float> cluster_index=getMaxProximityPositions(proximity_matrix);


        pos1=cluster_index[0];
        pos2=cluster_index[1];
        sim=cluster_index[2];


        if(sim>this->clust_thre)
        {
            //merge
            if(verbose)
                cout<<"Clustering:"<<merged_fc->getDiscoveredPatterns()[pos1]<<"\t"<<merged_fc->getDiscoveredPatterns()[pos2]<<endl;

            if(merged_fc->getPatternsLen()[pos1]>=merged_fc->getPatternsLen()[pos2])
            {
                cout<<"pos1,pos2"<<endl;
                merged_fc=mergePatterns(pos1,pos2,merged_fc,false,new_pos,proximity_matrix);
            }
            else
            {
                cout<<"pos2,pos1"<<endl;
                merged_fc=mergePatterns(pos2,pos1,merged_fc,true,new_pos,proximity_matrix);
                pos2=pos1;//dirty workaround for updateProximity fucntion
            }
        }


        cout<<sim<<endl;
        cout<<"================================"<<endl;

    } while(sim>this->clust_thre);

    fc=merged_fc;

    // displayDiscoveredPatternInfo(merged_fc);

}

vector<vector<float> > ClusterPatterns::computeProximityMatrix(vector<int> &start_times, vector<vector<int> >&durations, vector<vector<int> > &structure_sensors)
{
    vector<vector<float> >proximity_matrix;


    // if(verbose)
    cout<<"computeProximityMatrix"<<endl;

    for(int i=0;i<start_times.size();i++)
    {
        int start_time1=start_times[i];
        vector<int> dur1=durations[i];
        vector<int> structure_sensor1=structure_sensors[i];

        vector<float> elem;

        for(int j=0;j<start_times.size();j++)
        {

            float final_similarity=0.0;
            int start_time2=start_times[j];
            vector<int> dur2=durations[j];
            vector<int> structure_sensor2=structure_sensors[j];


            float tmp_time_similarity=computeTimeSimilarity(start_time1,start_time2);
            float tmp_duration_similarity=computeDurationSimilarity(dur1,dur2);
            float tmp_structure_sensor_similarity=0.0;


            if(structure_sensor1.size()!=0 && structure_sensor2.size()!=0)
            {

                tmp_structure_sensor_similarity=computeStructureSimilarity(structure_sensor1,structure_sensor2);
                final_similarity=(tmp_time_similarity+tmp_duration_similarity+tmp_structure_sensor_similarity)/3;
            }
            else
                final_similarity=(tmp_time_similarity+tmp_duration_similarity)/2;


            elem.push_back(final_similarity);

            // cout<<tmp_time_similarity<<"\t"<<tmp_duration_similarity<<"\t"<<tmp_structure_sensor_similarity<<endl;


        }
        //cout<<"========================================"<<endl;

        proximity_matrix.push_back(elem);

    }

    return proximity_matrix;

}

float ClusterPatterns::computeStructureSimilarity(vector<int>& xvec, vector<int>& yvec)
{

    float result=0.0;
    //    if(verbose)
    //        cout<<"computeStructureSimilarity"<<endl;


    vector<int> intersection_sensors,union_sensors;

    std::set_intersection (xvec.begin(), xvec.end(), yvec.begin(), yvec.end(), back_inserter(intersection_sensors));
    std::set_union(xvec.begin(), xvec.end(), yvec.begin(), yvec.end(), back_inserter(union_sensors));


    result=(1.0*intersection_sensors.size())/(1.0*union_sensors.size());

    return result;

}

float ClusterPatterns::computeDurationSimilarity(vector<int> &dur1,vector<int>& dur2)
{
    //    if(verbose)
    //        cout<<"computeDurationSimilarity"<<endl;
    float sum1=0,sum2=0,max=0;
    int sum=0;

    //    if(dur1.size()==0 && dur2.size()==0)
    //    {
    //        max=1;

    //    }
    //    else
    // {
    sum1=  std::accumulate(dur1.begin(), dur1.end(), 0);
    sum2=  std::accumulate(dur2.begin(), dur2.end(), 0);
    max=sum1+sum2;

    for(int i=0;i<dur1.size();i++)
        sum+=abs(dur1[i]-dur2[i]);
    // }

    float dur_sim=1-((1.0*sum)/(max*1.0));

    return dur_sim;
}

float ClusterPatterns::computeTimeSimilarity(int& start_time1,int& start_time2)
{
    //    if(verbose)
    //        cout<<"computeTimeSimilarity"<<endl;

    float radian1=(((1.0*start_time1/(1.0*24))*360*1.0)*M_PI)/(1.0*180);
    float radian2=(((1.0*start_time2/(1.0*24))*360*1.0)*M_PI)/(1.0*180);

    float time_sim=1-(abs(radian2-radian1)/(2*M_PI));

    return time_sim;
}

void ClusterPatterns::prepareMeasures( vector<int>& start_times,vector<vector<int> >& durations, vector<vector<int> >& structure_sensors,FeatureContrainer* fc)
{
    // if(verbose)
    cout<<"prepareMeasures"<<endl;

    start_times=fc->getPatternDayParts();


    vector<vector<int> > final_discovered_patterns=fc->getProcessedFeatures()[0];
    durations=fc->getProcessedFeatures()[0];

    for(int i=0;i<final_discovered_patterns.size();i++)
    {
        vector<int> sensors,core_sensors;
        vector<int> patterns=final_discovered_patterns[i];


        for(int j=0;j<final_discovered_patterns[i].size();j++)
            sensors.push_back(j);

        Common::sortFrequentPatterns(patterns,sensors);

        for(int j=0;j<sensors.size();j++)
        {
            //  if(patterns[j]>0)
            core_sensors.push_back(sensors[j]);
        }

        structure_sensors.push_back(core_sensors);

        //  cout<<endl;

    }
    final_discovered_patterns.shrink_to_fit();
    final_discovered_patterns.clear();

}

