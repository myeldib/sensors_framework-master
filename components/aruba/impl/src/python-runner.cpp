#include<python-runner.h>
using namespace cv;
/**
 * @brief PythonRunner::PythonRunner
 * @param data_dir
 * @param home_setup_file
 * @param time_window_config
 */
PythonRunner::PythonRunner(string data_dir,string home_setup_file, string time_window_config)
{
  logging::INFO("PythonRunner");

  this->data_dir = data_dir;

  logging::INFO("script_location:"+this->data_dir);

  init_(home_setup_file,time_window_config);
}

/**
 * @brief PythonRunner::~PythonRunner
 */
PythonRunner::~PythonRunner()
{
  logging::INFO("~PythonRunner");

  if(home_)
    {
      delete home_;
    }
}

/**
 * @brief PythonRunner::init_
 * @param home_setup_file
 * @param time_window_config
 */
void PythonRunner::init_(string home_setup_file, string time_window_config)
{
  logging::INFO("init_");

  home_ = new Home(home_setup_file,time_window_config);
  home_->readHomeSetup();

}

/**
 * @brief PythonRunner::initPython
 */
void PythonRunner::initPython()
{
  logging::INFO("initPython");

  char *tmpargv[] = {};
  int tmpargc = 0;

  Py_Initialize();
  import_array();

  PySys_SetArgv(tmpargc, tmpargv);

  char * newpath = new char[data_dir.size() + 1];
  std::copy(data_dir.begin(), data_dir.end(), newpath);
  newpath[data_dir.size()] = '\0';

  PyObject *sys = PyImport_ImportModule("sys");
  PyObject *path = PyObject_GetAttrString(sys, "path");
  PyList_Append(path, PyUnicode_FromString(newpath));

  delete[] newpath;
}

/**
 * @brief PythonRunner::finalizePython
 */
void PythonRunner::finalizePython()
{
  logging::INFO("finalizePython");

  Py_Finalize();
}

/**
 * @brief PythonRunner::makeNumpyArray_
 * @param rows
 * @param cols
 * @param data
 * @param array
 */
void PythonRunner::makeNumpyArray_(size_t rows, size_t cols, const float *data,PyObject*& array)
{
  logging::INFO("makeNumpyArray_");

  npy_intp dim[2]{rows, cols};
  array= PyArray_SimpleNewFromData(2, dim, NPY_FLOAT, (void*)(data));

}


/**
 * @brief PythonRunner::predictUsingModel
 * @param script_name
 * @param function_name
 * @param train_sensor_data
 * @param test_sensor_data
 */
void PythonRunner::predictUsingModel(string script_name, string function_name,int num_function_arg, FeatureContainer *train_sensor_data, FeatureContainer *test_sensor_data,vector<int>& actual_activity_label,vector<int>& predicted_activity_labels, PyInterpreterState* interp)
{
  logging::INFO("predictUsingModel");


  predictUsingModel_(script_name,
                     function_name,
                     num_function_arg,
                     train_sensor_data,
                     test_sensor_data,
                     actual_activity_label,
                     predicted_activity_labels,
                     interp);

}

/**
 * @brief PythonRunner::predictUsingModel_
 * @param script_name
 * @param function_name
 * @param train_sensor_data
 * @param test_sensor_data
 * @param predict_activity_labels
 */
