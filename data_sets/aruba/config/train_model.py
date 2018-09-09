#!/usr/bin/python
import threading
import Queue
from sklearn.neighbors import KNeighborsClassifier
from sklearn.decomposition import LatentDirichletAllocation
from sklearn.linear_model import LogisticRegression,Perceptron,PassiveAggressiveClassifier,SGDClassifier
from sklearn.ensemble import RandomForestClassifier,ExtraTreesClassifier,BaggingClassifier,BaggingRegressor,RandomForestRegressor
from sklearn.metrics import accuracy_score, f1_score, precision_score, recall_score, classification_report, confusion_matrix
import numpy as np
import sklearn

def train_and_predict_thread(train_data,test_data,result):
    train_features = train_data[:,1:]
    train_labels = train_data[:,0:1]
    
    test_features = test_data[:,1:]
    test_labels = test_data[:,0:1]
	
    model =  LogisticRegression(solver='sag',n_jobs=-1)
    #model = Perceptron(n_jobs=-1,penalty='l2')
    
    print "fitting mode..."
    model.fit(train_features, train_labels.ravel())
    
    print "predictiing..."
    predicted_labels = model.predict(test_features)
    
    result.put(predicted_labels)
    
    
def train_and_predict_thread_runner(train_data,test_data):
    result_queue = Queue.Queue()
    t = threading.Thread(target=train_and_predict_thread, args=(train_data, test_data, result_queue))
    t.start()
    t.join()
    result = result_queue.get()
    
    return result.astype(np.ubyte, copy=False)


def train_and_predict(train_data,test_data):
	
    train_features = train_data[:,1:]
    train_labels = train_data[:,0:1]
    
    test_features = test_data[:,1:]
    test_labels = test_data[:,0:1]
	
    model =  LogisticRegression(solver='sag',n_jobs=-1)
    #model = Perceptron(n_jobs=-1,penalty='l2',alpha=0.00001)
    
    print "fitting mode..."
    model.fit(train_features, train_labels.ravel())
    
    print "predictiing..."
    predicted_labels = model.predict(test_features)
	
    return predicted_labels.astype(np.ubyte, copy=False)

def compute_accuracy(actual_labels,predicted_labels):
    
    accuracy_info = ""
    
    acc=sklearn.metrics.accuracy_score(actual_labels, predicted_labels);
    f1 =sklearn.metrics.f1_score(actual_labels, predicted_labels,average='weighted')
    classification_report = sklearn.metrics.classification_report(actual_labels, predicted_labels)
    
    accuracy_info+="accuracy:"+str(acc)+'\n'
    accuracy_info+="F1:"+str(f1)+'\n'
    accuracy_info+=classification_report

    
    return accuracy_info

