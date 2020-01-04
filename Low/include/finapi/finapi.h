/**
 * @file finapi.h
 * 
 * @brief This is the main file for accessing the library.
 * 
 * This file is included in every source file and is processed as a pre-compiled header(?),
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

#define assert(expr) if (!(expr)) {\
    printf("Assertion failed in file %s on expression\n\n%s\n\non line %d\n", __FILE__, #expr, __LINE__);\
    exit(1); }

#pragma region Namespace_Documentation

/**
 * @brief Base namespace that contains all data structures and functionality of the library
 */
namespace finapi {}

/**
 * @brief Contains all the base data structures as well as basic methods for retreiving them.
 */
namespace finapi::models {}

/**
 * @brief Common utility methods for extracting binary data from a given file stream.
 */
namespace finapi::models::filemethods {}

/**
 * @brief Contains the base methods for using and creating network connections
 */
namespace finapi::network {}

/**
 * @brief Contains all the methods and data structures for creating and manipulating a connection with the file server.
 */
namespace finapi::Cloud {}

#pragma endregion

#pragma region Library

#include "Core/SysInclude.h"

/*          Network         */
#include "Network/File.h"
#include "Network/Network.h"
#include "Network/CClient.h"
#include "Network/ServerStream.h"

/*          Models          */
#include "Models/Company.h"
#include "Models/DataTag.h" 
#include "Models/Statement.h" 
#include "Models/EodAdj.h"
#include "Models/FinDataFrame.h"
#include "Models/BufferStruct.h"

/*        Backtester        */
#include "Execution/Event.h"
#include "Execution/DataHandler.h"
#include "Execution/Strategy.h"
#include "Execution/ExSystemStructs.h"

#pragma endregion