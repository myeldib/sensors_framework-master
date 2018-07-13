#include<python-runner.h>
using namespace cv;
/**
 * @brief PythonRunner::PythonRunner
 * @param data_dir
 */
PythonRunner::PythonRunner(string data_dir)
{
  logging::INFO("PythonRunner");

  this->data_dir = data_dir;

  init_();
}

/**
 * @brief PythonRunner::~PythonRunner
 */
PythonRunner::~PythonRunner()
{
  logging::INFO("~PythonRunner");
  Py_Finalize();
}

/**
 * @brief PythonRunner::init_
*/
void PythonRunner::init_()
{
  logging::INFO("init_");

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
 * @brief PythonRunner::makeNumpyArray_
 * @param rows
 * @param cols
 * @param data
 * @param array
 */
void PythonRunner::makeNumpyArray_(size_t rows, size_t cols, const float *data, PyObject *array)
{
  logging::INFO("makeNumpyArray_");

  npy_intp dim[2]{rows, cols};
  array = PyArray_SimpleNewFromData(2, dim, NPY_FLOAT, (void*)(data));

}

/**
 * @brief PythonRunner::predictUsingModel
 * @param script_name
 * @param function_name
 * @param train_sensor_data
 * @param test_sensor_data
 */
void PythonRunner::predictUsingModel(string script_name, string function_name,int num_function_arg, FeatureContainer *train_sensor_data, FeatureContainer *test_sensor_data,vector<float>& predicted_activity_labels)
{
  logging::INFO("predictUsingModel");


  predictUsingModel_(script_name,
                     function_name,
                     num_function_arg,
                     train_sensor_data,
                     test_sensor_data,
                     predicted_activity_labels);

}

/**
 * @brief PythonRunner::predictUsingModel_
 * @param script_name
 * @param function_name
 * @param train_sensor_data
 * @param test_sensor_data
 * @param predict_activity_labels
 */
void PythonRunner::predictUsingModel_(string script_name, string function_name, int num_function_arg, FeatureContainer *train_sensor_data, FeatureContainer *test_sensor_data, vector<float> &predict_activity_labels)
{
  logging::INFO("predictUsingModel_");

  vector<vector<float> > train_features_vec,test_features_vec;

  prepareTrainData_(train_sensor_data,train_features_vec);
  prepareTestData_(test_sensor_data,test_features_vec);

  Mat train_features_mat,test_features_mat;
  convert2DVectorToCvMat_(train_features_vec,train_features_mat);
  convert2DVectorToCvMat_(test_features_vec,test_features_mat);

  PyObject *train_features_py_obj;
  PyObject *test_features_py_obj;

  makeNumpyArray_(train_features_mat.rows,train_features_mat.cols,(float*)train_features_mat.data,train_features_py_obj);
  makeNumpyArray_(test_features_mat.rows,test_features_mat.cols,(float*)test_features_mat.data,test_features_py_obj);

  bool success =true;
  useModel_(script_name,function_name,num_function_arg,train_features_py_obj,test_features_py_obj,predict_activity_labels,success);

}

/**
 * @brief PythonRunner::useModel_
 * @param script_name
 * @param function_name
 * @param train_features_py_obj
 * @param test_features_py_obj
 * @param success
 */
void PythonRunner::useModel_(string script_name, string function_name, int num_function_arg, PyObject* train_features_py_obj, PyObject* test_features_py_obj, vector<float> &predict_activity_labels, bool &success)
{
  logging::INFO("useModel_");

  PyObject *pName, *pModule, *pFunc;
  PyObject *pArgs, *pValue;
  cv::Mat result;

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
                  int pred_val = static_cast<float>(result.at<uchar>(i,0));
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
}
/**
 * @brief PythonRunner::prepareTestData_
 * @param test_sensor_data
 * @param test_features
 */
void PythonRunner::prepareTestData_(FeatureContainer *test_sensor_data, vector<vector<float> > &test_features)
{
  logging::INFO("prepareTestData_");
}
/**
 * @brief PythonRunner::prepareTrainData_
 * @param train_sensor_data
 * @param train_features
 */
void PythonRunner::prepareTrainData_(FeatureContainer *train_sensor_data, vector<vector<float> > &train_features)
{
  logging::INFO("prepareTrainData_");
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
