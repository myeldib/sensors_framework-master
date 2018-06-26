#include<ActivityDiscovery.h>

ActivityDiscovery::ActivityDiscovery(SensorReader& sr,DayContainer& coarse_grain_day, DayContainer& fine_grain_day,int& coarse_grain_win,int& fine_grain_win,vector<string> room_sybmols)
{

    this->coarse_grain_day=coarse_grain_day;
    this->fine_grain_day=fine_grain_day;
    this->coarse_grain_win=coarse_grain_win/60;
    this->fine_grain_win=fine_grain_win/60;
    this->sr=sr;
    this->room_sybmols=room_sybmols;
    this->dl_threshold=(0.0)/(1440.0);
    this->emd_threshold=0.2;
    this->time_dependency_threshold=0.50;
    this->X1=5;
    this->X2=8;
    room_output_file.open("../data/room_output.txt", std::ios_base::app);
    sensor_output_file.open("../data/sensor_output.txt", std::ios_base::app);


}


bool ActivityDiscovery::computeTimeDepenency(int X, vector<int>& t1, vector<int>& t2)
{
    int diff=0.0;
    for(int i=0;i<t1.size();i++)
    {
        diff+=abs(t1[i]-t2[i]);
    }
    float time_de=exp(-1*pow(2,-1*X)*diff);

    //    if(t1.size()<15 && t2.size()<15)
    //        room_output_file<<diff<<endl;

    //    if(t1.size()>15 && t2.size()>15)
    //        sensor_output_file<<diff<<endl;

    if(time_de>=time_dependency_threshold)
        return true;
    else
        return false;
}
bool ActivityDiscovery::computeEMDSimilarity(vector<int> &e1, vector<int> &e2)
{
    EarthMoverDistance emd(e1,e2);
    float e=emd.computeEMD();

    //    if(e1.size()<15 && e2.size()<15)
    //        room_output_file<<e<<endl;

    //    if(e1.size()>15 && e2.size()>15)
    //        sensor_output_file<<e<<endl;

    if(e<=0.1)
        return true;
    else
        return false;

}


void ActivityDiscovery::printRoomEllapsedTimeLevel(vector<int>& room_ellapsed_time)
{


    if(room_ellapsed_time.size()>15)
    {
        cout<<"(";
        for(int i=0;i<room_ellapsed_time.size();i++)
        {

            if(room_ellapsed_time[i]!=0)
                cout<<"M"<<i+1<<":"<<room_ellapsed_time[i];

            if(i<room_ellapsed_time.size()-1)
                cout<<",";
        }
        cout<<")";
        cout<<endl;
    }
    else
    {
        cout<<"(";
        for(int i=0;i<room_ellapsed_time.size();i++)
        {

            if(room_ellapsed_time[i]!=0)
                cout<<"R"<<i+1<<":"<<room_ellapsed_time[i];

            if(i<room_ellapsed_time.size()-1)
                cout<<",";
        }
        cout<<")";
        cout<<endl;
    }


}

