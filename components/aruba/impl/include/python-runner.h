#ifndef PYTHONRUNNER_H
#define PYTHONRUNNER_H
#include <feature-container.h>
#include <home.h>
#include <Python.h>
#include <numpy/arrayobject.h>
#include <opencv2/opencv.hpp>
#include<logger.h>

// acquire GIL
class ensure_gil_state
{
public:
  ensure_gil_state()
  {
    logging::INFO("ensure_gil_state");
    _state = PyGILState_Ensure();
  }

  ~ensure_gil_state()
  {
    logging::INFO("~ensure_gil_state");
    PyGILState_Release(_state);
  }

private:
  PyGILState_STATE _state;
};

// acquire and release the GIL
struct gil_lock
{
  gil_lock()
  {
     logging::INFO("gil_lock");
    PyEval_AcquireLock();
  }

  ~gil_lock()
  {
     logging::INFO("~gil_lock");
    PyEval_ReleaseLock();
  }
};

// restore the thread state when the object goes out of scope
class restore_tstate
{
public:

  restore_tstate()
  {
     logging::INFO("restore_tstate");
    _ts = PyThreadState_Get();
  }

  ~restore_tstate()
  {
    logging::INFO("~restore_tstate");
    PyThreadState_Swap(_ts);
  }

private:
  PyThreadState* _ts;
};

// swap the current thread state with ts, restore when the object goes out of scope
class swap_tstate
{
public:

  swap_tstate(PyThreadState* ts)
  {
    logging::INFO("swap_tstate");
    _ts = PyThreadState_Swap(ts);
  }

  ~swap_tstate()
  {
    logging::INFO("~swap_tstate");
    PyThreadState_Swap(_ts);
  }

private:
  PyThreadState* _ts;
};

// create new thread state for interpreter interp, clean up on destruction
class thread_state
{
public:

  thread_state(PyInterpreterState* interp)
  {
    logging::INFO("thread_state");
    _ts = PyThreadState_New(interp);
  }

  ~thread_state()
  {
    logging::INFO("~thread_state");
    if( _ts )
      {
        //PyThreadState_Clear(_ts);
        PyThreadState_Delete(_ts);

        _ts = nullptr;
      }
  }

  operator PyThreadState*()
  {
    logging::INFO("PyThreadState");
    return _ts;
  }

private:
  PyThreadState* _ts;
};

// represent a sub interpreter
class sub_interpreter
{
public:

  // perform the necessary setup and cleanup for a new thread running using a specific interpreter
  struct thread
  {
    gil_lock _lock;
    thread_state _state;
    swap_tstate _swap;

    thread(PyInterpreterState* interp) :
      _state(interp),
      _swap(_state)
    {
      logging::INFO("thread");
    }
  };

  sub_interpreter()
  {
    logging::INFO("sub_interpreter");
    restore_tstate restore;

    _ts = Py_NewInterpreter();
  }

  ~sub_interpreter()
  {
    logging::INFO("~sub_interpreter");
    if( _ts )
      {
        swap_tstate sts(_ts);

        Py_EndInterpreter(_ts);
      }
  }

  PyInterpreterState* interp()
  {
    logging::INFO("interp");
    return _ts->interp;
  }

private:
  PyThreadState* _ts;
};

class PythonRunner{

public:
  PythonRunner(string data_dir,string home_setup_file, string time_window_config);
  ~PythonRunner();
  void predictUsingModel(string script_name,
                         string function_name,
                         int num_function_arg,
                         FeatureContainer* train_sensor_data,
                         FeatureContainer* test_sensor_data,
                         vector<int>& actual_activity_label,
                         vector<int>& predicted_activity_labels,
                         PyInterpreterState* interp);
  void computeAccuracy(string script_name,
                       string function_name,
                       int num_function_arg,
                       vector<int>& actual_activity_labels,
                       vector<int>& predicted_activity_labels,
                       string& accuracy_info);
  void initPython();
  void finalizePython();
private:
  void init_(string home_setup_file, string time_window_config);
  void makeNumpyArray_(size_t rows,
                       size_t cols,
                       const float* data,
                       PyObject*& array);
  void convert2DVectorToCvMat_(vector<vector<float> >& vec,
                               cv::Mat& mat);
  void convert1DVectorToCvMat_(vector<int>& vec,
                               cv::Mat& mat);
  void predictUsingModel_(string script_name,
                          string function_name,
                          int num_function_arg,
                          FeatureContainer* train_sensor_data,
                          FeatureContainer* test_sensor_data,
                          vector<int>& actual_activity_label,
                          vector<int>& predict_activity_labels,
                          PyInterpreterState* interp);
  void prepareTrainData_(FeatureContainer* train_sensor_data,vector<vector<float> >& train_features);
  void prepareTestData_(FeatureContainer* test_sensor_data,vector<int>& actual_activity_label,vector<vector<float> >& test_features);
  void useModelFunction_(string script_name,
                         string function_name,
                         int num_function_arg,
                         PyObject* train_features_py_obj,
                         PyObject* test_features_py_obj,
                         vector<int> &predict_activity_labels,
                         int thread_id,
                         PyInterpreterState* interp,
                         bool &success);
  void UseAccuracyFucntion_(string script_name,
                            string function_name,
                            int num_function_arg,
                            PyObject* actual_labels_py_obj,
                            PyObject* predicted_activity_py_obj,
                            string& accuracy_info,
                            bool &success);
  void computeAccuracy_(string script_name,
                        string function_name,
                        int num_function_arg,
                        vector<int>& actual_activity_labels,
                        vector<int>& predicted_activity_labels,
                        string& accuracy_info,
                        bool& success);

private:
  string data_dir;
  Home* home_;

};

#endif // PYTHONRUNNER_H
