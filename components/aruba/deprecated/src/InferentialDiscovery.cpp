#include<InferentialDiscovery.h>


InferentialDiscovery::InferentialDiscovery()
{
    base_dir="../data/days/";
    f_sr = new SensorReader();
    f_sr->run();
    num_of_days=f_sr->getNumOfDays();
    f_day_parts=f_sr->getNumberOfDayParts();
    f_timeslot=f_sr->getTimeSlot();
    f_time_interval=f_sr->getTimeInterval();
    room_sybmols=f_sr->getRoomSymbols();
    days=f_sr->getDays();
    activity_names=f_sr->getActivityNames();
    activity_indexes=f_sr->getActivityIndexes();
    sensor_to_rooms=f_sr->getSensorToRooms();


    day_len=1440;
    this->verbose=false;

    delete f_sr;

}
vector<FeatureContrainer*> InferentialDiscovery::readFeatureFiles()
{
    cout<<"readFeatureFiles"<<endl;

    vector<FeatureContrainer*> dataset_features;
    for(int i=0;i<num_of_days; i++)
    {

        string day=days[i];
        vector<string> file_paths=preparePaths(day);

        FeatureContrainer* fc = new FeatureContrainer(file_paths);
        dataset_features.push_back(fc);

    }

    computeFeatures(dataset_features);

    return dataset_features;
}


vector<vector<int> > InferentialDiscovery::withinPatternsMerge(vector<vector<int> > &patterns_to_merge_all, vector<int>& fine_grain_seq,vector<int>& discovered_patterns,vector<int> & pattern_day_parts)
{
    //    if(verbose)
    //        cout<<"withinMergePatterns"<<endl;



    vector<bool> is_pattern_merge(discovered_patterns.size(),false);
    vector<int> new_discovered_patterns;
    vector<int> new_pattern_day_parts;

    //    if(verbose){
    //        for(int i=0;i<fine_grain_seq.size();i++)
    //            cout<<"*"<<fine_grain_seq[i]<<"* ";
    //        cout<<endl;

    //        for(int i=0;i<discovered_patterns.size();i++)
    //            cout<<discovered_patterns[i]<<" ";
    //        cout<<endl;

    //        for(int i=0;i<patterns_to_merge_all.size();i++)
    //        {
    //            vector<int>p=patterns_to_merge_all[i];

    //            for(int j=0;j<p.size();j++)
    //            {
    //                cout<<discovered_patterns[i]<<"\t"<<discovered_patterns[p[j]]<<endl;
    //            }
    //        }
    //    }



    for(int i=0;i<patterns_to_merge_all.size();i++)
    {
        vector<int> patterns_to_merge=patterns_to_merge_all[i];

        if(patterns_to_merge.size()>1 )
        {

            bool is_merged=false;
            for(int c=0;c<patterns_to_merge.size();c++)
            {

                for(int j=0;j<fine_grain_seq.size();j++)
                {

                    if(discovered_patterns[patterns_to_merge[c]]==fine_grain_seq[j]  && !is_pattern_merge[i])
                    {
                        fine_grain_seq[j]=discovered_patterns[i];
                        is_merged=true;
                    }

                }

            }

            if(is_merged)
            {
                for(int c=0;c<patterns_to_merge.size();c++)
                {
                    is_pattern_merge[patterns_to_merge[c]]=true;
                }

                new_discovered_patterns.push_back(discovered_patterns[i]);
                new_pattern_day_parts.push_back(pattern_day_parts[i]);
            }


        }
        else
        {
            new_discovered_patterns.push_back(discovered_patterns[i]);
            new_pattern_day_parts.push_back(pattern_day_parts[i]);
        }


    }

    //    if(verbose){
    //        for(int i=0;i<fine_grain_seq.size();i++)
    //            cout<<"*"<<fine_grain_seq[i]<<"* ";
    //        cout<<endl;

    //        for(int i=0;i<new_discovered_patterns.size();i++)
    //            cout<<new_discovered_patterns[i]<<" ";
    //        cout<<endl;


    //        cout<<discovered_patterns.size()<<"\t"<<new_discovered_patterns.size()<<endl;
    //    }

    vector<vector<int> > pattern_topics;
    pattern_topics.push_back(new_discovered_patterns);
    pattern_topics.push_back(new_pattern_day_parts);


    return pattern_topics;

}
vector<int> InferentialDiscovery::getNumberDiscoveredActivities(FeatureContrainer *merged_fc)
{
    cout<<"getNumberDiscoveredActivities"<<endl;

    vector<int> patterns_len=merged_fc->getPatternsLen();
    vector<bool> is_activity_discovered(activity_names.size(),false);


    vector<vector<int> > processed_activity_labels=merged_fc->getProcessedActivityLabels();



    int count_activities=0;
    int count_cluster=0;

    for(int i=0;i<patterns_len.size();i++)
    {
        vector<int> a=processed_activity_labels[i];

        a.pop_back();

        auto biggest = std::max_element(std::begin(a), std::end(a));
        int  pos=std::distance(std::begin(a), biggest);

        if(*biggest!=0)
        {
            is_activity_discovered[pos]=true;
            count_cluster++;
        }

    }

    for(int i=0;i<is_activity_discovered.size();i++)
    {
        if(is_activity_discovered[i])
        {
            count_activities++;
        }
    }

    vector<int> patterns_stat;
    patterns_stat.push_back(count_cluster);
    patterns_stat.push_back(count_activities);
    return patterns_stat;
}