void PythonRunner::predictUsingModel_(string script_name, string function_name, int num_function_arg, FeatureContainer *train_sensor_data, FeatureContainer *test_sensor_data, vector<int>& actual_activity_label,vector<int> &predict_activity_labels, PyInterpreterState* interp)
{
  logging::INFO("predictUsingModel_");

  logging::INFO("script_name:"+script_name+"\t"+
                "function_name:"+function_name+"\t"+
                "num_function_arg:"+std::to_string(num_function_arg));

  vector<vector<float> > train_features_vec,test_features_vec;

  prepareTrainData_(train_sensor_data,train_features_vec);
  prepareTestData_(test_sensor_data,actual_activity_label,test_features_vec);


  logging::INFO("train_features_vec_rows:"+std::to_string(train_features_vec.size())+"\t"+
                "train_features_vec_cols:"+std::to_string(train_features_vec[0].size()));

  logging::INFO("test_features_vec_rows:"+std::to_string(test_features_vec.size())+"\t"+
                "test_features_vec_cols:"+std::to_string(test_features_vec[0].size()));

  Mat train_features_mat,test_features_mat;
  convert2DVectorToCvMat_(train_features_vec,train_features_mat);

  logging::INFO("train_features_mat_rows:"+std::to_string(train_features_mat.rows)+"\t"+
                "train_features_mat_cols:"+std::to_string(train_features_mat.cols));

  convert2DVectorToCvMat_(test_features_vec,test_features_mat);

  logging::INFO("test_features_mat_rows:"+std::to_string(test_features_mat.rows)+"\t"+
                "test_features_mat_cols:"+std::to_string(test_features_mat.cols));


  PyObject *train_features_py_obj;
  PyObject *test_features_py_obj;

  makeNumpyArray_(train_features_mat.rows,train_features_mat.cols,(float*)train_features_mat.data,train_features_py_obj);
  makeNumpyArray_(test_features_mat.rows,test_features_mat.cols,(float*)test_features_mat.data,test_features_py_obj);

  bool success =true;
  int thread_id = train_sensor_data->getThreadID();

  cout<<thread_id<<endl;

  useModelFunction_(script_name,function_name,num_function_arg,train_features_py_obj,test_features_py_obj,predict_activity_labels,thread_id,interp,success);

  if(!success)
    {
      logging::INFO("could not predict using a model");
      //return;
    }

  logging::INFO("predict_activity_labels_size:"+std::to_string(predict_activity_labels.size()));
}



/**
 * @brief PythonRunner::useModel_
 * @param script_name
 * @param function_name
 * @param train_features_py_obj
 * @param test_features_py_obj
 * @param success
 */
void PythonRunner::useModelFunction_(string script_name, string function_name, int num_function_arg, PyObject* train_features_py_obj, PyObject* test_features_py_obj, vector<int> &predict_activity_labels,int thread_id, PyInterpreterState* interp, bool &success)
{
  logging::INFO("useModelFunction_");

  sub_interpreter::thread scope(interp);

  initPython();

  PyObject *pName, *pModule, *pFunc;
  PyObject *pArgs, *pValue;
  cv::Mat result;

  //ensure_gil_state gil_scope;


  pName = PyString_FromString(script_name.c_str());

  pModule = PyImport_Import(pName);


  Py_DECREF(pName);

  if (pModule != NULL) {

      pFunc = PyObject_GetAttrString(pModule, function_name.c_str());
      /* pFunc is a new reference */

      if (pFunc && PyCallable_Check(pFunc))
        {
          //
          pArgs = PyTuple_New(num_function_arg);

          PyTuple_SetItem(pArgs, 0, train_features_py_obj);
          PyTuple_SetItem(pArgs, 1, test_features_py_obj);

          pValue = PyObject_CallObject(pFunc, pArgs);

          Py_DECREF(pArgs);
          if (pValue != NULL)
            {
              PyArrayObject * retval = (PyArrayObject *) PyArray_FROM_OTF(pValue, NPY_UBYTE, NPY_OUT_ARRAY);
              result.create(retval->dimensions[0], 1, CV_8UC1);

              memcpy(result.data, retval->data, retval->dimensions[0]);

              //parse mat to 1d vector
              for(int i =0; i<result.rows;i++)
                {
                  int pred_val = static_cast<int>(result.at<uchar>(i,0));
                  predict_activity_labels.push_back(pred_val);
                }

              Py_DECREF(pValue);
              Py_DECREF(retval);
            }
          else
            {
              Py_DECREF(pFunc);
              Py_DECREF(pModule);
              PyErr_Print();
              fprintf(stderr,"{\"error\":\"Call failed\"}\n");
              success = false;
            }
        }
      else
        {
          if (PyErr_Occurred())
            {
              PyErr_Print();
            }
          fprintf(stderr, "{\"error\":\"Cannot find function %s\"}\n", function_name.c_str());
          success = false;
        }
      Py_XDECREF(pFunc);
      Py_DECREF(pModule);
    }
  else
    {
      PyErr_Print();
      fprintf(stderr, "{\"error\":\"Failed to load %s\"}\n", script_name.c_str());
      success = false;
    }

  logging::INFO("useModelFunction_ Finished");
}
/**
 * @brief PythonRunner::prepareTestData_
 * @param test_sensor_data
 * @param test_features
 */
