/**
  ******************************************************************************
  * @file    main.c
  * @author  SRA
  * @brief   source code of command line example of High Speed Datalog
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */
#include "main.h"

#include <unordered_map>
#include <iomanip>
#include <sstream>
#include "json.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    /* ---------------------------- Parse command line options ---------------------------- */

    InputParser input(argc, argv);

    if(input.cmdOptionExists("-h"))
    {
        cout << "Welcome to HSDatalog Command Line Interface example" << endl << endl;
        cout << "Usage: " << endl << " cli_example [-COMMAND [ARGS]]" << endl << endl;
        cout << "Commands:" << endl << endl;
        cout << " -h\t\t: Show this help" << endl;
        cout << " -f <filename>\t: Device Configuration file (JSON)" << endl;
        cout << " -u <filename>\t: UCF Configuration file for MLC" << endl;
        cout << " -t <seconds>\t: Duration of the current acquisition (seconds) " << endl;
        cout << " -g\t\t: Get current Device Configuration, save it to file <device_config.json> and return." << endl;
        cout << " -y\t\t: Directly start the acquisition without waiting for user confirmation" << endl;
        cout << "   \t\t  All other parameters are ignored! " << endl;
        cout << endl;

        exit(0);
    }

    if(input.cmdOptionExists("-g"))
    {
        if(hs_datalog_open() != ST_HS_DATALOG_OK)
        {
            cout << "Error occurred while initializing datalog\n";
            cout << "Press any key to exit \n";
            getchar();
            return -1;
        }

        int nDevices = 0;
        if(hs_datalog_get_device_number(&nDevices) != ST_HS_DATALOG_OK)
        {
            cout << "Error occurred while retrieving device number\n";
            cout << "Press any key to exit \n";
            getchar();
            return -1;

        }

        if(nDevices == 0)
        {
            cout << "No devices, exiting\n";
            cout << "Press any key to exit \n";
            getchar();
            return -1;
        }

        char *tmp1;
        if(hs_datalog_get_device_status(0, &tmp1) != ST_HS_DATALOG_OK)
        {
            cout << "Error occurred while retrieving device configuration\n";
            cout << "Press any key to exit \n";
            getchar();
            return -1;
        }

        auto json = nlohmann::json::parse(tmp1);

        ofstream currentConfigFile;
        currentConfigFile.open("device_config.json", ios::out|ios::binary);
        currentConfigFile << json.dump(1);
        currentConfigFile.close();

        cout << "Current Device Status has been saved as device_config.json.\n";
        hs_datalog_free(tmp1);
        exit(0);
    }


    const std::string &fileNameParam = input.getCmdOption("-f");
    ifstream configFile;

    if(input.cmdOptionExists("-f"))
    {
        if (!fileNameParam.empty())
        {
            string configFileName = fileNameParam;

            configFile.open(configFileName, ios::in|ios::binary);

            if (!configFile.is_open()) // Needed in linux in case of local file???
            {
                configFileName ="./" + fileNameParam;
                configFile.open(configFileName, ios::in|ios::binary);
            }
            if (!configFile.is_open())
            {
                cout << "Device configuration file not found" << endl << endl;
                exit(1);
            }
        }
        else
        {
            cout << "Please specify a file name after -f command" << endl << endl;
            exit(1);
        }
    }

    const std::string &fileUCFParam = input.getCmdOption("-u");
    ifstream ucfFile;
    if(input.cmdOptionExists("-u"))
    {
        if (!fileUCFParam.empty())
        {
            string ucfFileName = fileUCFParam;

            ucfFile.open(ucfFileName, ios::in|ios::binary);

            if (!ucfFile.is_open()) // Needed in linux in case of local file???
            {
                ucfFileName ="./" + fileUCFParam;
                ucfFile.open(ucfFileName, ios::in|ios::binary);
            }
            if (!ucfFile.is_open())
            {
                cout << "UCF configuration file not found" << endl << endl;
                exit(1);
            }
        }
        else
        {
            cout << "Please specify a file name after -u command" << endl << endl;
            exit(1);
        }
    }

    const std::string &timeoutSecondsParam = input.getCmdOption("-t");
    unsigned long timeoutSeconds = 0;

    if(input.cmdOptionExists("-t"))
    {
        if (!timeoutSecondsParam.empty())
        {
            try
            {
                timeoutSeconds = std::stoul(timeoutSecondsParam);
            }
            catch (const std::invalid_argument& ia)
            {
                cout << "The specified timeout argument is not a valid number: " << timeoutSecondsParam << endl;
                exit(1);
            }
        }
        else
        {
            cout << "Please specify a timeout after -t command" << endl << endl;
            exit(1);
        }
    }

    /* ---------------------------- Retrieve device information  ---------------------------- */
    char * version;
    hs_datalog_get_version(&version);

    cout << "STWIN Command Line Interface example\n";
    cout << "Version: 2.0.0\n";
    cout << "Based on : ";
    cout << version;
    cout << "\n";

    if(hs_datalog_open() != ST_HS_DATALOG_OK)
    {
        cout << "Error occurred while initializing datalog\n";
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }

    int nDevices = 0;
    if(hs_datalog_get_device_number(&nDevices) != ST_HS_DATALOG_OK)
    {
        cout << "Error occurred while retrieving device number\n";
        cout << "Press any key to exit \n";
        getchar();
        return -1;

    }

    if(nDevices == 0)
    {
        cout << "No devices, exiting\n";
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }

    /* If multiple devices are present, address only device with id = 0 */
    int deviceID = 0;
    int nSensors = 0;
    int nActiveSensors = 0;
    char * fwInfo;
    char * deviceInfo;
    string deviceAlias;

    if(hs_datalog_get_component_status(deviceID, &fwInfo, "firmware_info") != ST_HS_DATALOG_OK)
    {
        cout << "Error occurred while retrieving FW Information Component\n";
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }

    auto json = nlohmann::json::parse(fwInfo);
    cout << "Firmware Information: \n";
    deviceAlias = json["firmware_info"]["alias"];

    cout << json.dump(1) << "\n";

    /* Free memory */
    if(hs_datalog_free(fwInfo) != ST_HS_DATALOG_OK)
    {
        cout << "Error occurred while freeing memory\n";
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }

    if(hs_datalog_get_component_status(deviceID, &deviceInfo, "DeviceInformation") != ST_HS_DATALOG_OK)
    {
        cout << "Error occurred while retrieving FW Information Component\n";
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }

    json = nlohmann::json::parse(deviceInfo);
    cout << "Device Information: \n";
    cout << json.dump(1) << "\n";

    /* Free memory */
    if(hs_datalog_free(deviceInfo) != ST_HS_DATALOG_OK)
    {
        cout << "Error occurred while freeing memory\n";
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }

    if(hs_datalog_get_sensor_components_number(deviceID, &nSensors, true) != ST_HS_DATALOG_OK)
    {
        cout << "Error occurred while retrieving Sensor Components number\n";
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }

    std::cout << "----------------------> N Active Sensor Components: " << nSensors <<endl <<endl;