//algorithms to cluster discovered patterns using chi_square independecne test, ANOVA
//first algorithm: withinPatternsChiSquareIndTest
//second algorithm: betweenPatternsChiSquareIndTest
void InferentialDiscovery::run()
{

    Common::createFolder("../data/data_mining_phase");
    std::ofstream dp_patterns_statistics_file("../data/data_mining_phase/patterns_statistics.txt");



    vector<FeatureContrainer*> dataset_features=readFeatureFiles();


    FeatureContrainer* merged_fc=dataset_features[0];


    for(int i=1;i<dataset_features.size();i++)
    {
        FeatureContrainer* fc=dataset_features[i];

        int dp_prunced_patterns=(merged_fc->getDiscoveredPatterns().size()+fc->getDiscoveredPatterns().size());

        merged_fc=betweenPatternSimilarityTest(i,merged_fc,fc);

        dp_prunced_patterns=dp_prunced_patterns-merged_fc->getDiscoveredPatterns().size();
        dp_patterns_statistics_file<<days[i]<<"\t"<<getNumberDiscoveredActivities(merged_fc)[1]<<"\t"<<merged_fc->getDiscoveredPatterns().size()<<"\t"<<dp_prunced_patterns<<endl;

    }


//    ClusterPatterns* cp1 = new ClusterPatterns(merged_fc,activity_names,activity_indexes);
//    cp1->run();
//    merged_fc=cp1->getClusteredPatterns();

    writeFeatureContainer(merged_fc);

}

void InferentialDiscovery::withinPatternSimilarityTest(FeatureContrainer* fc)
{
    //    if(verbose)
    //        cout<<"withinPatternSimilarityTest"<<endl;


    vector<int> discovered_patterns=fc->getDiscoveredPatterns();
    vector<int> sequence_patterns=fc->getSequencePatterns();

    vector<vector<int> >within_p_chi_square= computeChiSquareIndTest(fc,fc);
    vector<int> pattern_day_parts=fc->getPatternDayParts();
    vector<vector<int> >pattern_topics=withinPatternsMerge(within_p_chi_square,sequence_patterns,discovered_patterns,pattern_day_parts);

    fc->setDiscoveredPatterns(pattern_topics[0]);
    fc->setPatternDayParts(pattern_topics[1]);
    fc->setSequencePatterns(sequence_patterns);

    fc=computeFeature(fc);

}

