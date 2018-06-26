#include<within-day-clustering.h>

/**
 * @brief WithinDayClustering::WithinDayClustering
 */
WithinDayClustering::WithinDayClustering(string home_setup_file,string sensors_data_file,string preprocessed_data_file,string time_window_config,string result_dir,bool is_data_preprocessed)
{
    COUT<<"WithinDayClustering"<<endl;
    logging::INFO("WithinDayClustering");

    this->home_setup_file=home_setup_file;
    this->sensors_data_file=sensors_data_file;
    this->preprocessed_data_file=preprocessed_data_file;
    this->time_window_config=time_window_config;
    this->result_dir=result_dir;
    this->is_data_preprocessed=is_data_preprocessed;

    init_();

}

void WithinDayClustering::init_()
{
    logging::INFO("init_");

    home = new Home(home_setup_file,time_window_config);
    home->readHomeSetup();

    this->result_dir=this->result_dir+
            "_"+
            std::to_string(home->getShortDurationLambda())+
            "_"+
            std::to_string(home->getLongDurationLambda())
            +"/";

    Common::createFolder(this->result_dir);


    pre = new PreprocessingStage(sensors_data_file,preprocessed_data_file,home);
    fileSep= new FileSeparator(preprocessed_data_file,result_dir);
    time_window= new TimeWindow(home,result_dir);
    hist_clust = new HistogramClustering(result_dir,home);
}

/**
 * @brief WithinDayClustering::destroy
 */
void WithinDayClustering::destroy()
{
    logging::INFO("destroy");
    delete this;
}

/**
 * @brief WithinDayClustering::~WithinDayClustering
 */
WithinDayClustering::~WithinDayClustering()
{
    COUT<<"~WithinDayClustering"<<endl;
    logging::INFO("~WithinDayClustering");

    if(home) delete home;

    if(pre)  delete pre;

    if(fileSep) delete fileSep;

    if(time_window)  delete time_window;

    if(hist_clust)  delete hist_clust;

}

/**
 * @brief WithinDayClustering::run
 */
void WithinDayClustering::run()
{
    COUT<<"run"<<endl;
    logging::INFO("run");

    if(!is_data_preprocessed)
    {
        //process sensor data file in order to extract duration and location information
        pre->extractSensorInfo();

        //divide preprocessed sensor data file date folders
        fileSep->writeDailySensorData();

    }

    //create duration windows
    time_window->run();

    // cluster windows within a single day instance
    hist_clust->run();


}