void PythonRunner::prepareTestData_(FeatureContainer *test_sensor_data, vector<int>& actual_activity_label,vector<vector<float> > &test_features)
{
  logging::INFO("prepareTestData_");

  vector<vector<float> > sensor_durations = test_sensor_data->getSensorDurations();
  vector<float> time_index_per_pattern = test_sensor_data->getTimeIndexPerPattern();
  vector<vector<int> > most_active_sensors = test_sensor_data->getActiveSensorsPerPattern();
  vector<string> activity_labels = test_sensor_data->getActivityLabel();

  for(int i = 0; i<sensor_durations.size();i++)
    {
      string activity_label =activity_labels[i];
      vector<float> feature_row;

      if(activity_label=="-")   activity_label="";

      int time_index = time_index_per_pattern[i];
      vector<int> active_sensors = most_active_sensors[i];
      vector<float> sensor_duration = sensor_durations[i];

      feature_row.push_back(home_->getActivityLabelStringIntMap().at(activity_label));
      feature_row.push_back(time_index);

      for(int j = 0; j<active_sensors.size();j++)
        feature_row.push_back(active_sensors[j]);

      for(int j = 0; j<sensor_duration.size();j++)
        feature_row.push_back(sensor_duration[j]);

      test_features.push_back(feature_row);
      actual_activity_label.push_back(home_->getActivityLabelStringIntMap().at(activity_label));
    }

  logging::INFO("test_features_size:"+std::to_string(test_features.size()));

}
/**
 * @brief PythonRunner::prepareTrainData_
 * @param train_sensor_data
 * @param train_features
 */
void PythonRunner::prepareTrainData_(FeatureContainer *train_sensor_data, vector<vector<float> > &train_features)
{
  logging::INFO("prepareTrainData_");

  vector<vector<float> > avg_sensor_durations = train_sensor_data->getAverageSensorDurationsPerPattern();
  vector<int> time_index_per_pattern = train_sensor_data->getMostAssignedTimeIndexPerPatternInHourIndex();
  vector<vector<int> > most_active_sensors = train_sensor_data->getActiveSensorsPerPattern();
  vector<string> activity_labels = train_sensor_data->getActivityLabel();
  vector<int> most_activity_labels = train_sensor_data->getMostCommonActivityLabelPerPattern();


  vector<int> sequence_patterns = train_sensor_data->getSequencePatterns();
  vector<int> discovered_patterns = train_sensor_data->getDiscoveredPatterns();

  for(int i = 0; i<sequence_patterns.size();i++)
    {
      vector<float> feature_row;

      int discovered_patterns_index = find(discovered_patterns.begin(), discovered_patterns.end(), sequence_patterns[i]) - discovered_patterns.begin();

      string activity_label = activity_labels[i];
      if(activity_label=="-")   activity_label="";
      int activit_label_index = most_activity_labels[discovered_patterns_index];
      //int activit_label_index =home_->getActivityLabelStringIntMap().at(activity_label);
      vector<float> sensor_duration = avg_sensor_durations[discovered_patterns_index];
      vector<int> active_sensors = most_active_sensors[discovered_patterns_index];
      int time_index = time_index_per_pattern[discovered_patterns_index];


      feature_row.push_back(activit_label_index);
      feature_row.push_back(time_index);

      for(int k = 0; k<active_sensors.size();k++)
        feature_row.push_back(active_sensors[k]);

      for(int k = 0; k<sensor_duration.size();k++)
        feature_row.push_back(sensor_duration[k]);

      train_features.push_back(feature_row);
    }

  logging::INFO("train_features_size:"+std::to_string(train_features.size()));
}
/**
 * @brief PythonRunner::convert2DVectorToCvMat_
 * @param vec
 * @param mat
 */