void ActivityDiscovery::findFineGrainPatterns(vector<int>& discovered_patterns, vector<int>& uni_pattern_index)
{


    vector<vector<int > > f_day_sensor_ellapsed_time=fine_grain_day.get_day_sensor_ellapsed_time();
    vector<vector<int > > f_day_window_level=fine_grain_day.get_day_window_level();
    vector<int> day_parts=fine_grain_day.getDayParts();


    int cnt_uni_pattern=1;
    int pattern_gen=Common::isUniqueGeneratedPattern(1,discovered_patterns);

    for(int i=0;i<f_day_sensor_ellapsed_time.size();i++)
        fine_grain_seq.push_back(0);

    for(int i=0;i<discovered_patterns.size();i++)
    {
        for(int j=0;j<coarse_grain_seq.size();j++)
        {
            if(coarse_grain_seq[j]==discovered_patterns[i])
            {
                int start_fine_grain_index=(j*coarse_grain_win)/fine_grain_win;
                int end_fine_grain_index=(j*coarse_grain_win+coarse_grain_win)/fine_grain_win;
               cout<<start_fine_grain_index<<"\t"<<end_fine_grain_index<<endl;
                for(int c=start_fine_grain_index;c<end_fine_grain_index;c++)
                {
                    fine_grain_seq[c]=discovered_patterns[i];
                }
                 cout<<coarse_grain_seq[j]<<"\t"<<discovered_patterns[i]<<endl;
            }

        }
    }


    //room time
    for(int i=0;i<uni_pattern_index.size();i++)
    {
        int start_fine_grain_index1=(uni_pattern_index[i]*coarse_grain_win)/fine_grain_win;
        int end_fine_grain_index1=(uni_pattern_index[i]*coarse_grain_win+coarse_grain_win)/fine_grain_win;

        for(int c=start_fine_grain_index1;c<end_fine_grain_index1;c++)
        {

            bool pattern_found=false;
            vector<int> room_ellapsed_time1=f_day_sensor_ellapsed_time[c];

            for(int j=0;j<uni_pattern_index.size();j++)
            {
                int start_fine_grain_index2=(uni_pattern_index[j]*coarse_grain_win)/fine_grain_win;
                int end_fine_grain_index2=(uni_pattern_index[j]*coarse_grain_win+coarse_grain_win)/fine_grain_win;


                for(int r=start_fine_grain_index2;r<end_fine_grain_index2;r++)
                {

                    vector<int> room_ellapsed_time2=f_day_sensor_ellapsed_time[r];

                    bool is_match=false;

                    if(day_parts[c]==day_parts[r])
                    {
                        //is_match=computeTimeDepenency(X1,room_ellapsed_time1,room_ellapsed_time2);
                        is_match=computeEMDSimilarity(room_ellapsed_time1,room_ellapsed_time2);
                    }

                    if(fine_grain_seq[r]==0 && day_parts[c]==day_parts[r])
                    {



                        if(is_match)
                        {

                            //                            printRoomEllapsedTimeLevel(room_ellapsed_time1);
                            //                            printRoomEllapsedTimeLevel(room_ellapsed_time2);
                            //                            cout<<c<<"\t"<<r<<"\t"<<e<<endl;
                            fine_grain_seq[r]=pattern_gen;
                            pattern_found=true;
                        }
                    }
                }
            }
            if(pattern_found)
            {
                discovered_patterns.push_back(pattern_gen);
                pattern_gen=Common::isUniqueGeneratedPattern(++pattern_gen,discovered_patterns);
                cnt_uni_pattern++;
            }

        }

    }


    patterns_size.push_back(discovered_patterns.size());
    cout<<"findFineGrainPatterns:"<<discovered_patterns.size()<<endl;

    discoverDistinctPatterns(fine_grain_seq,fine_grain_win,discovered_patterns,f_day_window_level);
    patterns=discovered_patterns;

}

void ActivityDiscovery::discoverDistinctPatterns(vector<int>& fine_grain_seq,int& fine_grain_win, vector<int>& discovered_patterns, vector<vector<int > >& f_day_window_level)
{

    vector<int> cnt_frequent_pattern,cnt_subfrequent_pattern,frequent_pattern,subfrequent_pattern,new_discovered_patterns;
    vector<string> most_occ_room;


    findFrequentPatterns(cnt_frequent_pattern,frequent_pattern,subfrequent_pattern,cnt_subfrequent_pattern,fine_grain_seq,fine_grain_win,discovered_patterns);

    int all_time=0;
    int subfre=0;
    for(int i=0;i<cnt_frequent_pattern.size();i++)
    {
        //default was 10
        //if(cnt_frequent_pattern[i]<=10)
        {
            all_time+=cnt_frequent_pattern[i];
            subfre++;
        }

        cout<<frequent_pattern[i]<<":"<<cnt_frequent_pattern[i]<<" ";
    }
    cout<<endl;
    cout<<all_time<<"\t"<<subfre<<endl;


    Common::sortFrequentPatterns(cnt_frequent_pattern,frequent_pattern);
    Common::sortFrequentPatterns(cnt_subfrequent_pattern,subfrequent_pattern);

    discovered_patterns=frequent_pattern;

}