void InferentialDiscovery::writeFeatureContainer(FeatureContrainer *merged_fc)
{
    vector<int> merged_discovered_patterns=merged_fc->getDiscoveredPatterns();
    vector<int> merged_pattern_times=merged_fc->getPatternDayParts();
    vector<int> merged_sequence_patterns=merged_fc->getSequencePatterns();
    vector<vector<int> > merged_sensor_ellapsed_time=merged_fc->getSensorEllapsedTime();
    vector<vector<int> > merged_chi_square_features=merged_fc->getProcessedFeatures()[0];
    vector<vector<int> > processed_activity_labels=merged_fc->getProcessedActivityLabels();
    vector<int> merged_most_common_activity_labels=merged_fc->getMostCommonActivityPattern();
    vector<int> patterns_len=merged_fc->getPatternsLen();
    vector<int> sequence_activity_labels=merged_fc->getActivityLabels();
    vector<int> sequence_time=merged_fc->getSequenceTimes();

    Common::createFolder("../data/final_phase");
    std::ofstream discovered_patterns_file("../data/final_phase/discovered_patterns.txt");
    std::ofstream activity_file("../data/final_phase/activity_patterns.txt");
    std::ofstream features_file("../data/final_phase/features.txt");

    vector<vector<int> >new_chi_square_features;

    for(int i=0;i<merged_chi_square_features.size();i++)
    {
        vector<int> feature=merged_chi_square_features[i];
        for(int j=0;j<feature.size();j++)
        {
            if(feature[j]==1)
                feature[j]=0;
            if(feature[j]==2)
                feature[j]=1;
        }
        new_chi_square_features.push_back(feature);
    }


    for(int j=0;j<merged_sensor_ellapsed_time.size();j++)
    {
        for(int i =0;i<merged_discovered_patterns.size();i++)
        {

            vector<int> processed_features=new_chi_square_features[i];
            if(merged_discovered_patterns[i]==merged_sequence_patterns[j])
            {
                vector<int> features=merged_sensor_ellapsed_time[j];

                // if(sequence_activity_labels[j]!=11)
                {
                    features_file<<sequence_activity_labels[j]<<"\t"<<merged_sequence_patterns[j]<<"\t"<<sequence_time[j]<<"\t";

                    for(int c=0;c<features.size();c++)
                    {
                        if(c==features.size()-1)
                            features_file<<processed_features[c]<<endl;
                        else
                            features_file<<processed_features[c]<<"\t";

                    }
                }
            }
        }
    }




    int sum_of_elems = std::accumulate(patterns_len.begin(), patterns_len.end(), 0);


    Common::sortPatternFeatures(patterns_len,merged_discovered_patterns,merged_most_common_activity_labels,new_chi_square_features,processed_activity_labels,merged_pattern_times);
    int count_clusters=0;

    for(int i=0;i<patterns_len.size();i++)
    {

        vector<int> p=new_chi_square_features[i];
        vector<int> a=processed_activity_labels[i];
        vector<int> a2=processed_activity_labels[i];


        auto biggest1 = std::max_element(std::begin(p), std::end(p));
        int pos1=std::distance(std::begin(p), biggest1);

        a.pop_back();


        auto biggest2 = std::max_element(std::begin(a), std::end(a));
        int pos2=std::distance(std::begin(a), biggest2);


        auto biggest3 = std::max_element(std::begin(a2), std::end(a2));
        int pos3=std::distance(std::begin(a2), biggest3);



        if(*biggest2!=0 )
        {
            if(pos1<10)
                discovered_patterns_file<<merged_discovered_patterns[i]<<"\t"<<activity_names[pos2]<<"\tM00"<<(pos1+1)<<"\t"<<*biggest1<<"\t"<<patterns_len[i]<<"\t"<<pos2<<" "<<endl;

            else
                discovered_patterns_file<<merged_discovered_patterns[i]<<"\t"<<activity_names[pos2]<<"\tM0"<<(pos1+1)<<"\t"<<*biggest1<<"\t"<<patterns_len[i]<<"\t"<<pos2<<" "<<endl;;

            for(int c=0;c<a.size();c++)
            {

                if(pos3!=c)
                    activity_file<<activity_names[c]<<":"<<a[c]<<"\t";
                else
                    activity_file<<"***"<<activity_names[c]<<":"<<a[c]<<"\t";
            }

            count_clusters++;
            activity_file<<endl;

        }
        else
        {
            if(pos1<10)
                discovered_patterns_file<<merged_discovered_patterns[i]<<"\t"<<activity_names[merged_most_common_activity_labels[i]]<<"\tM00"<<(pos1+1)<<"\t"<<*biggest1<<"\t"<<patterns_len[i]<<"\t"<<merged_most_common_activity_labels[i]<<" "<<endl;

            else
                discovered_patterns_file<<merged_discovered_patterns[i]<<"\t"<<activity_names[merged_most_common_activity_labels[i]]<<"\tM0"<<(pos1+1)<<"\t"<<*biggest1<<"\t"<<"\t"<<merged_most_common_activity_labels[i]<<" "<<endl;

            for(int c=0;c<a2.size();c++)
            {

                if(pos3!=c)
                    activity_file<<activity_names[c]<<":"<<a[c]<<"\t";
                else
                    activity_file<<"***"<<activity_names[c]<<":"<<a[c]<<"\t";
            }

            activity_file<<endl;

        }


    }


    cout<<"^^^End \t"<<patterns_len.size()<<"\t"<<sum_of_elems<<"\t"<<count_clusters<<endl;

}

