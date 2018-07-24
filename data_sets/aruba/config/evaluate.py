#!/usr/bin/python
import threading
import Queue
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score, f1_score, precision_score, recall_score, classification_report, confusion_matrix
import numpy as np
import sklearn
import itertools
import matplotlib.pyplot as plt

def train_and_predict_thread(train_data,test_data,result):
    train_features = train_data[:,1:]
    train_labels = train_data[:,0:1]
    
    test_features = test_data[:,1:]
    test_labels = test_data[:,0:1]
	
    model = LogisticRegression()
    
    print "fitting mode..."
    model.fit(train_features, train_labels.ravel())
    
    print "predictiing..."
    predicted_labels = model.predict(test_features)
    
    result.put(predicted_labels)
    
    
def train_and_predict(train_data,test_data):
    result_queue = Queue.Queue()
    t = threading.Thread(target=train_and_predict_thread, args=(train_data, test_data, result_queue))
    t.start()
    t.join()
    result = result_queue.get()
    
    return result.astype(np.ubyte, copy=False)


def train_and_predict0(train_data,test_data):
	
    train_features = train_data[:,1:]
    train_labels = train_data[:,0:1]
    
    test_features = test_data[:,1:]
    test_labels = test_data[:,0:1]
	
    model = LogisticRegression()
    
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


def compute_confusion_matrix(true_labels,predicted_labels,img_file_name, label_type):
    
    class_names = []
    # Compute confusion matrix
    conf_arr = confusion_matrix(true_labels, predicted_labels)
    np.set_printoptions(precision=3)
    
    #0 : with other activity labels
    #1 : without other activity labels
    if label_type == 0:
        class_names = ['Meal_Preparation', 'Relax', 'Eating', 'Work', 'Sleeping', 'Wash_Dishes', 'Bed_to_Toilet', 'Leave_Home', 'Enter_Home', 'Housekeeping', 'Resperate', 'Other']
    elif label_type == 1:
        class_names = ['Meal_Preparation', 'Relax', 'Eating', 'Work', 'Sleeping', 'Wash_Dishes', 'Bed_to_Toilet', 'Leave_Home',' Enter_Home', 'Housekeeping', 'Resperate']
    
    
    print true_labels
    print predicted_labels
    print img_file_name
    print label_type
    conf_arr = conf_arr.astype('float') / conf_arr.sum(axis=1)[:, np.newaxis]


    norm_conf = []
    for i in conf_arr:
        a = 0
        tmp_arr = []
        a = sum(i, 0)
        for j in i:
            tmp_arr.append(float(j)/float(a))
        norm_conf.append(tmp_arr)

    fig = plt.figure()
    plt.clf()
    ax = fig.add_subplot(111)
    ax.set_aspect(0)
    res = ax.imshow(np.array(norm_conf), cmap=plt.cm.jet,
                interpolation='nearest')

    width, height = conf_arr.shape

    for x in xrange(width):
        for y in xrange(height):
            ax.annotate(str(round(conf_arr[x][y],2)), xy=(y, x),
                        horizontalalignment='center',
                        verticalalignment='center')

    cb = fig.colorbar(res)
    tick_marks = np.arange(len(class_names))
    plt.xticks(tick_marks, class_names, rotation=90)
    plt.yticks(tick_marks, class_names)
    plt.tight_layout()
    plt.savefig(img_file_name+'.png', format='png')
    plt.show()    

