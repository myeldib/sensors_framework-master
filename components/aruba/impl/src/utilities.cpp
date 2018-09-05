/*
 * utilities.cpp
 *
 *  Created on: 2017-03-14
 *      Author: Mohamed Eldib
 * This class contains utility functionsed used by other classes
 */

#include<utilities.h>

/**
 * @brief Common::split
 * @param s: string to be splitted
 * @param delim: delimiter character
 * @param elems: vector of splitted elements
 * @return retun vector of splitted elements
 */
vector<string>& Common::split(const string &s, char delim, vector<string> &elems)
{

  stringstream ss(s);
  string item;
  while (getline(ss, item, delim))
    {
      elems.push_back(item);
    }
  return elems;

}

/**
 * @brief Common::getMostCommonElementInVector
 * @param v
 * @param element
 */
void Common::getMostCommonElementInVector(vector<int> &v, int &element)
{
  map<int,int> m;
  int max = 0;
  int most_common = -1;
  for (std::vector<int>::iterator vi = v.begin(); vi != v.end(); vi++)
    {
      m[*vi]++;
      if (m[*vi] > max)
        {
          max = m[*vi];
          most_common = *vi;
        }
    }

  element = most_common;
}
/**
 * @brief Common::computeNumElementsPerThread
 * @param num_elements
 * @param num_threads
 * @param elemnts_per_thread
 */
void Common::computeNumElementsPerThread(int num_elements, int& num_threads, vector<int> &elemnts_per_thread)
{
  logging::INFO("computeNumElementsPerThread");

  logging::INFO("num_elements:"+std::to_string(num_elements));

  if(num_threads>num_elements)
    {
      num_threads=1;
    }

  //compute number of elements to be included for each thread
  float num_elements_per_thread= (1.0*num_elements)/(1.0*num_threads);
  float left_over=0;


  logging::INFO("num_elements_per_thread:"+std::to_string(num_elements_per_thread));

  //if each thread will have odd number of elemnts
  if(num_elements%num_threads!=0)
    {
      //compute left over to be included in the last thread
      left_over= num_elements_per_thread-(int)num_elements_per_thread;

      logging::INFO("left_over:"+std::to_string(left_over));

    }

  for(int i = 0; i<num_threads;i++)
    {

      //add sub elements to last thread
      //add left over of elements if exist to last thread
      if(i==num_threads-1)
        {
          int sum_last_sub_element = num_elements_per_thread+(left_over*num_threads);

          elemnts_per_thread.push_back(sum_last_sub_element);

          logging::INFO("sum_last_sub_element:"+std::to_string(sum_last_sub_element));
        }
      else
        {
          //add sub elements to each thread
          elemnts_per_thread.push_back(num_elements_per_thread);

        }

    }

}

/**
 * @brief Common::buildOutputFile
 * @param folder_name
 * @param file_name
 * @return output file name
 */
string Common::buildOutputFile(string folder_path,string folder_name, string file_name)
{
  return folder_path+folder_name+"/"+file_name+".txt";
}


/**
 * @brief Common::extractDayFromFileName
 * @param file_name: path to daily instance sensor events file
 * @return day name from path to file
 */
string Common::extractDayFromFileName(string file_name)
{
  vector<string> elem1,elem2;
  Common::split(file_name,Constants::DIRECTOR_SEPARATOR,elem1);
  Common::split(elem1[elem1.size()-1],'.',elem2);

  return elem2[0];
}

/**
 * @brief Common::millis_from_date
 * @param s: string date
 * @return time in miliseconds
 */
uint64_t Common::millis_from_date(const string& s)
{
  boost::posix_time::ptime pt;
  istringstream is(s);
  auto* f = new boost::posix_time::time_input_facet("%Y-%m-%dT%H:%M:%S%FZ");
  locale loc(locale(""), f);
  is.imbue(loc);
  is >> pt;
  boost::posix_time::ptime timet_start(boost::gregorian::date(1970,1,1));
  boost::posix_time::time_duration diff = pt - timet_start;
  return diff.total_milliseconds();
}