void InferentialDiscovery::displayDiscoveredPatternInfo(FeatureContrainer* merged_fc)
{
    cout<<"displayDiscoveredPatternInfo"<<endl;
    vector<int> patterns_len=merged_fc->getPatternsLen();
    vector<vector<vector<int> > > merged_processed_features=merged_fc->getProcessedFeatures();
    vector<int> merged_discovered_patterns=merged_fc->getDiscoveredPatterns();
    vector<int> merged_most_common_activity_labels=merged_fc->getMostCommonActivityPattern();
    vector<vector<int> > merged_chi_square_features=merged_processed_features[0];

    vector<vector<int> > processed_activity_labels=merged_fc->getProcessedActivityLabels();
    vector<int> pattern_times=merged_fc->getPatternDayParts();

    int sum_of_elems = std::accumulate(patterns_len.begin(), patterns_len.end(), 0);


    Common::sortPatternFeatures(patterns_len,merged_discovered_patterns,merged_most_common_activity_labels,merged_chi_square_features,processed_activity_labels,pattern_times);
    int count_clusters=0;

    for(int i=0;i<patterns_len.size();i++)
    {
        float p_ratio=(1.0*patterns_len[i])/(sum_of_elems*1.0)*100;

        vector<int> p=merged_chi_square_features[i];
        vector<int> a=processed_activity_labels[i];

        auto biggest = std::max_element(std::begin(p), std::end(p));
        int pos=std::distance(std::begin(p), biggest);

        if(pos<10)
            cout<<merged_discovered_patterns[i]<<"\t"<<activity_names[merged_most_common_activity_labels[i]]<<"\tM00"<<(pos+1)<<":"<<*biggest<<"\t"<<patterns_len[i]<<" "<<endl;

        else
            cout<<merged_discovered_patterns[i]<<"\t"<<activity_names[merged_most_common_activity_labels[i]]<<"\tM0"<<(pos+1)<<":"<<*biggest<<"\t"<<patterns_len[i]<<" "<<endl;



        a.pop_back();
        int sum_of_a = std::accumulate(a.begin(), a.end(), 0);

        biggest = std::max_element(std::begin(a), std::end(a));
        pos=std::distance(std::begin(a), biggest);


        if(*biggest!=0 )
        {
            for(int c=0;c<a.size();c++)
            {
                float ratio_a=(1.0*a[c])/(1.0*sum_of_a);
                if(pos!=c)
                    cout<<activity_names[c]<<":"<<a[c]<<"\t";
                else
                    cout<<"***"<<activity_names[c]<<":"<<a[c]<<"\t";
            }
            count_clusters++;
            cout<<endl;
            cout<<"============"<<endl;
        }


    }


    cout<<endl;

    cout<<"^^^End \t"<<patterns_len.size()<<"\t"<<sum_of_elems<<"\t"<<count_clusters<<endl;


}

