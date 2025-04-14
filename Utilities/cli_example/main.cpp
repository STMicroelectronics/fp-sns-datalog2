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

using namespace std;

// TODO: Next version --> Hotplug events notification support
// void plug_callback()
// {
//     std::cout << "HSD DEVICE PLUGGED" << std::endl;
//     return;
// }

// void unplug_callback()
// {
//     std::cout << "HSD DEVICE UNPLUGGED" << std::endl;
//     return;
// }
// TODO: Next version --> Hotplug events notification support

int main(int argc, char *argv[])
{
    /* ---------------------------- Parse command line options ---------------------------- */

    setLogLevel(LOG_LEVEL_INFO);

    InputParser input(argc, argv);

    if(input.cmdOptionExists("-h"))
    {
        cout << "Welcome to HSDatalog Command Line Interface example" << endl << endl;
        cout << "Usage: " << endl << " cli_example [-COMMAND [ARGS]]" << endl << endl;
        cout << "Commands:" << endl << endl;
        cout << " -h\t\t: Show this help" << endl;
        cout << " -f <filename>\t: Device Configuration file (JSON)" << endl;
        cout << " -u <filename>\t: UCF Configuration file for MLC" << endl;
        cout << " -msn <mlc_sensor_name>\t: MLC Sensor Name" << endl;
        cout << " -t <seconds>\t: Duration of the current acquisition (seconds) " << endl;
        cout << " -d <debug_level>\t: Set the debug level (0-DEBUG, 1-INFO, 2-WARNING, 3-ERROR). Default is 1 (INFO)" << endl;
        cout << " -g\t\t: Get current Device Configuration, save it to file <device_config.json> and return." << endl;
        cout << " -y\t\t: Directly start the acquisition without waiting for user confirmation" << endl;
        cout << "   \t\t  All other parameters are ignored! " << endl;
        cout << endl;

        exit(0);
    }

    // TODO: Next version --> Hotplug events notification support
    // hs_datalog_register_usb_hotplug_callback(plug_callback, unplug_callback);
    // TODO: Next version --> Hotplug events notification support

    // TODO: Next version --> Hotplug events notification support
    // hs_datalog_start_hotplug_monitor();
    // TODO: Next version --> Hotplug events notification support

    if(input.cmdOptionExists("-d"))
    {
        const std::string &debugLevelParam = input.getCmdOption("-d");
        if (!debugLevelParam.empty())
        {
            try
            {
                int debugLevel = std::stoi(debugLevelParam);
                switch (debugLevel)
                {
                    case 0:
                        setLogLevel(LOG_LEVEL_DEBUG);
                        break;
                    case 1:
                        setLogLevel(LOG_LEVEL_INFO);
                        break;
                    case 2:
                        setLogLevel(LOG_LEVEL_WARNING);
                        break;
                    case 3:
                        setLogLevel(LOG_LEVEL_ERROR);
                        break;
                    default:
                        logMessage(LOG_LEVEL_ERROR, "Invalid debug level: " + debugLevelParam);
                        logMessage(LOG_LEVEL_INFO, "Log level is set to default (INFO)");
                }
            }
            catch (const std::invalid_argument& ia)
            {
                logMessage(LOG_LEVEL_ERROR, "The specified debug level argument is not a valid number: " + debugLevelParam);
                logMessage(LOG_LEVEL_INFO, "Log level is set to default (INFO)");
            }
        }
        else
        {
            logMessage(LOG_LEVEL_ERROR, "Missing debug level after -d command");
            logMessage(LOG_LEVEL_INFO, "Log level is set to default (INFO)");
        }
    }

    logMessage(LOG_LEVEL_INFO, "Welcome to HSDatalog Command Line Interface example");

    if(input.cmdOptionExists("-g"))
    {
        if(hs_datalog_open() != ST_HS_DATALOG_OK)
        {
            logMessage(LOG_LEVEL_ERROR, "Error occurred while initializing datalog");
            cout << "Press any key to exit \n";
            getchar();
            return -1;
        }

        int nDevices = 0;
        if(hs_datalog_get_device_number(&nDevices) != ST_HS_DATALOG_OK)
        {
            logMessage(LOG_LEVEL_ERROR, "Error occurred while retrieving device number");
            cout << "Press any key to exit \n";
            getchar();
            return -1;

        }

        if(nDevices == 0)
        {
            logMessage(LOG_LEVEL_ERROR, "No devices, exiting");
            cout << "Press any key to exit \n";
            getchar();
            return -1;
        }

        char *tmp1;
        if(hs_datalog_get_device_status(0, &tmp1) != ST_HS_DATALOG_OK)
        {
            logMessage(LOG_LEVEL_ERROR, "Error occurred while retrieving device configuration");
            cout << "Press any key to exit \n";
            getchar();
            return -1;
        }

        auto json = nlohmann::json::parse(tmp1);

        ofstream currentConfigFile;
        currentConfigFile.open("device_config.json", ios::out|ios::binary);
        currentConfigFile << json.dump(1);
        currentConfigFile.close();

        logMessage(LOG_LEVEL_INFO, "Current Device Status has been saved as device_config.json.");
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
                logMessage(LOG_LEVEL_ERROR, "Device configuration file not found");
                exit(1);
            }
        }
        else
        {
            logMessage(LOG_LEVEL_ERROR, "Please specify a file name after -f command");
            exit(1);
        }
    }

    const std::string &fileUCFParam = input.getCmdOption("-u");
    ifstream ucfFile;
    char* mlc_comp_str = nullptr;
    if(input.cmdOptionExists("-u"))
    {
        if(input.cmdOptionExists("-msn"))
        {
            mlc_comp_str = new char[input.getCmdOption("-msn").length() + 1];
            strcpy(mlc_comp_str, input.getCmdOption("-msn").c_str());
        }
        else
        {
            logMessage(LOG_LEVEL_ERROR, "-msn <mlc_sensor_name> is required when -u <filename> is specified.");
            exit(1);
        }
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
                logMessage(LOG_LEVEL_ERROR, "UCF configuration file not found");
                exit(1);
            }
        }
        else
        {
            logMessage(LOG_LEVEL_ERROR, "Please specify a file name after -u command");
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
                logMessage(LOG_LEVEL_ERROR, "The specified timeout argument is not a valid number: " + timeoutSecondsParam);
                exit(1);
            }
        }
        else
        {
            logMessage(LOG_LEVEL_ERROR, "Please specify a timeout after -t command");
            exit(1);
        }
    }

    /* ---------------------------- Retrieve device information  ---------------------------- */
    char * version;
    hs_datalog_get_version(&version);

    logMessage(LOG_LEVEL_INFO, "STWIN Command Line Interface example");
    logMessage(LOG_LEVEL_INFO, "Version: 2.0.0");
    logMessage(LOG_LEVEL_INFO, "Based on : " + string(version));

    if(hs_datalog_open() != ST_HS_DATALOG_OK)
    {
        logMessage(LOG_LEVEL_ERROR, "Error occurred while initializing datalog");
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }



    int nDevices = 0;
    if(hs_datalog_get_device_number(&nDevices) != ST_HS_DATALOG_OK)
    {
        logMessage(LOG_LEVEL_ERROR, "Error occurred while retrieving device number");
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }

    if(nDevices == 0)
    {
        logMessage(LOG_LEVEL_ERROR, "No devices, exiting");
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

    char* fwInfo_str = (char*)"firmware_info";
    if(hs_datalog_get_component_status(deviceID, &fwInfo, fwInfo_str) != ST_HS_DATALOG_OK)
    {
        logMessage(LOG_LEVEL_ERROR, "Error occurred while retrieving FW Information Component");
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }

    auto json = nlohmann::json::parse(fwInfo);
    
    logMessage(LOG_LEVEL_INFO, "Firmware Info:");
    deviceAlias = json["firmware_info"]["alias"];
    logMessage(LOG_LEVEL_INFO, json.dump(1));

    /* Free memory */
    if(hs_datalog_free(fwInfo) != ST_HS_DATALOG_OK)
    {
        logMessage(LOG_LEVEL_ERROR, "Error occurred while freeing memory");
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }
    char* deviceInfo_str = (char*)"DeviceInformation";
    if(hs_datalog_get_component_status(deviceID, &deviceInfo, deviceInfo_str) != ST_HS_DATALOG_OK)
    {
        logMessage(LOG_LEVEL_ERROR, "Error occurred while retrieving Device Information Component");
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }

    json = nlohmann::json::parse(deviceInfo);
    logMessage(LOG_LEVEL_INFO, "Device Information: \n" + json.dump(1));

    /* Free memory */
    if(hs_datalog_free(deviceInfo) != ST_HS_DATALOG_OK)
    {
        logMessage(LOG_LEVEL_ERROR, "Error occurred while freeing memory");
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }

    if(hs_datalog_get_sensor_components_number(deviceID, &nSensors, true) != ST_HS_DATALOG_OK)
    {
        logMessage(LOG_LEVEL_ERROR, "Error occurred while retrieving Sensor Components number");
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }

    logMessage(LOG_LEVEL_INFO, "----------------------> N Active Sensor Components: " + to_string(nSensors) + "\n");

    /* -------------------- Load device configuration from JSON file (if requested) or use default configuration  -------------------- */

    /* Try and open JSON config file on the hard disk*/
    bool configFromFile = false;

    /* JSON config file is present */
    if (configFile.is_open())
    {
        configFromFile = true;
        logMessage(LOG_LEVEL_INFO, "\nDevice Status imported from Json file\n");

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
            logMessage(LOG_LEVEL_ERROR, "Error occurred while freeing memory");
            cout << "Press any key to exit \n";
            getchar();
            return -1;
        }

        if(hs_datalog_get_sensor_components_number(deviceID, &nSensors, true) != ST_HS_DATALOG_OK)
        {
            logMessage(LOG_LEVEL_ERROR, "Error occurred while retrieving Sensor Components number");
            cout << "Press any key to exit \n";
            getchar();
            return -1;
        }

        logMessage(LOG_LEVEL_INFO, "----------------------> N Active Sensor Components: " + to_string(nSensors) + "\n");
    }
    else
    {   /* if JSON device_donfig file is not specified, simply get all the sensors status with their default configuration */
        logMessage(LOG_LEVEL_INFO, "Using default configuration stored in the device\n");
    }

    /* send UCF file to MLC if present */
    if (ucfFile.is_open())
    {
        char* pnpl_response = nullptr;
        hs_datalog_load_ucf_file_to_mlc(0, mlc_comp_str, const_cast<char*>(fileUCFParam.c_str()), &pnpl_response);

        if(pnpl_response != nullptr){
            if(hs_datalog_free(pnpl_response) != ST_HS_DATALOG_OK)
            {
            logMessage(LOG_LEVEL_ERROR, "Error occurred while freeing memory");
            cout << "Press any key to exit \n";
            getchar();
            return -1;
            }
        }
    }

    bool taggingEnabled=true;

    if(!input.cmdOptionExists("-y"))
    {
        cout << "Press Enter key to start logging: " << endl;
        getchar();
    }

    std::map<std::tuple<int,string,string>, bool> tags;
    if(taggingEnabled)
    {
        char *tagsInfo;
//        hs_datalog_get_tags_info(deviceID, &tagsInfo);
        char* tagsInfo_str = (char*)"tags_info";
        if(hs_datalog_get_component_status(deviceID, &tagsInfo, tagsInfo_str) != ST_HS_DATALOG_OK)
        {
            logMessage(LOG_LEVEL_ERROR, "Error occurred while retrieving tags information");
            cout << "Press any key to exit \n";
            getchar();
            return -1;
        }

        logMessage(LOG_LEVEL_DEBUG, "Tags Information: \n" + string(tagsInfo));
        auto tags_json = nlohmann::json::parse(tagsInfo);
        int counter = 0;                                                  
        for (auto& tag_el : tags_json["tags_info"].items()) {
            logMessage(LOG_LEVEL_DEBUG, tag_el.key() + ": " + tag_el.value().dump());
            if(tag_el.key().find("sw_tag") != std::string::npos){ //only SW_Tag could be used for SW Tagging
                auto ta = std::make_tuple (counter, tag_el.key(), tag_el.value().at("label"));
                counter++;
                tags.insert(std::pair<std::tuple<int,string,string>,bool>(ta,false));
            }
        }

        if(hs_datalog_free(tagsInfo) != ST_HS_DATALOG_OK)
        {
            logMessage(LOG_LEVEL_ERROR, "Error occurred while freeing memory");
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
#elif __APPLE__
    mkdir(time_buff, 0755);
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

        char* mlc_str = new char[strlen(mlc_comp_str) + 5]; // 4 for "_mlc" and 1 for null terminator
        strcpy(mlc_str, mlc_comp_str);
        strcat(mlc_str, "_mlc");
        char* enable_str = (char*)"enable";
        char * pnpl_response = nullptr;
        hs_datalog_set_boolean_property(deviceID, true, mlc_str, enable_str, nullptr, &pnpl_response);

        char * deviceStatus;
        hs_datalog_get_device_status(deviceID, &deviceStatus);
        hs_datalog_update_components_map(deviceID, deviceStatus);

        /* Free memory */
        if(hs_datalog_free(deviceStatus) != ST_HS_DATALOG_OK)
        {
            logMessage(LOG_LEVEL_ERROR, "Error occurred while freeing memory");
            cout << "Press any key to exit \n";
            getchar();
            return -1;
        }
        if(pnpl_response != nullptr){
            /* Free memory */
            if(hs_datalog_free(pnpl_response) != ST_HS_DATALOG_OK)
            {
                logMessage(LOG_LEVEL_ERROR, "Error occurred while freeing memory");
                cout << "Press any key to exit \n";
                getchar();
                return -1;
            }
        }
    }

    //update nSensors (check if any sensors have been disabled)
    hs_datalog_get_sensor_components_number(deviceID, &nSensors, true);
    //create sensor names array
    char* sNames[nSensors];
    hs_datalog_get_sensor_components_names(deviceID, sNames, true);

    for (auto c : sNames)
    {
        bool tmp_enable;
        char* enable_str = (char*)"enable";
        hs_datalog_get_boolean_property(deviceID, &tmp_enable, c, enable_str);

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
    char * pnpl_response = nullptr;
    hs_datalog_set_rtc_time(deviceID, &pnpl_response);
    if(pnpl_response != nullptr){
        /* Free memory */
        if(hs_datalog_free(pnpl_response) != ST_HS_DATALOG_OK)
        {
            logMessage(LOG_LEVEL_ERROR, "Error occurred while freeing memory");
            cout << "Press any key to exit \n";
            getchar();
            return -1;
        }
    }

    /* Start logging */
    pnpl_response = nullptr;
    hs_datalog_start_log(deviceID, 1, &pnpl_response); //1:USB
    if(pnpl_response != nullptr){
        /* Free memory */
        if(hs_datalog_free(pnpl_response) != ST_HS_DATALOG_OK)
        {
            logMessage(LOG_LEVEL_ERROR, "Error occurred while freeing memory");
            cout << "Press any key to exit \n";
            getchar();
            return -1;
        }
    }

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
#elif __APPLE__
        sleep(1);
        system("clear");
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

                char* tagsInfo_str = (char*)"tags_info";
                char* status_str = (char*)"status";
                char * pnpl_response = nullptr;
                if(!result->second)
                {
                    hs_datalog_set_boolean_property(deviceID, true, tagsInfo_str, tag_prop_name_cstr, status_str, &pnpl_response);
                    result->second=true;
                }
                else
                {
                    hs_datalog_set_boolean_property(deviceID, false, tagsInfo_str, tag_prop_name_cstr, status_str, &pnpl_response);
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
    pnpl_response = nullptr;
    hs_datalog_stop_log(deviceID, &pnpl_response);
    if(pnpl_response != nullptr){
        /* Free memory */
        if(hs_datalog_free(pnpl_response) != ST_HS_DATALOG_OK)
        {
            logMessage(LOG_LEVEL_ERROR, "Error occurred while freeing memory");
            cout << "Press any key to exit \n";
            getchar();
            return -1;
        }
    }

    /* close files */
#ifdef __APPLE__
    _closeFiles(sNames, nSensors, pFiles);
#else
    for (auto c: sNames)
    {
        fclose(pFiles.at(c));
    }
#endif

    char * deviceStatus;
    if(hs_datalog_get_device_status(deviceID, &deviceStatus) != ST_HS_DATALOG_OK)
    {
        logMessage(LOG_LEVEL_ERROR, "Error occurred while retrieving device status");
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

    // Calculate the required buffer size
    size_t bufferSize = strlen(time_buff) + strlen("/device_config.json") + 1; // +1 for the null terminator
    // Allocate memory for the buffer
    char *deviceConfigFileName = new char[bufferSize];
    // Safely format the string into the buffer
    snprintf(deviceConfigFileName, bufferSize, "%s/device_config.json", time_buff);

    FILE * jsonFile = fopen(deviceConfigFileName, "wt");
    fwrite(jsonS.c_str(), sizeof(char), jsonS.size(), jsonFile);
    fclose(jsonFile);

    /* Free memory */
    if(hs_datalog_free(deviceStatus) != ST_HS_DATALOG_OK)
    {
        logMessage(LOG_LEVEL_ERROR, "Error occurred while freeing memory");
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }


    char * acquisitionInfo;
    char* acquisitionInfo_str = (char*)"acquisition_info";
    if(hs_datalog_get_component_status(deviceID, &acquisitionInfo, acquisitionInfo_str) != ST_HS_DATALOG_OK)
    {
        logMessage(LOG_LEVEL_ERROR, "Error occurred while retrieving Acquisition Info");
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }
    json_final = nlohmann::json::parse(acquisitionInfo);
    jsonS = json_final["acquisition_info"].dump(1,'\t'); // pretty format

    // Calculate the required buffer size
    bufferSize = strlen(time_buff) + strlen("/acquisition_info.json") + 1; // +1 for the null terminator
    // Allocate memory for the buffer
    char *acquisitionInfoFileName = new char[bufferSize];
    // Safely format the string into the buffer
    snprintf(acquisitionInfoFileName, bufferSize, "%s/acquisition_info.json", time_buff);
    
    jsonFile = fopen(acquisitionInfoFileName, "wt");
    fwrite(jsonS.c_str(), sizeof(char), jsonS.size(), jsonFile);
    fclose(jsonFile);

    /* Free memory */
    if(hs_datalog_free(acquisitionInfo) != ST_HS_DATALOG_OK)
    {
        logMessage(LOG_LEVEL_ERROR, "Error occurred while freeing memory");
        cout << "Press any key to exit \n";
        getchar();
        return -1;
    }
    hs_datalog_close();

    // TODO: Next version --> Hotplug events notification support
    // hs_datalog_stop_hotplug_monitor();
    // TODO: Next version --> Hotplug events notification support

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

#elif __APPLE__

#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>

int _kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

void _closeFiles(char* sNames[], int nSensors, std::map<std::string, FILE*>& pFiles) {
    for (int i = 0; i < nSensors; ++i) {
        std::string name(sNames[i]);
        if (pFiles.find(name) != pFiles.end() && pFiles[name] != nullptr) {
            logMessage(LOG_LEVEL_DEBUG, "Closing file: " + name);
            fclose(pFiles[name]);
            pFiles[name] = nullptr; // Set the pointer to nullptr after closing
        } else {
            logMessage(LOG_LEVEL_ERROR, "Attempted to close an invalid or already closed file: " + name);
        }
    }
}
#endif