void PythonRunner::convert2DVectorToCvMat_(vector<vector<float> > &vec, Mat &mat)
{
  logging::INFO("convert2DVectorToCvMat_");

  int rows = vec.size();
  int cols = vec[0].size();

  mat =Mat(rows,cols,CV_32F,0.0);

  for(int i=0;i<rows;i++)
    {
      vector<float> elem = vec[i];

      for(int j=0;j<cols;j++)
        {
          mat.at<float>(i,j) = elem[j];
        }
    }
}

/**
 * @brief PythonRunner::convert1DVectorToCvMat_
 * @param vec
 * @param mat
 */
void PythonRunner::convert1DVectorToCvMat_(vector<int> &vec, Mat &mat)
{

  logging::INFO("convert1DVectorToCvMat_");

  int rows = vec.size();
  int cols = 1;

  mat =Mat(rows,cols,CV_32F,0.0);

  for(int i=0;i<rows;i++)
    {
      mat.at<float>(i,0) =  vec[i];
    }

}
/**
 * @brief PythonRunner::computeAccuracy
 * @param script_name
 * @param function_name
 * @param num_function_arg
 * @param actual_activity_labels
 * @param predicted_activity_labels
 * @param accuracy_info
 */
void PythonRunner::computeAccuracy(string script_name, string function_name, int num_function_arg, vector<int> &actual_activity_labels, vector<int> &predicted_activity_labels, string &accuracy_info)
{
  logging::INFO("computeAccuracy");
  bool success = true;
  computeAccuracy_(script_name,function_name,num_function_arg,actual_activity_labels,predicted_activity_labels,accuracy_info,success);

  if(!success)
    {
      logging::INFO("could not compute accuracy");
    }
}

/**
 * @brief PythonRunner::generateConfusionMatrix
 * @param script_name
 * @param function_name
 * @param num_function_arg
 * @param actual_activity_labels
 * @param predicted_activity_labels
 * @param img_file_name
 * @param type
 */
void PythonRunner::generateConfusionMatrix(string script_name, string function_name, int num_function_arg, vector<int> &actual_activity_labels, vector<int> &predicted_activity_labels, string img_file_name, Constants::Result_Type result_type)
{
  logging::INFO("generateConfusionMatrix");

  bool success = true;
  generateConfusionMatrix_(script_name,function_name,num_function_arg,actual_activity_labels,predicted_activity_labels,img_file_name,result_type,success);

  if(!success)
    {
      logging::INFO("could not generate confusion matrix");
    }

}