FeatureContrainer* InferentialDiscovery::betweenPatternSimilarityTest(int multi_ind,FeatureContrainer* fc1,FeatureContrainer* fc2)
{
    cout<<"betweenPatternSimilarityTest \t"<<multi_ind<<endl;

    FeatureContrainer* merged_fc = new FeatureContrainer();


    vector<int> discovered_patterns1=fc1->getDiscoveredPatterns();
    vector<int> sequence_patterns1=fc1->getSequencePatterns();
    vector<vector<int> > features1=fc1->getSensorEllapsedTime();
    vector<int> activity_labels1=fc1->getActivityLabels();


    vector<int> discovered_patterns2=fc2->getDiscoveredPatterns();
    vector<int> sequence_patterns2=fc2->getSequencePatterns();
    vector<vector<int> > features2=fc2->getSensorEllapsedTime();
    vector<int> activity_labels2=fc2->getActivityLabels();



    vector<int> pattern_day_parts1,pattern_day_parts2;
    vector<vector<int> >new_discovered_patterns;
    // pattern comparison
    if(discovered_patterns2>discovered_patterns1)
    {
        vector<vector<int> >within_p_chi_square= computeChiSquareIndTest(fc2,fc1);

        pattern_day_parts1=fc1->getPatternDayParts();
        pattern_day_parts2=fc2->getPatternDayParts();

        new_discovered_patterns=betweenMergePatterns(multi_ind,within_p_chi_square,discovered_patterns2,discovered_patterns1,sequence_patterns2,sequence_patterns1,pattern_day_parts2,pattern_day_parts1);

    }
    else
    {
        vector<vector<int> >within_p_chi_square= computeChiSquareIndTest(fc1,fc2);

        pattern_day_parts1=fc1->getPatternDayParts();
        pattern_day_parts2=fc2->getPatternDayParts();

        new_discovered_patterns=betweenMergePatterns(multi_ind,within_p_chi_square,discovered_patterns1,discovered_patterns2,sequence_patterns1,sequence_patterns2,pattern_day_parts1,pattern_day_parts2);


    }

    merged_fc=updateFeatures(activity_labels1,activity_labels2,new_discovered_patterns,discovered_patterns1,discovered_patterns2,sequence_patterns1,sequence_patterns2,features1,features2,pattern_day_parts1,pattern_day_parts2);
    merged_fc=computeFeature(merged_fc);


    delete fc1;
    delete fc2;


    return merged_fc;
}
FeatureContrainer* InferentialDiscovery::updateFeatures(vector<int>& activity_labels1,vector<int>& activity_labels2,vector< vector<int> >& new_discovered_topics, vector<int> &discovered_patterns1, vector<int> &discovered_patterns2, vector<int> &fine_grain_seq1, vector<int> &fine_grain_seq2,vector<vector<int> >& features1,vector<vector<int> >& features2,vector<int>& pattern_day_parts1,vector<int>& pattern_day_parts2)
{

    FeatureContrainer* merged_fc = new FeatureContrainer();
    vector<vector<int> > merged_features;
    vector<int> merged_sequence_patterns;
    vector<int> merged_discovered_patterns;
    vector<int> merged_activity_labels;
    vector<int> merged_pattern_day_parts;
    vector<int> patterns_len;
    vector<int> merged_sequence_times;

    vector<int> new_discovered_features=new_discovered_topics[0];
    vector<int> new_pattern_day_parts=new_discovered_topics[1];

    //   / if(verbose)
    cout<<fine_grain_seq1.size()<<"\t"<<fine_grain_seq2.size()<<"\t"<<features1.size()<<"\t"<<features2.size()<<"\t"<<discovered_patterns1.size()<<"\t"<<discovered_patterns2.size()<<"\t"<<new_discovered_features.size()<<"\t"<<new_pattern_day_parts.size()<<endl;

    //append common patterns from group 1 and 2
    for(int i=0;i<new_discovered_features.size();i++)
    {
        int pattern_len=0;
        for(int j=0;j<fine_grain_seq1.size();j++)
        {
            if(new_discovered_features[i]==fine_grain_seq1[j])
            {

                fine_grain_seq1[j]=new_discovered_features[i];
                pattern_len++;
            }
        }

        for(int j=0;j<fine_grain_seq2.size();j++)
        {
            if(new_discovered_features[i]==fine_grain_seq2[j])
            {
                fine_grain_seq2[j]=new_discovered_features[i];
                pattern_len++;
            }
        }

        merged_discovered_patterns.push_back(new_discovered_features[i]);
        merged_pattern_day_parts.push_back(new_pattern_day_parts[i]);
        patterns_len.push_back(pattern_len);
    }


    for(int i=0;i<fine_grain_seq1.size();i++)
    {
        merged_features.push_back(features1[i]);
        merged_activity_labels.push_back(activity_labels1[i]);
        merged_sequence_patterns.push_back(fine_grain_seq1[i]);

        for(int j=0;j<discovered_patterns1.size();j++)
        {
            if(fine_grain_seq1[i]==discovered_patterns1[j])
                merged_sequence_times.push_back(pattern_day_parts1[j]);
        }

    }

    for(int i=0;i<fine_grain_seq2.size();i++)
    {
        merged_features.push_back(features2[i]);
        merged_activity_labels.push_back(activity_labels2[i]);
        merged_sequence_patterns.push_back(fine_grain_seq2[i]);

        for(int j=0;j<discovered_patterns2.size();j++)
        {
            if(fine_grain_seq2[i]==discovered_patterns2[j])
                merged_sequence_times.push_back(pattern_day_parts2[j]);
        }

    }

    //append non-common patterns from group 1
    for(int i=0;i<discovered_patterns1.size();i++)
    {

        bool is_match=false;
        int pattern_len=0;
        for(int j=0;j<new_discovered_features.size();j++)
        {
            if(discovered_patterns1[i]==new_discovered_features[j])
            {
                is_match=true;

            }

        }

        if(!is_match)
        {

            for(int j=0;j<fine_grain_seq1.size();j++)
            {
                if(fine_grain_seq1[j]==discovered_patterns1[i])
                    pattern_len++;

            }
            merged_discovered_patterns.push_back(discovered_patterns1[i]);
            merged_pattern_day_parts.push_back(pattern_day_parts1[i]);
            patterns_len.push_back(pattern_len);

        }

    }

    //append non-common patterns from group 2

    for(int i=0;i<discovered_patterns2.size();i++)
    {

        bool is_match=false;
        int pattern_len=0;
        for(int j=0;j<new_discovered_features.size();j++)
        {
            if(discovered_patterns2[i]==new_discovered_features[j])
                is_match=true;

        }

        if(!is_match)
        {

            for(int j=0;j<fine_grain_seq2.size();j++)
            {
                if(fine_grain_seq2[j]==discovered_patterns2[i])
                    pattern_len++;

            }
            merged_discovered_patterns.push_back(discovered_patterns2[i]);
            merged_pattern_day_parts.push_back(pattern_day_parts2[i]);
            patterns_len.push_back(pattern_len);
        }

    }

    merged_fc->setDiscoveredPatterns(merged_discovered_patterns);
    merged_fc->setSequencePatterns(merged_sequence_patterns);
    merged_fc->setSensorEllapsedTime(merged_features);
    merged_fc->setActivityLabels(merged_activity_labels);
    merged_fc->setPatternsLen(patterns_len);
    merged_fc->setPatternDayParts(merged_pattern_day_parts);
    merged_fc->setSequenceTimes(merged_sequence_times);

    cout<<merged_sequence_times.size()<<"\t"<<merged_features.size()<<"\t"<<merged_sequence_patterns.size()<<endl;

    if(verbose)
        cout<<merged_sequence_patterns.size()<<"\t"<<merged_features.size()<<"\t"<<merged_discovered_patterns.size()<<"\t"<<patterns_len.size()<<endl;

    return merged_fc;
}