//TODO
//    char acq_name[] = "testName";
//    char acq_description[] = "descriptionTest";
//    hs_datalog_set_acquisition_param(deviceID, acq_name, acq_description);

    /* -------------------- Load device configuration from JSON file (if requested) or use default configuration  -------------------- */

    /* Try and open JSON config file on the hard disk*/
    bool configFromFile = false;

    /* JSON config file is present */
    if (configFile.is_open())
    {
        configFromFile = true;
        cout << endl <<"Device Status imported from Json file " << endl << endl;

        configFile.seekg (0, configFile.end);
        long long size = configFile.tellg();
        configFile.seekg (0, configFile.beg);

        /* Read the whole file */
        char * jsonChar = new char [unsigned(size+1)];
        configFile.seekg (0, ios::beg);
        configFile.read (jsonChar, static_cast<int>(size));
        configFile.close();
        jsonChar[size]=0;  // JSON lib needs a '\0' at the end of the string

        hs_datalog_set_device_status(0,jsonChar);

        if(hs_datalog_free(jsonChar) != ST_HS_DATALOG_OK)
        {
         cout << "Error occurred while freeing memory\n";
         cout << "Press any key to exit \n";
         getchar();
         return -1;
        }

        if(hs_datalog_get_sensor_components_number(deviceID, &nSensors, true) != ST_HS_DATALOG_OK)
        {
            cout << "Error occurred while retrieving Sensor Components number\n";
            cout << "Press any key to exit \n";
            getchar();
            return -1;
        }

        std::cout << "----------------------> N Active Sensor Components: " << nSensors <<endl <<endl;
    }
    else
    {   /* if JSON device_donfig file is not specified, simply get all the sensors status with their default configuration */
        cout <<"Using default configuration stored in the device" << endl << endl;
    }

    /* send UCF file to MLC if present */
    if (ucfFile.is_open())
    {
        ucfFile.seekg (0, ucfFile.end);
        long long size_ucf = ucfFile.tellg();
        ucfFile.seekg (0, ucfFile.beg);

        /* Read the whole file */
        char * ucfData = new char [unsigned(size_ucf+1)];
        ucfFile.seekg (0, ios::beg);
        ucfFile.read (ucfData, static_cast<int>(size_ucf));

        //TODO REDESIGN THIS!
        hs_datalog_load_ucf_to_mlc(0, "ism330dhcx", (uint8_t *)ucfData, size_ucf);

        if(hs_datalog_free(ucfData) != ST_HS_DATALOG_OK)
        {
         cout << "Error occurred while freeing memory\n";
         cout << "Press any key to exit \n";
         getchar();
         return -1;
        }
    }

    bool taggingEnabled=true;

    if(!input.cmdOptionExists("-y"))
    {
        cout << "Press Enter key to start logging\n";
        getchar();
    }

    std::map<std::tuple<int,string,string>, bool> tags;
    if(taggingEnabled)
    {
        char *tagsInfo;
//        hs_datalog_get_tags_info(deviceID, &tagsInfo);
        hs_datalog_get_component_status(deviceID, &tagsInfo, "tags_info");

        cout << tagsInfo << endl;
        auto tags_json = nlohmann::json::parse(tagsInfo);
        int counter = 0;                                                  
        for (auto& tag_el : tags_json["tags_info"].items()) {
            cout << tag_el << endl;
            if(tag_el.key().find("sw_tag") != std::string::npos){ //only SW_Tag could be used for SW Tagging
                auto ta = std::make_tuple (counter, tag_el.key(), tag_el.value().at("label"));
                counter++;
                tags.insert(std::pair<std::tuple<int,string,string>,bool>(ta,false));
            }
        }

        if(hs_datalog_free(tagsInfo) != ST_HS_DATALOG_OK)
        {
         cout << "Error occurred while freeing memory\n";
         cout << "Press any key to exit \n";
         getchar();
         return -1;
        }
    }

    /* -------------------- Open raw data files and allocate buffers  -------------------- */
    std::map<string, FILE *> pFiles;
    std::map<string, int> packetsReceived;

    /*Get current time as char buffer to create the directory */
    time_t rawtime;
    struct tm * timeinfo;
    char time_buff[256] = "./";
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(&time_buff[2],sizeof(time_buff) - 2,"%Y%m%d_%H_%M_%S",timeinfo);
#ifdef __linux__
    mkdir(time_buff, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#elif _WIN32
    mkdir(time_buff);
#endif

    /* save UCF file in Acquisition folder and enable MLC */
    if (ucfFile.is_open())
    {
        int index = fileUCFParam.find_last_of("/\\");
        std::string input_trace_filename = fileUCFParam.substr(index+1);

        std::string name=  string(time_buff) + "/";
        name.append(input_trace_filename);

        std::ofstream  dst(name,   std::ios::binary);
        ucfFile.seekg (0, ucfFile.beg);
        dst << ucfFile.rdbuf();
        ucfFile.close();

        //TODO like @row 344
//        hs_datalog_set_Enable_pnpl(deviceID, true, "ism330dhcx","mlc");
//        hs_datalog_set_Enable_pnpl(deviceID, true, "ism330dhcx_mlc");
        hs_datalog_set_boolean_property(deviceID, true, "ism330dhcx_mlc", "enable");
    }


    //update nSensors (check if any sensors have been disabled)
    hs_datalog_get_sensor_components_number(deviceID, &nSensors, true);
    //create sensor names array
    char* sNames[nSensors];
    hs_datalog_get_sensor_components_names(deviceID, sNames, true);

    for (auto c : sNames)
    {
        bool tmp_enable;
        hs_datalog_get_boolean_property(deviceID, &tmp_enable, c, "enable");

        FILE * tmp_f = nullptr;
        if(tmp_enable){
            std::string name=  string(time_buff) + "/";
            name.append(c);
            name.append(".dat");
            tmp_f = fopen(name.data(), "wb+");
        }
        pFiles.insert({c,tmp_f});
    }

    /* -------------------- Start logging and wait for user command or timeout  -------------------- */
    /* Set RTC time */
    hs_datalog_set_rtc_time(deviceID);

    /* Start logging */
    hs_datalog_start_log(deviceID, 1); //1:USB

    auto start_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>( start_time - start_time ).count();

    //packetsReceived Map initialization (all component received data = 0)
    for(auto c : sNames)
    {
        packetsReceived.insert({c,0});
    }

    uint8_t MLC_val[9] = {0};
    double  MLC_time=0;
    bool exit = false;

    while(!exit)
    {
#ifdef __linux__
        sleep(1);
#elif _WIN32
        Sleep(1000);
        system("CLS");
#endif
        cout << "+--------------HSDatalog CLI----------------+"  << endl;
        cout << "| Streaming from: ";
        cout << right << setw(25) << deviceAlias << " |" << endl;

        auto current_time = std::chrono::high_resolution_clock::now();
        elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>( current_time - start_time ).count();
        cout << right << "| Elapsed: " << setw(5) << round(elapsed_time/1000.f) << "s";

        if(timeoutSeconds!=0)
        {
            auto remainingTime = timeoutSeconds-(elapsed_time/1000.f);
            stringstream tmp;
            tmp << "Remaining: " << right << setw(5) << round(((remainingTime>0)?remainingTime:0)) << "s |";
            cout << right << setw(28) << tmp.str();
            if(timeoutSeconds<elapsed_time/1000.f)
            {
                exit = true;
            }
        }
        else
        {
            cout << right << setw(28) << "|";

        }
        cout << endl;

        cout << "+--------------Received Data----------------+"  << endl;
        for (auto c : sNames)
        {
            int size = 0, actual = 0, MLC_counter;

            hs_datalog_get_available_data_size(deviceID, c, &size);
            cout.setf(ios :: left, ios :: adjustfield);
            cout << "| " << setw(17) << c;
            cout.setf(ios :: right, ios :: adjustfield);
            cout << setw(18) << packetsReceived.at(c) << " Bytes |\n";

            if(size > 0)
            {
                uint8_t * data = new uint8_t[size];
                hs_datalog_get_data(deviceID, c, data, size, &actual);

                if(std::strstr(c,"_mlc") != NULL){

                    for (MLC_counter = 0; MLC_counter < 9; MLC_counter++)
                    {
                        MLC_val[MLC_counter]= data[MLC_counter + size - 17];
                    }
                    MLC_time = *reinterpret_cast<double *>(&data[size-8]);
                }

                fwrite (data , sizeof(char), static_cast<size_t>(size), pFiles.at(c));
                delete[] data;
                packetsReceived.at(c) += size;
            }
        }
        cout << "+-------------------------------------------+"  << endl;

        if(MLC_time != 0.0)
        {
            int MLC_counter;
            cout << "| MLC 1 Status: " << left << setw(5) << static_cast<int>(MLC_val[0]);
            stringstream tmp;
            tmp << " Timestamp: " << round(MLC_time) << "s |" << endl;
            cout << right << setw(25) << tmp.str();

            for (MLC_counter = 1; MLC_counter < 9; MLC_counter++)
            {
                cout << "| MLC " << static_cast<int>(MLC_counter+1) << " Status: " << left << setw(5) << static_cast<int>(MLC_val[MLC_counter]) << right << setw(24) << " |"<< endl;
            }
        }

        char key;
        int num_tag=-1;
        if(getInput(&key))
        {
            if(key == 0x1B || key == 'q')  /* Press Esc or q to exit */
            {
                exit = true;
            }
            else if (key >= '0' && key <= '9')
            {
                num_tag = key-'0';
            }
        }

        if(taggingEnabled)
        {
            auto result = std::find_if(
                      tags.begin(),
                      tags.end(),
                      [num_tag](const std::pair<std::tuple<int,string,string>, bool>& mo) {
                            return std::get<0>(mo.first) == num_tag;
                      });

            if(result != tags.end())
            {
                std::string tag_prop_name = std::get<1>(result->first);
                std::string tag_label = std::get<2>(result->first);
                char *tag_label_cstr = new char[tag_label.length() + 1];
                strcpy(tag_label_cstr, tag_label.c_str());
                char *tag_prop_name_cstr = new char[tag_prop_name.length() + 1];
                strcpy(tag_prop_name_cstr, tag_prop_name.c_str());

                if(!result->second)
                {    
                    hs_datalog_set_boolean_property(deviceID, true, "tags_info", tag_prop_name_cstr, "status");
                    result->second=true;
                }
                else
                {
                    hs_datalog_set_boolean_property(deviceID, false, "tags_info", tag_prop_name_cstr, "status");
                    result->second=false;
                }

                delete [] tag_label_cstr;
            }

            auto it = tags.begin();
            auto end = tags.end();

            cout << "+----------------Tag labels-----------------+" << endl;
            while(it != end)
            {
                char a = it->second==0?' ':static_cast<char>(254);
                cout <<  "| -" << std::get<0>(it->first) << "- (";
                cout << a;
                cout << ") " << left << setw(34) << std::get<2>(it->first) << "|" << endl;
                it++;
            }
            cout << "+-------------------------------------------+" << endl;
            cout << "Press the corresponding number to activate/deactivate a tag. ";
        }
        cout << "ESC to exit!" << std::endl;
    }


    /* -------------------- Stop logging, close files and connection -------------------- */

    /* Stop logging */
    hs_datalog_stop_log(deviceID);

    /* close files */
    for (auto c: sNames)
    {
        fclose(pFiles.at(c));
    }

    char * deviceStatus;
    if(hs_datalog_get_device_status(deviceID, &deviceStatus) != ST_HS_DATALOG_OK)
    {
        cout << "Error occurred while retrieving device status \n";
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }

    /* Remove acquisition_info field from device_config.json */
    unsigned int n_elements = 0, remove_elements = 0;
    auto json_final = nlohmann::json::parse(deviceStatus);
    for (auto &it : json_final["devices"][0]["components"].items())
    {
        auto component = json_final["devices"][0]["components"].at(n_elements);
        if (component.contains("acquisition_info"))
        {
            remove_elements = n_elements;
        }
        n_elements++;
    }
    json_final["devices"][0]["components"].erase(remove_elements);

    string jsonS = json_final.dump(1,'\t'); // pretty format

    char * jsonFileName = new char[strlen(time_buff) + strlen("device_config.json") + 1];
    snprintf(jsonFileName, strlen(time_buff) + strlen("device_config.json") + 2, "%s/device_config.json", time_buff);

    FILE * jsonFile = fopen(jsonFileName, "wt");
    fwrite(jsonS.c_str(), sizeof(char), jsonS.size(), jsonFile);
    fclose(jsonFile);

    /* Free memory */
    if(hs_datalog_free(deviceStatus) != ST_HS_DATALOG_OK)
    {
        cout << "Error occurred while freeing memory\n";
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }


    char * acquisitionInfo;
    if(hs_datalog_get_component_status(deviceID, &acquisitionInfo, "acquisition_info") != ST_HS_DATALOG_OK)
    {
        cout << "Error occurred while retrieving device status \n";
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }
    json_final = nlohmann::json::parse(acquisitionInfo);
    jsonS = json_final["acquisition_info"].dump(1,'\t'); // pretty format

    jsonFileName = new char[strlen(time_buff) + strlen("acquisition_info.json") + 1];
    snprintf(jsonFileName, strlen(time_buff) + strlen("acquisition_info.json") + 2, "%s/acquisition_info.json", time_buff);

    jsonFile = fopen(jsonFileName, "wt");
    fwrite(jsonS.c_str(), sizeof(char), jsonS.size(), jsonFile);
    fclose(jsonFile);

    /* Free memory */
    if(hs_datalog_free(acquisitionInfo) != ST_HS_DATALOG_OK)
    {
        cout << "Error occurred while freeing memory\n";
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }

    //TODO CHECK Acquisition Info!!!
//    char * acquisitionInfo;
//    if(hs_datalog_get_acquisition_info(deviceID, &acquisitionInfo) != ST_HS_DATALOG_OK)
//    {
//        cout << "Error occurred while retrieving Acquisition Information \n";
//        cout << "Press any key to exit \n";
//        getchar();
//        return -1;
//    }

//    cout << acquisitionInfo << endl;

//    auto json_acq = nlohmann::json::parse(acquisitionInfo);
//    string jsonS_acq = json_acq.dump(1,'\t'); // pretty format

//    char * jsonFileName_acq = new char[strlen(time_buff) + strlen("AcquisitionInfo.json") + 1];
//    snprintf(jsonFileName_acq, strlen(time_buff) + strlen("AcquisitionInfo.json") + 2, "%s/AcquisitionInfo.json", time_buff);

//    FILE * jsonFile_acq = fopen(jsonFileName_acq, "wt");
//    fwrite(jsonS_acq.c_str(), sizeof(char), jsonS_acq.size(), jsonFile_acq);
//    fclose(jsonFile_acq);

//    /* Free memory */
//    if(hs_datalog_free(acquisitionInfo) != ST_HS_DATALOG_OK)
//    {
//        cout << "Error occurred while freeing memory\n";
//        cout << "Press any key to exit \n";
//        getchar();
//        return -1;
//    }

    hs_datalog_close();

    return 0;
}

#ifdef __linux__
/**
 Linux (POSIX) implementation of _kbhit().
 Morgan McGuire, morgan@cs.brown.edu
 */
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>

int _kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;

    if (! initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}
#endif