void PythonRunner::generateConfusionMatrix_(string script_name, string function_name, int num_function_arg, vector<int> &actual_activity_labels, vector<int> &predicted_activity_labels, string img_file_name, Constants::Result_Type result_type, bool &success)
{

  logging::INFO("generateConfusionMatrix_");

  logging::INFO("actual_activity_labels_size:"+std::to_string(actual_activity_labels.size())+"\t"+
                "predicted_activity_labels_size:"+std::to_string(predicted_activity_labels.size()));

  Mat actual_labels_mat,predicted_labels_mat;


  for(int i =0; i<predicted_activity_labels.size();i++)
    {
      if(actual_activity_labels[i] == 8)
        {
          predicted_activity_labels[i] = 8;
        }
    }

  convert1DVectorToCvMat_(actual_activity_labels,actual_labels_mat);

  logging::INFO("actual_labels_mat_rows:"+std::to_string(actual_labels_mat.rows)+"\t"+
                "actual_labels_mat_cols:"+std::to_string(actual_labels_mat.cols));


  convert1DVectorToCvMat_(predicted_activity_labels,predicted_labels_mat);

  logging::INFO("predicted_labels_mat_rows:"+std::to_string(predicted_labels_mat.rows)+"\t"+
                "predicted_labels_mat_cols:"+std::to_string(predicted_labels_mat.cols));


  PyObject *actual_labels_py_obj;
  PyObject *predicted_labels_py_obj;
  int label_type = -1;

  makeNumpyArray_(actual_labels_mat.rows,actual_labels_mat.cols,(float*)actual_labels_mat.data,actual_labels_py_obj);
  makeNumpyArray_(predicted_labels_mat.rows,predicted_labels_mat.cols,(float*)predicted_labels_mat.data,predicted_labels_py_obj);

  //0: with othe activity label
  //1: without other activity label
  if(result_type == Constants::Result_Type::with_other_activity_label)
    {
      img_file_name = img_file_name+"cm_with_other_activity_label";
      label_type = 0;
    }
  else if(result_type == Constants::Result_Type::without_other_activity_label)
    {
      img_file_name = img_file_name+"cm_without_other_activity_label";
      label_type = 1;
    }

  useConfusionMatrix_(script_name,function_name,num_function_arg,actual_labels_py_obj,predicted_labels_py_obj,img_file_name,label_type,success);
}
/**
 * @brief PythonRunner::useConfusionMatrix_
 * @param script_name
 * @param function_name
 * @param num_function_arg
 * @param actual_labels_py_obj
 * @param predicted_labels_py_obj
 * @param img_file_name
 * @param result_type
 * @param success
 */
void PythonRunner::useConfusionMatrix_(string script_name, string function_name, int num_function_arg, PyObject *actual_labels_py_obj, PyObject *predicted_labels_py_obj, string img_file_name, int result_type, bool &success)
{
  logging::INFO("useConfusionMatrix_");

  PyObject *pName, *pModule, *pFunc;
  PyObject *pArgs, *img_file_name_py_obj,*result_type_py_obj,*pValue;

  pName = PyString_FromString(script_name.c_str());
  pModule = PyImport_Import(pName);
  Py_DECREF(pName);

  if (pModule != NULL) {
      pFunc = PyObject_GetAttrString(pModule, function_name.c_str());
      /* pFunc is a new reference */

      if (pFunc && PyCallable_Check(pFunc))
        {
          img_file_name_py_obj = PyUnicode_FromString(img_file_name.c_str());
          result_type_py_obj = PyInt_FromLong(result_type);

          pArgs = PyTuple_New(num_function_arg);

          PyTuple_SetItem(pArgs, 0, actual_labels_py_obj);
          PyTuple_SetItem(pArgs, 1, predicted_labels_py_obj);
          PyTuple_SetItem(pArgs, 2, img_file_name_py_obj);
          PyTuple_SetItem(pArgs, 3, result_type_py_obj);


          pValue = PyObject_CallObject(pFunc, pArgs);

          Py_DECREF(pArgs);
          if (pValue != NULL) {
              if(!PyInt_AsLong(pValue))
                {
                  printf("{\"error\":\"Result of call: %ld\"}\n", PyInt_AsLong(pValue));
                }
              Py_DECREF(pValue);
            }
          else
            {
              Py_DECREF(pFunc);
              Py_DECREF(pModule);
              PyErr_Print();
              fprintf(stderr,"{\"error\":\"Call failed\"}\n");
              success = false;
            }
        }
      else
        {
          if (PyErr_Occurred())
            {
              PyErr_Print();
            }
          fprintf(stderr, "{\"error\":\"Cannot find function %s\"}\n", function_name.c_str());
          success = false;
        }
      Py_XDECREF(pFunc);
      Py_DECREF(pModule);
    }
  else
    {
      PyErr_Print();
      fprintf(stderr, "{\"error\":\"Failed to load %s\"}\n", script_name.c_str());
      success = false;
    }
}
/**
 * @brief PythonRunner::computeAccuracy_
 * @param script_name
 * @param function_name
 * @param num_function_arg
 * @param actual_activity_labels
 * @param predicted_activity_labels
 * @param accuracy_info
 * @param success
 */