vector<vector<int> > InferentialDiscovery::betweenMergePatterns(int mult_ind,vector<vector<int> > &patterns_to_merge_all,vector<int>& discovered_patterns1,vector<int>& discovered_patterns2,vector<int>& fine_grain_seq1, vector<int>& fine_grain_seq2,vector<int>& patter_day_parts1,vector<int>& pattern_day_parts2)
{

    if(verbose)
        cout<<"betweenMergePatterns"<<endl;

    vector<bool> is_pattern_merge(discovered_patterns2.size(),false);
    vector<int> new_discovered_patterns,new_pattern_day_parts;

    int ind_uni_pattern=10000*mult_ind;
    int initial_index=10000*mult_ind;
    vector<int> exist_pattern;


    if(verbose){
        for(int j=0;j<fine_grain_seq2.size();j++)
            cout<<fine_grain_seq2[j]<<" ";
        cout<<endl;
    }

    //    for(int i=0;i<patterns_to_merge_all.size();i++)
    //    {
    //        vector<int> c=patterns_to_merge_all[i];
    //        for(int j=0;j<c.size();j++)
    //            cout<<c[j]<<",";
    //            cout<<endl;
    //    }


    for(int i=0;i<patterns_to_merge_all.size();i++)
    {
        vector<int>assosciated_patterns=patterns_to_merge_all[i];
        vector<bool> is_merged_before(assosciated_patterns.size(),false);
        bool is_merged=false;



        //check if pattern is already added
        for(int k=0;k<exist_pattern.size();k++)
        {
            for(int w=0;w<assosciated_patterns.size();w++)
            {
                if(discovered_patterns2[assosciated_patterns[w]]==exist_pattern[k])
                    is_merged_before[w]=true;
            }
        }

        //if not added then merge

        for(int j=0;j<assosciated_patterns.size();j++)
        {

            exist_pattern.push_back(discovered_patterns2[assosciated_patterns[j]]);

            //update sequence 1
            for(int m=0;m<fine_grain_seq1.size();m++)
            {
                if(assosciated_patterns[j]!=0 && !is_merged_before[j] && discovered_patterns1[i]==fine_grain_seq1[m])
                {
                    //cout<<fine_grain_seq1[m]<<"\t"<<ind_uni_pattern<<endl;
                    fine_grain_seq1[m]=ind_uni_pattern;

                }

            }

            //update sequence 2
            for(int m=0;m<fine_grain_seq2.size();m++)
            {
                if(assosciated_patterns[j]!=0 && !is_merged_before[j]&& discovered_patterns2[assosciated_patterns[j]]==fine_grain_seq2[m])
                {
                    //cout<<fine_grain_seq2[m]<<"\t"<<ind_uni_pattern<<endl;
                    fine_grain_seq2[m]=ind_uni_pattern;
                    is_merged=true;
                }

            }


            //cout<<discovered_patterns1[i]<<"\t"<<discovered_patterns2[assosciated_patterns[j]]<<endl;
            if(assosciated_patterns[j]!=0 && is_merged)
                discovered_patterns2[assosciated_patterns[j]]=ind_uni_pattern;

        }

        if(assosciated_patterns[0]!=0 && is_merged)
        {
            new_discovered_patterns.push_back(ind_uni_pattern);
            new_pattern_day_parts.push_back(patter_day_parts1[i]);
            discovered_patterns1[i]=ind_uni_pattern;
            ind_uni_pattern++;
        }
        //cout<<"========"<<endl;

    }




    //remove duplicate entries
    sort( discovered_patterns1.begin(), discovered_patterns1.end() );
    discovered_patterns1.erase( unique( discovered_patterns1.begin(), discovered_patterns1.end() ), discovered_patterns1.end() );

    sort( discovered_patterns2.begin(), discovered_patterns2.end() );
    discovered_patterns2.erase( unique( discovered_patterns2.begin(), discovered_patterns2.end() ), discovered_patterns2.end() );



    //update current pattern indexes for day1 and day2
    int ind_gen=ind_uni_pattern;
    for(int i=0;i<discovered_patterns1.size();i++)
    {
        if(discovered_patterns1[i]<initial_index)
        {
            for(int j=0;j<fine_grain_seq1.size();j++)
            {
                if(discovered_patterns1[i]==fine_grain_seq1[j])
                {
                    fine_grain_seq1[j]=ind_gen;
                }
            }
            discovered_patterns1[i]=ind_gen;
            ind_gen++;
        }
        if(verbose)
            cout<<discovered_patterns1[i]<<" ";
    }
    if(verbose)
        cout<<endl;

    for(int i=0;i<discovered_patterns2.size();i++)
    {
        if(discovered_patterns2[i]<initial_index)
        {
            for(int j=0;j<fine_grain_seq2.size();j++)
            {
                if(discovered_patterns2[i]==fine_grain_seq2[j])
                {
                    fine_grain_seq2[j]=ind_gen;
                }
            }
            discovered_patterns2[i]=ind_gen;
            ind_gen++;
        }
        if(verbose)
            cout<<discovered_patterns2[i]<<" ";
    }
    if(verbose)
        cout<<endl;



    int count2=0;
    for(int j=0;j<discovered_patterns1.size();j++)
    {
        for(int i=0;i<fine_grain_seq1.size();i++)
        {


            if(fine_grain_seq1[i]==discovered_patterns1[j])
            {
                count2++;
            }


        }
    }
    cout<<fine_grain_seq1.size()<<"\t"<<count2<<endl;
    //for printing purposes, there is no any computations done below

    if(verbose){
        int cnt1=0,cnt2=0;

        for(int j=0;j<fine_grain_seq1.size();j++)
        {
            bool is_match=false;
            for(int i=0;i<new_discovered_patterns.size();i++)
            {

                if(fine_grain_seq1[j]==new_discovered_patterns[i])
                {
                    cnt1++;
                    is_match=true;
                }

            }

            if(is_match)
                cout<<fine_grain_seq1[j]<<" ";

            if(!is_match)
                cout<<"*"<<fine_grain_seq1[j]<<"* ";

        }
        cout<<endl;

        cout<<"\n\n\n\n\n\n";

        for(int j=0;j<fine_grain_seq2.size();j++)
        {
            bool is_match=false;
            for(int i=0;i<new_discovered_patterns.size();i++)
            {
                if(fine_grain_seq2[j]==new_discovered_patterns[i])
                {
                    cnt2++;
                    is_match=true;
                }

            }
            if(is_match)
                cout<<fine_grain_seq2[j]<<" ";

            if(!is_match)
                cout<<"*"<<fine_grain_seq2[j]<<"* ";
        }
        cout<<endl;

        int merged_discovered_patterns=discovered_patterns1.size()+discovered_patterns2.size()-new_discovered_patterns.size();
        cout<<discovered_patterns1.size()<<"\t"<<discovered_patterns2.size()<<"\t"<<merged_discovered_patterns<<endl;
        cout<<"****"<<cnt1<<"\t"<<cnt2<<"\t"<<new_discovered_patterns.size()<<endl;
    }

    vector<vector<int> > patterns_topics;
    patterns_topics.push_back(new_discovered_patterns);
    patterns_topics.push_back(new_pattern_day_parts);


    return patterns_topics;

}

