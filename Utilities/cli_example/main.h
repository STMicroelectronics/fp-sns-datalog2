/**
  ******************************************************************************
  * @file    main.c
  * @author  SRA
  * @brief   header file of command line example of High Speed Datalog
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

#include <stdint.h>
#include <iostream>
#include "HS_DataLog.h"
#include <stdio.h>
#include "pthread.h"
#include <unistd.h>
#include "json.hpp"
#include <vector>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <string>
#include <chrono>
#if _WIN32
#include <conio.h>
#endif

#ifdef __linux__
#include <sys/stat.h>
int _kbhit() ;
#elif _WIN32
#include "windows.h"
#endif

class InputParser
{
    public:
        InputParser (int &argc, char **argv)
        {
            for (int i=1; i < argc; ++i)
                this->tokens.push_back(std::string(argv[i]));
        }

        const std::string& getCmdOption(const std::string &option) const
        {
            std::vector<std::string>::const_iterator itr;
            itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
            if (itr != this->tokens.end() && ++itr != this->tokens.end())
            {
                return *itr;
            }
            static const std::string empty_string("");
            return empty_string;
        }

        bool cmdOptionExists(const std::string &option) const
        {
            return std::find(this->tokens.begin(), this->tokens.end(), option)
                   != this->tokens.end();
        }

    private:
        std::vector <std::string> tokens;
};


// Get Input
bool getInput(char *c)
{
#ifdef __linux__
    if (_kbhit())
    {
        *c = getc(stdin);
        return true; // Key Was Hit
    }
    return false; // No keys were pressed
#elif _WIN32
    if (_kbhit())
    {
        *c = getch();
        return true; // Key Was Hit
    }
    return false; // No keys were pressed
#endif
}