void PythonRunner::computeAccuracy_(string script_name, string function_name, int num_function_arg, vector<int> &actual_activity_labels, vector<int> &predicted_activity_labels, string &accuracy_info, bool &success)
{
  logging::INFO("computeAccuracy_");

  logging::INFO("actual_activity_labels_size:"+std::to_string(actual_activity_labels.size())+"\t"+
                "predicted_activity_labels_size:"+std::to_string(predicted_activity_labels.size()));


  Mat actual_labels_mat,predicted_labels_mat;
  convert1DVectorToCvMat_(actual_activity_labels,actual_labels_mat);

  logging::INFO("actual_labels_mat_rows:"+std::to_string(actual_labels_mat.rows)+"\t"+
                "actual_labels_mat_cols:"+std::to_string(actual_labels_mat.cols));

  convert1DVectorToCvMat_(predicted_activity_labels,predicted_labels_mat);

  logging::INFO("predicted_labels_mat_rows:"+std::to_string(predicted_labels_mat.rows)+"\t"+
                "predicted_labels_mat_cols:"+std::to_string(predicted_labels_mat.cols));


  PyObject *actual_labels_py_obj;
  PyObject *predicted_labels_py_obj;

  makeNumpyArray_(actual_labels_mat.rows,actual_labels_mat.cols,(float*)actual_labels_mat.data,actual_labels_py_obj);
  makeNumpyArray_(predicted_labels_mat.rows,predicted_labels_mat.cols,(float*)predicted_labels_mat.data,predicted_labels_py_obj);

  UseAccuracyFucntion_(script_name,function_name,num_function_arg,actual_labels_py_obj,predicted_labels_py_obj,accuracy_info,success);

}
/**
 * @brief PythonRunner::UseAccuracyFucntion_
 * @param script_name
 * @param function_name
 * @param num_function_arg
 * @param actual_labels_py_obj
 * @param predict_labels_py_obj
 * @param accuracy_info
 * @param success
 */
void PythonRunner::UseAccuracyFucntion_(string script_name, string function_name, int num_function_arg, PyObject *actual_labels_py_obj, PyObject *predicted_labels_py_obj, string &accuracy_info, bool &success)
{
  logging::INFO("computeAccuracy_");

  PyObject *pName, *pModule, *pFunc;
  PyObject *pArgs, *pValue;

  pName = PyString_FromString(script_name.c_str());
  pModule = PyImport_Import(pName);
  Py_DECREF(pName);

  if (pModule != NULL) {
      pFunc = PyObject_GetAttrString(pModule, function_name.c_str());
      /* pFunc is a new reference */

      if (pFunc && PyCallable_Check(pFunc))
        {
          //
          pArgs = PyTuple_New(num_function_arg);

          PyTuple_SetItem(pArgs, 0, actual_labels_py_obj);
          PyTuple_SetItem(pArgs, 1, predicted_labels_py_obj);

          pValue = PyObject_CallObject(pFunc, pArgs);

          Py_DECREF(pArgs);
          if (pValue != NULL)
            {
              char* accuracy_info_char=PyString_AsString(pValue);
              accuracy_info=accuracy_info_char;

              Py_DECREF(pValue);
            }
          else
            {
              Py_DECREF(pFunc);
              Py_DECREF(pModule);
              PyErr_Print();
              fprintf(stderr,"{\"error\":\"Call failed\"}\n");
              success = false;
            }
        }
      else
        {
          if (PyErr_Occurred())
            {
              PyErr_Print();
            }
          fprintf(stderr, "{\"error\":\"Cannot find function %s\"}\n", function_name.c_str());
          success = false;
        }
      Py_XDECREF(pFunc);
      Py_DECREF(pModule);

    }
  else
    {
      PyErr_Print();
      fprintf(stderr, "{\"error\":\"Failed to load %s\"}\n", script_name.c_str());
      success = false;
    }

}