vector<vector<int> > InferentialDiscovery::computeChiSquareIndTest(FeatureContrainer* fc1,FeatureContrainer* fc2)
{
    if(verbose)
        cout<<"computeChiSquareIndTest"<<endl;

    vector<int> total_rows_pattern1,total_rows_pattern2;
    vector<vector<int> > within_p_chi_square;

    vector<vector<vector<int> > > processed_features1=fc1->getProcessedFeatures();
    vector<vector<int> > day_patterns1=processed_features1[0];
    vector<int> dp1=fc1->getDiscoveredPatterns();
    vector<int> sequence_patterns1=fc1->getSequencePatterns();

    vector<vector<vector<int> > > processed_features2=fc2->getProcessedFeatures();
    vector<vector<int> > day_patterns2=processed_features2[0];
    vector<int> dp2=fc2->getDiscoveredPatterns();
    vector<int> sequence_patterns2=fc2->getSequencePatterns();
    vector<int> pattern_day_parts1,pattern_day_parts2;


    if(sequence_patterns1.size()==day_len)
    {
        pattern_day_parts1=Common::findPatternDayParts(dp1,sequence_patterns1,f_day_parts);
        fc1->setPatternDayParts(pattern_day_parts1);

    }
    else
        pattern_day_parts1=fc1->getPatternDayParts();





    if(sequence_patterns2.size()==day_len)
    {
        pattern_day_parts2=Common::findPatternDayParts(dp2,sequence_patterns2,f_day_parts);
        fc2->setPatternDayParts(pattern_day_parts2);

    }
    else
        pattern_day_parts2=fc2->getPatternDayParts();


    for(int i=0;i<day_patterns1.size();i++)
    {
        vector<int> p=day_patterns1[i];

        int sum_row=  std::accumulate(p.begin(), p.end(), 0);;


        total_rows_pattern1.push_back(sum_row);
    }


    for(int i=0;i<day_patterns2.size();i++)
    {
        vector<int> p=day_patterns2[i];

        int sum_row=  std::accumulate(p.begin(), p.end(), 0);


        total_rows_pattern2.push_back(sum_row);
    }



    for(int i=0;i<day_patterns1.size();i++)
    {
        vector<int> p1=day_patterns1[i];
        int sum_row_p1=total_rows_pattern1[i];

        vector<int> within_p;
        bool is_match=false;

        for(int j=0;j<day_patterns2.size();j++)
        {

            vector<int> p2=day_patterns2[j];
            float X_square=0;

            if(pattern_day_parts1[i]==pattern_day_parts2[j])
            {
                int sum_row_p2=total_rows_pattern2[j];


                int total_sum=sum_row_p1+sum_row_p2;


                //compute X^2
                for(int d=0;d<p1.size();d++)
                {
                    int sum_col=p1[d]+p2[d];
                    float expected_p1=(sum_row_p1*1.0*sum_col)/(total_sum*1.0);
                    float expected_p2=(sum_row_p2*1.0*sum_col)/(total_sum*1.0);

                    X_square+=(std::pow((p1[d]-expected_p1)*1.0,2)/(expected_p1*1.0))+(std::pow((p2[d]-expected_p2)*1.0,2)/(expected_p2*1.0));


                }
                //critical value @ alpha=0.05 with df=34
                if(X_square<this->chi_square_critical_value)
                {
                    is_match=true;
                    within_p.push_back(j);
                    if(verbose)
                        cout<<"-->"<<dp1[i]<<"\t"<<dp2[j]<<"\t"<<X_square<<endl;


                }
            }



        }

        if(!is_match)
        {
            if(verbose)
                cout<<dp1[i]<<endl;

            within_p.push_back(0);
        }

        within_p_chi_square.push_back(within_p);
    }


    return within_p_chi_square;

}
void InferentialDiscovery::findMostCommonActivityPattern(FeatureContrainer* fc)
{
    if(verbose)
        cout<<"findMostCommonActivityPattern"<<endl;

    vector<int> activity_labels=fc->getActivityLabels();
    vector<int>  discovered_patterns=fc->getDiscoveredPatterns();
    vector<int>  fine_grain_seq=fc->getSequencePatterns();



    //default is "-", no activity assigned or labeled
    vector<int> most_common_act(discovered_patterns.size(),11);
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

FeatureContrainer* InferentialDiscovery::computeFeature(FeatureContrainer* fc)
{
    fc->clearProcessedFeatures();

    //process features to create chi_sqaure, ANOVA features
    generateChiSquareFeatures(fc);


    findMostCommonActivityPattern(fc);

    //    if(verbose){
    //        displayDiscoveredPatternInfo(fc);
    //        fc->printSequencePatterns();
    //    }

    return fc;

}

void InferentialDiscovery::computeFeatures(vector<FeatureContrainer*>& dataset_features)
{
    if(verbose)
        cout<<"computeFeatures"<<endl;

    for(int i=0;i<dataset_features.size();i++)
    {
        if(verbose)
            cout<<"======"<<days[i]<<"======"<<endl;

        FeatureContrainer* fc=dataset_features[i];
        dataset_features[i]=computeFeature(fc);

    }

}

void InferentialDiscovery::generateChiSquareFeatures(FeatureContrainer* fc)
{
    if(verbose)
        cout<<"generateChiSquareFeatures"<<endl;

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
        {
            if(count==0)
                count=1;
            sum_sensor_ellapsed_time[c]= ceil((sum_sensor_ellapsed_time[c]*1.0)/(count*1.0));

            if(sum_sensor_ellapsed_time[c]==1)
                sum_sensor_ellapsed_time[c]=2;

            if(sum_sensor_ellapsed_time[c]==0)
                sum_sensor_ellapsed_time[c]=1;

        }

        sum_pattern_ellapsed_time.push_back(sum_sensor_ellapsed_time);


    }



    fc->insertProcessedFeature(sum_pattern_ellapsed_time);
    fc->setPatternsLen(patterns_len);

}

vector<string> InferentialDiscovery::preparePaths(string& day)
{
    vector<string> file_paths;
    //used features
    file_paths.push_back(base_dir+day+"/features/sensor_ellapsed_time.txt");
    file_paths.push_back(base_dir+day+"/features/discovered_patterns.txt");
    file_paths.push_back(base_dir+day+"/features/sequence_patterns.txt");
    file_paths.push_back(base_dir+day+"/features/activity_labels.txt");

    //not used features, maye useful to determine activitiness
    file_paths.push_back(base_dir+day+"/features/sensor_count.txt");
    file_paths.push_back(base_dir+day+"/features/room_count.txt");
    file_paths.push_back(base_dir+day+"/features/room_ellapsed_time.txt");

    return file_paths;
}
