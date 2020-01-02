/**
 * @file finapi.h
 * 
 * @brief This is the main file for accessing the library.
 * 
 * This file is included in every source file and is processed as a pre-compiled header,
 * thus any main system includes should go here. As the library expands, each new file should
 * be added to this file and the user should only need this include in order to access 
 * the entire library.
 * 
 * @author  Max Ortner
 * @date    2019-12-23
 * @version 0.0.1
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#pragma once

/*      Sys. Includes       */
#include <fstream>  // ifstream, ofstream
#include <vector>   // vector class
#include <thread>   // thread class
#include <cstdlib>  // malloc, free
#include <string>   // string class
#include <cstring>  // memset

/*          Network         */
#include "Network/File.h"
#include "Network/Network.h"
#include "Network/CClient.h"
#include "Network/ServerStream.h"

/*          Models          */
#include "Models/Company.h"
#include "Models/DataTag.h" 
#include "Models/Statement.h" 

#define assert(expr) if (!(expr)) {\
    printf("Assertion failed in file %s on expression\n\n%s\n\non line %d\n", __FILE__, #expr, __LINE__);\
    exit(1); }