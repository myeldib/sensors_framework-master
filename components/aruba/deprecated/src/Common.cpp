#include"Common.h"
using namespace deprecated;
vector<string>& Common::split(const string &s, char delim, vector<string> &elems)
{

    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;

}

int Common::isUniqueGeneratedPattern(int pattern_gen,vector<int>& discovered_patterns)
{

    for(int i=0;i<discovered_patterns.size();i++)
    {
        if(discovered_patterns[i]==pattern_gen)
            isUniqueGeneratedPattern(++pattern_gen,discovered_patterns);
    }

    return pattern_gen;
}

void Common::saveImage(string path, Mat img)
{
    imwrite(path,img);
}
bool Common::compareMapValues(const std::pair<std::string, int>& lhs,
                              const std::pair<std::string, int>& rhs)
{
    // cout<<"******"<<lhs.first<<"\t"<<lhs.second<<"\t"<<rhs.first<<"\t"<<rhs.second<<"******"<<endl;
    return lhs.second < rhs.second;
}

double Common::calculateDistance(Point p1,Point p2)
{
    double dist = pow((p1.x - p2.x),2)+ pow((p1.y - p2.y),2);
    return dist;
}
bool Common::createFolder(string directory)
{
    boost::filesystem::path dir(directory);
    boost::filesystem::create_directory(dir);
    // if (boost::filesystem::create_directory(dir))
    // std::cout << "Success" << "\n";
}


