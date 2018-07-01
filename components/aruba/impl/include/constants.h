#ifndef CONSTANTS
#define CONSTANTS
#include<string>

using namespace std;

namespace Constants {

enum  Cluster_Type { within_day_cluster,between_day_cluster, hierarchal_cluster,cluster_recognition};
static const double ONE_SECOND_IN_MELI=1000.0;
static const int FULL_DAY_IN_HOURS=24;
static const int FULL_HOUR_IN_SECONDS=3600;
static const string OPEN="OPEN";
static const string CLOSE="CLOSE";
static const string ON="ON";
static const string OFF="OFF";
static const string TEMP_SENSOR="T0";
static const string BEGIN_ACTIVITY="begin";
static const string END_ACTIVITY="end";
static const char DATE_SEPARATOR='T';
static const char MILI_SECOND_SEPARATOR='Z';
static const char COMMA_SEPARATOR=',';
static const char EQUAL_SEPARATOR='=';
static const char DIRECTOR_SEPARATOR='/';
static const int ONE_MILI_SECOND=1;
static const int EXP_ONE_MILI_SECOND=10;
static const int MIN_CLUSTER_COUNT=1;
#define COUT if (0) cout

}



#endif // CONSTANTS

