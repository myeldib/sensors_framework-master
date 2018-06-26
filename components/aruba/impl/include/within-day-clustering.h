#ifndef WITHINDAYCLUSTERING_H
#define WITHINDAYCLUSTERING_H
#include<home.h>
#include<preprocessing-stage.h>
#include<file-separator.h>
#include<time-window-approach.h>
#include<hist-cluster.h>
#include<logger.h>

class WithinDayClustering
{
public:
    WithinDayClustering(string home_setup_file,string sensors_data_file,string preprocessed_data_file,string time_window_config,string result_dir,bool is_data_preprocessed);
    ~WithinDayClustering();
    void run();
    void destroy();
private:
    void init_();
private:
    string home_setup_file;
    string sensors_data_file;
    string preprocessed_data_file;
    string time_window_config;
    string result_dir;
    bool is_data_preprocessed;

    Home* home;
    PreprocessingStage* pre;
    FileSeparator* fileSep;
    TimeWindow* time_window;
    HistogramClustering* hist_clust;
};

#endif // WITHINDAYCLUSTERING_H