string Common::execTerminalCMD(string cmd)
{
    //  /  cout<<"execTerminalCMD: "<<cmd<<endl;

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe))
    {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

void Common::useKalmanFilter2(vector<Sensor>& sensor_positions)
{
    Mat img=imread("../data/layout.jpg");
    vector<Point> mousev, kalmanv;


    Mat_<float> measurement(2,1); measurement.setTo(Scalar(0));
    KalmanFilter* kalman = new KalmanFilter( 4, 2, 0 );
    double deltatime = 0.7;
    float Accel_noise_mag = 0.5;

    namedWindow("mouse kalman");

    bool DataCorrect=true;

    kalman->statePre.at<float>(0) = sensor_positions[0].getSensorPosition().x; // x
    kalman->statePre.at<float>(1) = sensor_positions[0].getSensorPosition().y; // y

    kalman->statePre.at<float>(2) = 0;
    kalman->statePre.at<float>(3) = 0;

    kalman->statePost.at<float>(0)=sensor_positions[0].getSensorPosition().x;
    kalman->statePost.at<float>(1)=sensor_positions[0].getSensorPosition().y;

    kalman->transitionMatrix = (Mat_<float>(4, 4) << 1,0,deltatime,0,   0,1,0,deltatime,  0,0,1,0,  0,0,0,1);

    setIdentity(kalman->measurementMatrix);

    kalman->processNoiseCov=(Mat_<float>(4, 4) <<
                             pow(deltatime,4.0)/4.0	,0						,pow(deltatime,3.0)/2.0		,0,
                             0						,pow(deltatime,4.0)/4.0	,0							,pow(deltatime,3.0)/2.0,
                             pow(deltatime,3.0)/2.0	,0						,pow(deltatime,2.0)			,0,
                             0						,pow(deltatime,3.0)/2.0	,0							,pow(deltatime,2.0));


    kalman->processNoiseCov*=Accel_noise_mag;


    for(int j=1;j<sensor_positions.size();j++)
    {

        Mat prediction = kalman->predict();
        Point LastResult=Point2f(prediction.at<float>(0),prediction.at<float>(1));

        //        measurement(0) = sensor_positions[j].getSensorPosition().x;
        //        measurement(1) = sensor_positions[j].getSensorPosition().y;



        if(!DataCorrect)
        {
            measurement.at<float>(0) = LastResult.x;  //update using prediction
            measurement.at<float>(1) = LastResult.y;
        }
        else
        {
            measurement.at<float>(0) = sensor_positions[j].getSensorPosition().x;  //update using measurements
            measurement.at<float>(1) = sensor_positions[j].getSensorPosition().y;
        }


        Point measPt(measurement(0),measurement(1));
        mousev.push_back(measPt);


        // Correction
        Mat estimated = kalman->correct(measurement);
        LastResult.x=estimated.at<float>(0);   //update using measurements
        LastResult.y=estimated.at<float>(1);

        sensor_positions[j].setSensorPosition(LastResult);
        kalmanv.push_back(LastResult);

        //        // plot points
        //#define drawCross( center, color, d )                                 \
        //    line( img, Point( center.x - d, center.y - d ),                \
        //    Point( center.x + d, center.y + d ), color, 2, CV_AA, 0); \
        //    line( img, Point( center.x + d, center.y - d ),                \
        //    Point( center.x - d, center.y + d ), color, 2, CV_AA, 0 )


        //        drawCross( LastResult, Scalar(0,255,255), 5 );
        //        drawCross( measPt, Scalar(0,0,255), 5 );


        //        for (int i = 0; i < mousev.size()-1; i++) {
        //            line(img, mousev[i], mousev[i+1], Scalar(255,255,0), 1);
        //        }
        //        for (int i = 0; i < kalmanv.size()-1; i++) {
        //            line(img, kalmanv[i], kalmanv[i+1], Scalar(0,255,0), 1);
        //        }

        //        imshow( "mouse kalman", img );

        //        cv::waitKey(0);

    }

}

int trackbarProcessNoiseCov = 317, trackbarMeasurementNoiseCov = 5;

float processNoiseCov=10, measurementNoiseCov = 1000, stateEstimationErrorCov = 50;
int trackbarProcessNoiseCovMax=10000, trackbarMeasurementNoiseCovMax = 10000,
trackbarStateEstimationErrorCovMax = 5000;

float processNoiseCovMin=0, measurementNoiseCovMin = 0,
stateEstimationErrorCovMin = 0;
float processNoiseCovMax=100, measurementNoiseCovMax = 5000,
stateEstimationErrorCovMax = 5000;

int nStates = 5, nMeasurements = 2, nInputs = 1;
KalmanFilter KF(nStates, nMeasurements, nInputs, CV_64F);


void on_trackbarProcessNoiseCov( int, void* )
{
    processNoiseCov = processNoiseCovMin +
            (trackbarProcessNoiseCov * (processNoiseCovMax-processNoiseCovMin)/trackbarProcessNoiseCovMax);
    setIdentity(KF.processNoiseCov, Scalar::all(processNoiseCov));
    //    std::cout << "\nProcess Noise Cov:     " << processNoiseCov;
    //    std::cout << "\nMeasurement Noise Cov: " << measurementNoiseCov << std::endl;
}
void on_trackbarMeasurementNoiseCov( int, void* )
{
    measurementNoiseCov = measurementNoiseCovMin +
            (trackbarMeasurementNoiseCov * (measurementNoiseCovMax-measurementNoiseCovMin)/trackbarMeasurementNoiseCovMax);
    setIdentity(KF.measurementNoiseCov, Scalar::all(measurementNoiseCov));
    //    std::cout << "\nProcess Noise Cov:     " << processNoiseCov;
    //    std::cout << "\nMeasurement Noise Cov: " << measurementNoiseCov << std::endl;
}


vector<Sensor> Common::useKalmanFilter(vector<Sensor>& sensor_positions)
{
    vector<Point> mousev, kalmanv,noisyv;
    vector<Sensor> s;
    Mat img=imread("../data/layout.jpg");
    Mat state(nStates, 1, CV_64F);/* (x, y, Vx, Vy, a) */
    Mat measurementNoise(nMeasurements, 1, CV_64F), processNoise(nStates, 1, CV_64F);
    Mat measurement(nMeasurements,1,CV_64F); measurement.setTo(Scalar(0.0));
    Mat noisyMeasurement(nMeasurements,1,CV_64F); noisyMeasurement.setTo(Scalar(0.0));
    Mat prevMeasurement(nMeasurements,1,CV_64F); prevMeasurement.setTo(Scalar(0.0));
    Mat prevMeasurement2(nMeasurements,1,CV_64F); prevMeasurement2.setTo(Scalar(0.0));
    int key = -1, dt=100, T=1000;


    //    namedWindow("Mouse-Kalman");

    //    createTrackbar( "Process Noise Cov", "Mouse-Kalman", &trackbarProcessNoiseCov,
    //                    trackbarProcessNoiseCovMax, on_trackbarProcessNoiseCov );
    //    createTrackbar( "Measurement Noise Cov", "Mouse-Kalman", &trackbarMeasurementNoiseCov,
    //                    trackbarMeasurementNoiseCovMax, on_trackbarMeasurementNoiseCov );

    on_trackbarProcessNoiseCov( trackbarProcessNoiseCov, 0 );
    on_trackbarMeasurementNoiseCov( trackbarMeasurementNoiseCov, 0 );

    //while ( (char)(key=cv::waitKey(100)) != 'q' )
    {
        /// A (TRANSITION MATRIX INCLUDING VELOCITY AND ACCELERATION MODEL)
        KF.transitionMatrix.at<double>(0,0) = 1;
        KF.transitionMatrix.at<double>(0,1) = 0;
        KF.transitionMatrix.at<double>(0,2) = (dt/T);
        KF.transitionMatrix.at<double>(0,3) = 0;
        KF.transitionMatrix.at<double>(0,4) = 0.5*(dt/T)*(dt/T);

        KF.transitionMatrix.at<double>(1,0) = 0;
        KF.transitionMatrix.at<double>(1,1) = 1;
        KF.transitionMatrix.at<double>(1,2) = 0;
        KF.transitionMatrix.at<double>(1,3) = (dt/T);
        KF.transitionMatrix.at<double>(1,4) = 0.5*(dt/T)*(dt/T);

        KF.transitionMatrix.at<double>(2,0) = 0;
        KF.transitionMatrix.at<double>(2,1) = 0;
        KF.transitionMatrix.at<double>(2,2) = 1;
        KF.transitionMatrix.at<double>(2,3) = 0;
        KF.transitionMatrix.at<double>(2,4) = (dt/T);

        KF.transitionMatrix.at<double>(3,0) = 0;
        KF.transitionMatrix.at<double>(3,1) = 0;
        KF.transitionMatrix.at<double>(3,2) = 0;
        KF.transitionMatrix.at<double>(3,3) = 1;
        KF.transitionMatrix.at<double>(3,4) = (dt/T);

        KF.transitionMatrix.at<double>(4,0) = 0;
        KF.transitionMatrix.at<double>(4,1) = 0;
        KF.transitionMatrix.at<double>(4,2) = 0;
        KF.transitionMatrix.at<double>(4,3) = 0;
        KF.transitionMatrix.at<double>(4,4) = 1;


        /// Initial estimate of state variables
        KF.statePost = cv::Mat::zeros(nStates, 1,CV_64F);
        KF.statePost.at<double>(0) = sensor_positions[0].getSensorPosition().x;
        KF.statePost.at<double>(1) = sensor_positions[0].getSensorPosition().y;
        KF.statePost.at<double>(2) = 0.1;
        KF.statePost.at<double>(3) = 0.1;
        KF.statePost.at<double>(4) = 0.1;

        KF.statePre = KF.statePost;
        state = KF.statePost;

        /// Ex or Q (PROCESS NOISE COVARIANCE)
        setIdentity(KF.processNoiseCov, Scalar::all(processNoiseCov));


        /// Initial covariance estimate Sigma_bar(t) or P'(k)
        setIdentity(KF.errorCovPre, Scalar::all(stateEstimationErrorCov));

        /// Sigma(t) or P(k) (STATE ESTIMATION ERROR COVARIANCE)
        setIdentity(KF.errorCovPost, Scalar::all(stateEstimationErrorCov));

        /// B (CONTROL MATRIX)
        KF.controlMatrix = cv::Mat(nStates, nInputs,CV_64F);
        KF.controlMatrix.at<double>(0,0) = /*0.5*(dt/T)*(dt/T);//*/0;
        KF.controlMatrix.at<double>(1,0) = /*0.5*(dt/T)*(dt/T);//*/0;
        KF.controlMatrix.at<double>(2,0) = 0;
        KF.controlMatrix.at<double>(3,0) = 0;
        KF.controlMatrix.at<double>(4,0) = 1;

        /// H (MEASUREMENT MATRIX)
        KF.measurementMatrix = cv::Mat::eye(nMeasurements, nStates, CV_64F);

        /// Ez or R (MEASUREMENT NOISE COVARIANCE)
        setIdentity(KF.measurementNoiseCov, Scalar::all(measurementNoiseCov));

        prevMeasurement.at<double>(0,0) = 0;
        prevMeasurement.at<double>(1,0) = 0;
        prevMeasurement2 = prevMeasurement;


        for(int i=0;i<sensor_positions.size();i++)
        {
            s.push_back(sensor_positions[i]);
            /// STATE UPDATE
            Mat prediction = KF.predict();

            /// MAKE A MEASUREMENT
            measurement.at<double>(0) = sensor_positions[i].getSensorPosition().x;
            measurement.at<double>(1) = sensor_positions[i].getSensorPosition().y;

            /// MEASUREMENT NOISE SIMULATION
            randn( measurementNoise, Scalar(0),
                   Scalar::all(sqrtf(measurementNoiseCov)));
            noisyMeasurement = measurement + measurementNoise;

            /// MEASUREMENT UPDATE
            Mat estimated = KF.correct(noisyMeasurement);

            cv::Mat u(nInputs,1,CV_64F);
            u.at<double>(0,0) = 0.0 * sqrtf(pow((prevMeasurement.at<double>(0) - measurement.at<double>(0)),2)
                                            + pow((prevMeasurement.at<double>(1) - measurement.at<double>(1)),2));

            /// STORE ALL DATA
            Point noisyPt(prediction.at<double>(0),prediction.at<double>(1));
            Point estimatedPt(estimated.at<double>(0),estimated.at<double>(1));
            Point measuredPt(measurement.at<double>(0),measurement.at<double>(1));
            mousev.push_back(measuredPt);
            kalmanv.push_back(estimatedPt);
            s[i].setSensorPosition(estimatedPt);
            noisyv.push_back(noisyPt);


            //            /// PLOT POINTS
            //#define drawCross( center, color, d )                                 \
            //    line( img, Point( center.x - d, center.y - d ),                \
            //    Point( center.x + d, center.y + d ), color, 1, CV_AA, 0); \
            //    line( img, Point( center.x + d, center.y - d ),                \
            //    Point( center.x - d, center.y + d ), color, 1, CV_AA, 0 )

            //            /// DRAW ALL ON IMAGE
            //            //img = Scalar::all(0);
            //            drawCross( noisyPt, Scalar(255,255,255), 9 );     //WHITE
            //            drawCross( estimatedPt, Scalar(0,0,255), 6 );       //RED
            //            drawCross( measuredPt, Scalar(0,255,0), 3 );        //GREEN


            //            for (int i = 0; i < mousev.size()-1; i++) {
            //                line(img, mousev[i], mousev[i+1], Scalar(0,255,0), 1);
            //            }
            //            for (int i = 0; i < kalmanv.size()-1; i++) {
            //                //circle(img,kalmanv[i],3,Scalar(0,0,255),3);
            //                line(img, kalmanv[i], kalmanv[i+1], Scalar(0,0,255), 1);
            //            }

            //            for (int i = 0; i < noisyv.size()-1; i++) {
            //                line(img, noisyv[i], noisyv[i+1], Scalar(0,0,255), 1);
            //            }


            //            imshow( "Mouse-Kalman", img );
            //            key=cv::waitKey(dt);
            prevMeasurement = measurement;


        }
    }

    return s;



}

bool Common::withinAreaSensorDiameter(Sensor local_sensor, Sensor area_sensor, int diameter)
{

    if(local_sensor.getRelationToAreaSensor()!="-1")
    {
        double dist =calculateDistance(local_sensor.getSensorPosition(),area_sensor.getSensorPosition());
        if(dist <=2*pow(diameter,2))
            return true;
        else
            return false;
    }
    else
        return false;

}

int Common::findDayPart(int unit,int win_size_sec,int f_day_parts)
{
    int lower=0;
    int upper=(60*1440)/(f_day_parts*win_size_sec);


    int count_parts=1;

    while(count_parts<=f_day_parts)
    {
        if(unit>=lower && unit<upper)
            return count_parts-1;
        else
        {
            ++count_parts;
            lower=upper;
            upper=(60*1440)/(f_day_parts*win_size_sec)*count_parts;

        }
    }
}

vector<int> Common::findPatternDayParts(vector<int> &discovered_patterns, vector<int> &sequence_patterns,int f_day_parts)
{
    vector<int> pattern_day_parts;
    for(int i=0;i<discovered_patterns.size();i++)
    {
        for(int j=0;j<sequence_patterns.size();j++)
        {
            if(discovered_patterns[i]==sequence_patterns[j])
            {
                pattern_day_parts.push_back(findDayPart(j,60,f_day_parts));
                break;
            }
        }
    }

    return pattern_day_parts;

}
void Common::createFile(vector<int> &features, ofstream& output,int choice)
{
    if(choice==0)
    {
        for(int i=0;i<features.size();i++)
        {
            output<<features[i]<<endl;
        }
        output.close();
    }
    else
    {
        for(int i=0;i<features.size();i++)
        {
            if(i<features.size()-1)
                output<<features[i]<<",";
            else
                output<<features[i]<<endl;
        }
        // output.close();
    }
}

void Common::sortPatternFeatures(vector<int> &frequent_pattern,vector<int>& discovered_patterns, vector<int> &patterns, vector<vector<int> > &features,vector<vector<int> >& activity_labels,vector<int>& times)
{
    int i, j, flag = 1;    // set flag to 1 to start first pass
    int temp_score;
    int temp_position;
    int tmp_time;
    vector<int> tmp_feature;
    vector<int> tmp_activity_labels;
    int pattern;
    int numLength = frequent_pattern.size();

    for(i = numLength; (i >= 1) && flag; i--)
    {
        flag = 0;

        for (j=(numLength -1); j >0; j--)
        {
            if (frequent_pattern[j-1] < frequent_pattern[j])      // ascending order simply changes to <
            {


                temp_score = frequent_pattern[j];             // swap elements
                frequent_pattern[j] = frequent_pattern[j-1];
                frequent_pattern[j-1] = temp_score;


                temp_position = patterns[j];             // swap elements
                patterns[j] = patterns[j-1];
                patterns[j-1]= temp_position;

                tmp_feature = features[j];             // swap elements
                features[j] = features[j-1];
                features[j-1]= tmp_feature;

                tmp_activity_labels = activity_labels[j];             // swap elements
                activity_labels[j] = activity_labels[j-1];
                activity_labels[j-1]= tmp_activity_labels;


                pattern = discovered_patterns[j];             // swap elements
                discovered_patterns[j] = discovered_patterns[j-1];
                discovered_patterns[j-1]= pattern;

                tmp_time = times[j];             // swap elements
                times[j] = times[j-1];
                times[j-1]= tmp_time;


                flag = 1;               // indicates that a swap occurred.
            }
        }
    }
}

void Common::sortFrequentPatterns(vector<int>& cnt_patterns,vector<int>& patterns)
{
    int i, j, flag = 1;    // set flag to 1 to start first pass
    int temp_score;
    int temp_position;
    int numLength = cnt_patterns.size();

    for(i = numLength; (i >= 1) && flag; i--)
    {
        flag = 0;

        for (j=(numLength -1); j >0; j--)
        {
            if (cnt_patterns[j-1] < cnt_patterns[j])      // ascending order simply changes to <
            {

                temp_score = cnt_patterns[j];             // swap elements
                cnt_patterns[j] = cnt_patterns[j-1];
                cnt_patterns[j-1] = temp_score;

                temp_position = patterns[j];             // swap elements
                patterns[j] = patterns[j-1];
                patterns[j-1]= temp_position;

                flag = 1;               // indicates that a swap occurred.
            }
        }
    }
    return;   //arrays are passed to functions by address; nothing is returned
}

vector<string> Common::getFolderNames(vector<string> folder_file_names)
{

    vector<string> days_folder_names;
    for(int i=0; i<folder_file_names.size();i++)
    {
        std::vector<std::string> elem;
        Common::split(folder_file_names[i],'/',elem);
        days_folder_names.push_back(elem[3]);
    }
    return days_folder_names;


}

vector<string> Common::getFolderFileNames(string directory)
{
    vector<string> days_folder_file_names;
    DIR           *d;
    struct dirent *dir;
    d = opendir(directory.c_str());
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (string(".").compare(dir->d_name) != 0 && string("..").compare(dir->d_name) != 0)
            {
                string directory_file_name=directory;
                directory_file_name.append(dir->d_name);
                directory_file_name.append("/");
                directory_file_name.append(dir->d_name);
                directory_file_name.append(".txt");

                days_folder_file_names.push_back(directory_file_name);
            }
        }

        closedir(d);
    }

    sort(days_folder_file_names.begin(), days_folder_file_names.end());

    return days_folder_file_names;
}
