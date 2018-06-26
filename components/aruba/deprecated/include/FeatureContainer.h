#ifndef FEATURECONTAINER_H
#define FEATURECONTAINER_H
#include<string>
#include<vector>
#include<Common.h>

using namespace std;
using namespace deprecated;

class FeatureContrainer{
public:
    FeatureContrainer(vector<string>& file_paths)
    {
        this->file_paths=file_paths;
        readConfigFile();
        processFiles();

    }
     ~FeatureContrainer()
    {
        freeMemory();
    }

    FeatureContrainer()
    {
        for(int i=0;i<num_files;i++)
        {

            vector<vector<int> >tmp;
            features.push_back(tmp);

        }
        readConfigFile();

    }
    vector<vector<vector<int> > >& getFeatures(){return features;}
    vector<int> &getPatternsLen(){return pattern_lens;}
    void setPatternsLen(vector<int> pattern_lens){this->pattern_lens=pattern_lens;}

    vector<vector<int> >& getSensorEllapsedTime()
    {

        sensor_ellapsed_time=features[0];
        return sensor_ellapsed_time;

    }


    vector<int>& getDiscoveredPatterns()
    {

        vector<vector<int> > patterns=features[1];
        discovered_patterns=patterns[0];


        return discovered_patterns;

    }

    vector<int>& getSequencePatterns()
    {
        vector<vector<int> > fine_grain_seq=features[2];
        sequence_patterns=fine_grain_seq[0];

        return sequence_patterns;
    }

    vector<int>& getActivityLabels() {return activity_labels; }

    void setPatternDayParts(vector<int> pattern_day_parts){this->pattern_day_parts=pattern_day_parts;}
    vector<int>& getPatternDayParts(){return pattern_day_parts;}

    void setSensorEllapsedTime(vector<vector<int> > sensor_ellapsed_time)
    {
        this->sensor_ellapsed_time=sensor_ellapsed_time;
        features[0]=sensor_ellapsed_time;
    }

    void setSequenceTimes(vector<int> sequence_times){this->sequence_times=sequence_times;}
    vector<int> getSequenceTimes(){return this->sequence_times;}

    void setDiscoveredPatterns(vector<int> discovered_patterns )
    {
        this->discovered_patterns=discovered_patterns;
        vector<vector<int> >patterns;
        patterns.push_back(this->discovered_patterns);
        features[1]=patterns;
    }

    void setSequencePatterns(vector<int> sequence_patterns )
    {
        this->sequence_patterns=sequence_patterns;
        vector<vector<int> >patterns;
        patterns.push_back(this->sequence_patterns);
        features[2]=patterns;
    }

    void printSequencePatterns()
    {
        for(int i=0;i<this->sequence_patterns.size();i++)
            cout<<sequence_patterns[i]<<endl;
    }

    void setActivityLabels(vector<int> activity_labels){this->activity_labels=activity_labels; }


    void insertProcessedFeature(vector<vector<int> > processed_feature){processed_features.push_back(processed_feature);}
    void insertProcessedActivityLabels(vector<int> activity_labels){ processed_activity_labels.push_back(activity_labels);}
    void setMostCommonActivityPattern(vector<int> most_common_activity_pattern){this->most_common_activity_pattern=most_common_activity_pattern;}
    vector<int>& getMostCommonActivityPattern(){ return most_common_activity_pattern;}
    vector<vector<vector<int> > >& getProcessedFeatures(){return this->processed_features;}
    void setProcessedFeatures(vector<vector<vector<int> > > processed_features){this->processed_features=processed_features;}
    vector<vector<int> >& getProcessedActivityLabels(){return this->processed_activity_labels;}
    void clearProcessedFeatures(){ this->processed_features.clear();processed_activity_labels.clear();}
    void freeMemory()
    {
        vector<int>().swap( pattern_day_parts );
        vector<string>().swap( activity_names );
        vector<int>().swap( most_common_activity_pattern );
        vector<int>().swap( activity_indexes );
        vector<int>().swap( pattern_lens );
        vector<int>().swap( activity_labels );
        vector<int>().swap( sequence_patterns );
        vector<int>().swap( discovered_patterns );
        vector<int>().swap( pattern_day_parts );
        vector<int>().swap( pattern_day_parts );
        vector<int>().swap( pattern_day_parts );
        vector<int>().swap( pattern_day_parts );
        vector<int>().swap( pattern_day_parts );
        vector<int>().swap( pattern_day_parts );
        vector<string>().swap( file_paths );

        sensor_ellapsed_time.clear();
        sensor_ellapsed_time.shrink_to_fit();

        processed_activity_labels.clear();
        processed_activity_labels.shrink_to_fit();

        processed_features.clear();
        processed_features.shrink_to_fit();

        features.clear();
        features.shrink_to_fit();

    }


private:
    void readConfigFile()
    {
        string config_path="../data/config";
        std::ifstream infile( config_path.c_str());
        int line_index=0;

        while (infile)
        {
            string line;

            if (!getline( infile, line )) break;

            vector<string> elem;
            Common::split(line,'=',elem);

            if(line_index>=3)
            {
                activity_names.push_back(elem[0]);
                activity_indexes.push_back(atoi(elem[1].c_str()));

            }

            line_index++;
        }
    }

    void processFiles()
    {
        for(int i=0;i<file_paths.size();i++)
        {
            vector<vector<int> >tmp;
            features.push_back(tmp);
        }
        //process only first 4 files (used features). Reason for size()-3
        for(int i=0;i<file_paths.size()-3;i++)
            features[i]=readFile(file_paths[i]);

    }
    vector<vector<int> > readFile(string& file_name)
    {

        std::ifstream infile(file_name);
        vector<vector<int> > file_content;

        while (infile)
        {
            string line;
            vector<int> line_content;
            vector<string> elem;
            if (!getline( infile, line )) break;

            Common::split(line,',',elem);

            if(file_name.find("activity_labels.txt") != std::string::npos)
            {
                for(int i =0;i<elem.size();i++)
                {
                    for(int j=0;j<activity_names.size();j++)
                    {
                        if(elem[i]==activity_names[j])
                        {
                            activity_labels.push_back(activity_indexes[j]);

                        }


                    }
                }


            }
            else
            {

                for(int i =0;i<elem.size();i++)
                    line_content.push_back(atoi(elem[i].c_str()));
            }

            file_content.push_back(line_content);
        }

        infile.close();
        return file_content;
    }




private:
    // 0-3 are used as features
    //0 sensor_ellapsed_time
    //1 discovered_patterns
    //2 sequence_patterns
    //3 activity_labels
    //4 sensor_count
    //5 room_count
    //6 room_ellapsed_time

    vector<vector<vector<int> > > features;
    vector<string> file_paths;
    vector<vector<vector<int> > > processed_features;
    vector<vector<int> > processed_activity_labels;

    //processed chi square features
    vector<int> discovered_patterns;
    vector<int> sequence_patterns;
    vector<vector<int> > sensor_ellapsed_time;
    vector<int> activity_labels;
    vector<int> pattern_lens;
    vector<int> sequence_times;
    vector<int> activity_indexes;
    vector<int> most_common_activity_pattern;
    vector<string> activity_names;
    vector<int> pattern_day_parts;

    int num_files=7;



};

#endif // FEATURECONTAINER_H
