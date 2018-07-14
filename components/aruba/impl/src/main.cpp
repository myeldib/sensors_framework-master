#include<within-day-clustering.h>
#include<between-day-clustering.h>
#include<hierarchal-agglomerative-clustering.h>
#include<heatmap.h>
#include<feature-reader.h>
#include<cluster-recognition.h>
#include<model-recognition.h>
#include<logger.h>

#include <Python.h>
#include <numpy/arrayobject.h>
#include <chrono>

//Within day clustering
string sensors_data_file="../data_sets/aruba/config/data";
string preprocessed_data="../data_sets/aruba/config/processed_data";
string with_day_folder_path="../data_sets/aruba/within_day_cluster_result/";
bool is_data_preprocessed = true;
bool success = true;

//home configuration
string home_setup_file="../data_sets/aruba/config/home_setup";
string time_window_config="../data_sets/aruba/config/time-window-config";

//Between day clustering
string betweem_day_folder_path = "../data_sets/aruba/between_day_cluster_result/";

//Hierachal clustering
string hierachal_cluster_folder_path = "../data_sets/aruba/hierachal_cluste_result/";

//patterns statistics
string pattern_stat_folder_path = "../data_sets/aruba/patterns_statistics_result/";
string logger_file = "../data_sets/aruba/log/log.txt";

//recognition using discovered clustered (based on similarity distance)
string cluster_rec_path = "../data_sets/aruba/cluster_recognition/";

//recognition using a trained model
string model_rec_path = "../data_sets/aruba/model_recognition/";

/**
 * @brief runWithinDayPattern
 */
void runWithinDayPattern()
{
  WithinDayClustering* withinDayClustering = new WithinDayClustering(home_setup_file,
                                                                     sensors_data_file,
                                                                     preprocessed_data,
                                                                     time_window_config,
                                                                     with_day_folder_path,
                                                                     is_data_preprocessed);
  withinDayClustering->run();
  withinDayClustering->destroy();

}

/**
 * @brief runBetweenDayPattern
 */
void runBetweenDayPattern()
{
  BetweenDayClustering* betweenDayClustering = new BetweenDayClustering(with_day_folder_path,
                                                                        betweem_day_folder_path,
                                                                        pattern_stat_folder_path,
                                                                        home_setup_file,
                                                                        time_window_config,
                                                                        success);

  if(!success)
    {
      betweenDayClustering->destroy();
      std::exit(0);

    }

  betweenDayClustering->run();

  betweenDayClustering->destroy();
}

/**
 * @brief runHierachalCluster
 */
void runHierachalCluster()
{

  HierarchalAgglomerativeClustering* hierarchalAgglomerativeClustering = new HierarchalAgglomerativeClustering(betweem_day_folder_path,
                                                                                                               hierachal_cluster_folder_path,
                                                                                                               home_setup_file,
                                                                                                               time_window_config,
                                                                                                               success);

  if(!success)
    {
      hierarchalAgglomerativeClustering->destroy();
      std::exit(0);
    }

  hierarchalAgglomerativeClustering->run();


  hierarchalAgglomerativeClustering->destroy();
}

void runClusterRecognition()
{

  ClusterRecognition* clusterRecognition = new ClusterRecognition(hierachal_cluster_folder_path,
                                                                  cluster_rec_path,
                                                                  with_day_folder_path,
                                                                  home_setup_file,
                                                                  time_window_config,
                                                                  success);

  if(!success)
    {
      clusterRecognition->destroy();
      std::exit(0);
    }

  clusterRecognition->run();

  clusterRecognition->destroy();
}

void runModelRecognition()
{

  ModelRecognition* modelRecognition = new ModelRecognition(hierachal_cluster_folder_path,
                                                            model_rec_path,
                                                            with_day_folder_path,
                                                            home_setup_file,
                                                            time_window_config,
                                                            success);

  if(!success)
    {
      modelRecognition->destroy();
      std::exit(0);
    }

  modelRecognition->run();

  modelRecognition->destroy();


}

PyObject *makearray(size_t rows, size_t cols, const float* data)
{
  npy_intp dim[2]{rows, cols};
  return PyArray_SimpleNewFromData(2, dim, NPY_FLOAT, (void*)(data));
}