void ActivityDiscovery::findFrequentPatterns(vector<int> &cnt_frequent_pattern, vector<int> &frequent_pattern, vector<int> &subfrequent_pattern,vector<int>& cnt_subfrequent_pattern,vector<int>& seq,int& fine_grain_win, vector<int> &discovered_patterns)
{

    int all_pattern_occ=0;
    for(int i=0;i<discovered_patterns.size();i++)
    {
        int cnt_pattern_occ=0;

        for(int j=0;j<seq.size();j++)
        {
            if(seq[j]==discovered_patterns[i])
                cnt_pattern_occ++;
        }
        if(computeMinimumDiscriptionLength(cnt_pattern_occ,fine_grain_win,seq.size()))
        {
            frequent_pattern.push_back(discovered_patterns[i]);
            cnt_frequent_pattern.push_back(cnt_pattern_occ);
            all_pattern_occ+=cnt_pattern_occ;
            // cout<<"pattern_discovered:"<<i<<endl;
        }
        else
        {
            subfrequent_pattern.push_back(discovered_patterns[i]);
            cnt_subfrequent_pattern.push_back(cnt_pattern_occ);

        }

        // cout<<"pattern_gen:"<<i<<"\t"<<cnt_pattern_occ<<endl;
    }


    // cout<<"all_pattern_occ:"<<all_pattern_occ<<endl;
}

bool ActivityDiscovery::computeMinimumDiscriptionLength(int pattern_length, int frequency, int dataset_size)
{
    float dl=(pattern_length*frequency*1.0)/(1.0*dataset_size);

    if(dl>dl_threshold)
        return true;
    else return false;
}


void ActivityDiscovery::printWindowSensorEvents(vector<Sensor>& sensors)
{
    for(int i=0;i<sensors.size();i++)
    {
        Sensor s=sensors[i];
        s.printSensorInfo();
    }
}
void ActivityDiscovery::run()
{
    findCoarseGrainPatterns();

    std::ofstream outfile("../data/pattern_size_stat.txt", std::ios_base::app);

    outfile<<fine_grain_day.getDay();

    for(int i=0;i<patterns_size.size();i++)
        outfile <<"\t"<<patterns_size[i];
    outfile<<endl;

    outfile.close();

}

void ActivityDiscovery::findCoarseGrainPatterns()
{

    int cnt_uni_pattern=0;
    int pattern_gen=1;

    vector<vector<int > > c_day_room_ellapsed_time=coarse_grain_day.get_day_room_ellapsed_time();
    vector<int> day_parts=coarse_grain_day.getDayParts();

    for(int i=0;i<c_day_room_ellapsed_time.size();i++)
        coarse_grain_seq.push_back(0);


    for(int i=0;i<c_day_room_ellapsed_time.size();i++)
    {
        vector<int> room_ellapsed_time1=c_day_room_ellapsed_time[i];
        bool pattern_found=false;

        for(int j=0;j<c_day_room_ellapsed_time.size();j++)
        {

            vector<int> room_ellapsed_time2=c_day_room_ellapsed_time[j];
            bool is_diff=false;



            if(day_parts[i]==day_parts[j])
            {
                //   is_diff=computeTimeDepenency(X2,room_ellapsed_time1,room_ellapsed_time2);
                is_diff=computeEMDSimilarity(room_ellapsed_time1,room_ellapsed_time2);
            }


            if(coarse_grain_seq[j]==0 && day_parts[i]==day_parts[j])
            {
                if(is_diff)
                {
                    coarse_grain_seq[j]=pattern_gen;
                    pattern_found=true;
                }
            }

        }

        if(pattern_found)
            pattern_gen++;
    }


    cout<<"pattern_gen:"<<pattern_gen<<endl;
    vector<int> discovered_patterns,uni_pattern_index;
    for(int i=1;i<pattern_gen;i++)
    {
        int cnt_pattern_occ=0;
        int uni_pattern;
        for(int j=0;j<coarse_grain_seq.size();j++)
        {
            if(coarse_grain_seq[j]==i)
            {
                uni_pattern=j;
                cnt_pattern_occ++;
            }

        }
        if(cnt_pattern_occ==1)
        {
            uni_pattern_index.push_back(uni_pattern);
            cnt_uni_pattern++;
        }
        else
        {
            discovered_patterns.push_back(i);
            cout<<"coarse_pattern_discovered:"<<i<<endl;
        }
       cout<<i<<"\t"<<cnt_pattern_occ<<"\t"<<endl;
    }

    patterns_size.push_back(pattern_gen);
    patterns_size.push_back(discovered_patterns.size());
    cout<<"findCoarseGrainPatterns: "<<discovered_patterns.size()<<endl;

    findFineGrainPatterns(discovered_patterns,uni_pattern_index);
}