/**
 * @brief Common::date_from_millis
 * @param ms: time in miliseconds
 * @return date as string
 */
string Common::date_from_millis(uint64_t ms)
{
  static const boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
  boost::posix_time::time_facet * facet = new boost::posix_time::time_facet("%Y-%m-%dT%H:%M:%S.%fZ");
  ostringstream stream;
  stream.imbue(locale(stream.getloc(), facet));
  stream << epoch + boost::posix_time::milliseconds(ms);;
  return stream.str();
}

/**
 * @brief Common::eraseDuplicatesFromVector
 * @param vec: sort and erase duplicate entries in a vector
 */
void Common::eraseDuplicatesFromVector(vector<int>& vec)
{
  sort( vec.begin(), vec.end() );
  vec.erase( unique( vec.begin(), vec.end() ), vec.end() );
}

/**
 * @brief Common::countVectorElements
 * @param v: vector which contains duplicate elements
 * @return map where each key is the element in v, and the value is the count of the element
 */
map<int,int> Common::countVectorElements(vector<int> v)
{
  map<int,int> dup;

  for_each( v.begin(), v.end(), [&dup]( int val ){ dup[val]++; } );

  return dup;
}

/**
 * @brief Common::deleteFolder
 * @param folder_path
 * @return true if folder is deleted
 */
bool Common::deleteFolder(const string &folder_path)
{

  filesystem::remove_all(folder_path.c_str());
  return true;
}
/**
 * @brief Common::createFolder
 * @param folder_path
 * @return return true if folder created, otherwise return false
 */
bool Common::createFolder(const string &folder_path)
{

  filesystem::path dir(folder_path);
  if(filesystem::create_directory(dir))
    {
      return true;
    }
  else
    {
      return false;
    }

}

/**
 * @brief Common::getFolderFileNames
 * @param directory: directory with folders and files
 * @return vector of sorted files in this directory in .txt format
 */
vector<string> Common::getFolderFileNames(string directory,string file_name)
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
              if(file_name=="")
                directory_file_name.append(dir->d_name);
              else
                directory_file_name.append(file_name);
              directory_file_name.append(".txt");

              days_folder_file_names.push_back(directory_file_name);
            }
        }

      closedir(d);
    }

  sort(days_folder_file_names.begin(), days_folder_file_names.end());

  return days_folder_file_names;
}

/**
 * @brief Common::getFolderNamesWithPaths
 * @param directory
 * @return
 */
vector<string> Common::getFolderNamesWithPaths(string directory)
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

              days_folder_file_names.push_back(directory_file_name);
            }
        }

      closedir(d);
    }

  sort(days_folder_file_names.begin(), days_folder_file_names.end());

  return days_folder_file_names;
}

/**
 * @brief Common::getFolderNames
 * @param directory
 * @return
 */
vector<string> Common::getFolderNames(string directory)
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
              days_folder_file_names.push_back(dir->d_name);
            }
        }

      closedir(d);
    }

  sort(days_folder_file_names.begin(), days_folder_file_names.end());

  return days_folder_file_names;
}
/**
 * @brief fileExists
 * @param filename
 * @return true if the file exist, otherwise false
 */
bool Common::fileExists(const string& filename)
{
  ifstream infile(filename);
  return infile.good();
}


/**
 * @brief Common::readFile
 * @param folder_path
 * @param separator
 * @return
 */
vector<vector<string> > Common::readFile(string &folder_path, char separator)
{
  ifstream input_stream( folder_path );
  vector<vector<string> > fileConntents;

  while (input_stream)
    {
      string line;
      vector<string> elem;

      if (!getline( input_stream, line )) break;

      Common::split(line,separator,elem);

      fileConntents.push_back(elem);
    }

  input_stream.close();
  return fileConntents;

}

