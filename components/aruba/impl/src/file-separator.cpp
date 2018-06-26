#include<file-separator.h>
/*
 * file-separator.cpp
 *
 *  Created on: 2017-03-31
 *      Author: Mohamed Eldib
 * This class divides the preprocessed file (sensor events) from preprocessing-stage class
 * into day instance files (sensor events)

 */
/**
 * @brief FileSeparator::FileSeparator
 * @param file_name: preprocessed file name
 */
FileSeparator::FileSeparator(string file_name,string folder_path)
{
    COUT<<"FileSeparator"<<endl;
    logging::INFO("FileSeparator");

    this->file_name=file_name;
    this->folder_path_=folder_path;
    this->output_file = new std::ofstream(this->file_name,std::fstream::app);

}

/**
 * @brief FileSeparator::~FileSeparator
 */
FileSeparator::~FileSeparator()
{

    COUT<<"~FileSeparator"<<endl;
    logging::INFO("~FileSeparator");

    if(this->output_file)
    {
        this->output_file->close();
        delete this->output_file;
    }
}

/**
 * @brief FileSeparator::writeValueToFile
 * @param value
 * @return result code
 */
int FileSeparator::writeValueToFile(int value)
{
    COUT<<"writeValueToFile"<<endl;
    logging::INFO("writeValueToFile");

    int rc = 1;
    if(this->output_file)
    {
        *this->output_file<<value<<endl;
    }
    else
    {
        rc = 0;
    }

    return rc;
}


/**
 * @brief FileSeparator::extractDateFromLine
 * @param line: extract date from line
 * @return date
 */
string FileSeparator::extractDateFromLine(string line)
{
    COUT<<"extractDateFromLine"<<endl;
    logging::INFO("extractDateFromLine");

    vector<string> elem1,elem2;
    Common::split(line,Constants::COMMA_SEPARATOR,elem1);
    string full_date=elem1[0];

    Common::split(full_date,Constants::DATE_SEPARATOR,elem2);

    return elem2[0];
}
/**
 * @brief FileSeparator::buildDateFileName
 * @param folder_path
 * @param file_name: date name
 * @return output file name
 */
string FileSeparator::buildDateFileName(string folder_path,string file_name)
{
    COUT<<"buildDateFileName"<<endl;
    logging::INFO("buildDateFileName");

    folder_path.append("/");
    folder_path.append(file_name);
    folder_path.append(".txt");

    return folder_path;
}
/**
 * @brief FileSeparator::createFolderFromDate
 * @param folder_name: create folder for day instance
 * @return path to folder
 */
string FileSeparator::createFolderFromDate(string folder_name)
{
    COUT<<"createFolderFromDate"<<endl;
    logging::INFO("createFolderFromDate");

    string folder_path=this->folder_path_;

    folder_path.append(folder_name);

    bool isCreated = Common::createFolder(folder_path);

    if(!isCreated)
        return "";

    return folder_path;

}
/**
 * @brief FileSeparator::writeDailySensorData
 * write lines to day instance file
 */
void FileSeparator::writeDailySensorData()
{
    COUT<<"writeDailySensorData"<<endl;
    logging::INFO("writeDailySensorData");

    bool is_first_entry=true;
    string prev_date;
    string curr_date;
    ifstream in_file(this->file_name);
    std::ofstream* out_file;

    while (in_file)
    {
        string line;

        if (!getline( in_file, line )) break;

        //get first line
        if(is_first_entry)
        {
            //initalize days for comparison to check later if it is need to create new folder
            prev_date=extractDateFromLine(line);
            curr_date=extractDateFromLine(line);

            //create day instance folder
            string folder_path=createFolderFromDate(curr_date);
            out_file= new std::ofstream(buildDateFileName(folder_path,curr_date));


            is_first_entry=false;
        }
        else
            curr_date=extractDateFromLine(line);

        //if same days then write to file
        if(prev_date==curr_date)
            *out_file<<line<<endl;

        else
        {
            //update day instance
            prev_date=curr_date;
            //create day instance folder
            string folder_path=createFolderFromDate(curr_date);
            out_file= new std::ofstream(buildDateFileName(folder_path,curr_date));

            //write to file
            *out_file<<line<<endl;

        }

    }
}