void buildPythonExample()
{
  char *tmpargv[] = {};
  int tmpargc = 0;


  Py_Initialize();

  import_array();

  PySys_SetArgv(tmpargc, tmpargv);

  char * newpath = new char[model_rec_path.size() + 1];
  std::copy(model_rec_path.begin(), model_rec_path.end(), newpath);
  newpath[model_rec_path.size()] = '\0';

  PyObject *sys = PyImport_ImportModule("sys");
  PyObject *path = PyObject_GetAttrString(sys, "path");
  PyList_Append(path, PyUnicode_FromString(newpath));

  cout<<newpath<<endl;


  PyObject *pName, *pModule, *pDict, *pFunc;
  PyObject *pArgs, *pValue;

  std::string script_name="test_model";
  std::string func_name="multiply";
  std::string model_path = model_rec_path + "/topview_classifier/ppl_counter_topview_rfc.pkl";


  pName = PyString_FromString(script_name.c_str());
  /* Error checking of pName left out */
  float g2f1_[] = { 0.001933,  0.118119,  0.338927, -0.921300,  0.338927, 0.118119, 0.001933};
  float g2f2_[] = {- 0.001933,  - 0.118119,  -0.338927, -0.921300,  -0.338927, -0.118119, 0.001933};

  Mat features1=Mat(7,2,CV_32F,0.0);
  Mat features2=Mat(7,2,CV_32F,0.0);
  Mat result;

  for(int i=0;i<7;i++)
    {
      for(int j=0;j<2;j++)
        {
          features1.at<float>(i,j) = g2f1_[i];
          features2.at<float>(i,j) = g2f2_[i];

        }
    }

  pModule = PyImport_Import(pName);
  Py_DECREF(pName);


  if (pModule != NULL) {
      pFunc = PyObject_GetAttrString(pModule, func_name.c_str());
      /* pFunc is a new reference */

      if (pFunc && PyCallable_Check(pFunc))
        {
          pArgs = PyTuple_New(2);

          PyObject * pyfeatures1 = makearray(features1.rows, features1.cols, (float*)features1.data);
          PyObject * pyfeatures2 = makearray(features2.rows, features2.cols, (float*)features2.data);

          PyTuple_SetItem(pArgs, 0, pyfeatures1);
          PyTuple_SetItem(pArgs, 1, pyfeatures2);

          pValue = PyObject_CallObject(pFunc, pArgs);
          Py_DECREF(pArgs);
          if (pValue != NULL)
            {
              PyArrayObject * retval = (PyArrayObject *) PyArray_FROM_OTF(pValue, NPY_UBYTE, NPY_OUT_ARRAY);
              result.create(retval->dimensions[0], 1, CV_8UC1);

              memcpy(result.data, retval->data, retval->dimensions[0]);
              vector<float> pred;
              cout<<result<<endl;
              //parse mat to 1d vector
              for(int i =0; i<result.rows;i++)
                {
                  int pred_val = static_cast<float>(result.at<uchar>(i,0));
                  pred.push_back(pred_val);
                  cout<<static_cast<float>(result.at<uchar>(i,0))<<endl;
                }

              cout<<"size:"<<pred.size()<<endl;

              for(int j =0; j<pred.size();j++)
                {
                  cout<<pred[j]<<endl;
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

            }
        }
      else
        {
          if (PyErr_Occurred())
            {
              PyErr_Print();
            }
          fprintf(stderr, "{\"error\":\"Cannot find function %s\"}\n", func_name.c_str());
        }
      Py_XDECREF(pFunc);
      Py_DECREF(pModule);
    }
  else
    {
      PyErr_Print();
      fprintf(stderr, "{\"error\":\"Failed to load %s\"}\n", script_name.c_str());
    }

  delete[] newpath;
  Py_Finalize();
}
int main(int argc, char ** argv)
{
  logging::configure({ {"type", "file"}, {"file_name", logger_file.c_str()} });
  //runWithinDayPattern();
  //runBetweenDayPattern();
  //runHierachalCluster();
  //runClusterRecognition();
  runModelRecognition();


  //string test="../result/2010-12-26/locations_duration.txt";

  //HeatMap* hp = new HeatMap(test,home);
  //hp->drawMostOccuppiedTimeIntervalHeatMap();

  //buildPythonExample();


}

