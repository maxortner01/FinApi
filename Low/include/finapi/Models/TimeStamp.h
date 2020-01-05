/**
 * @file TimeStamp.h
 * 
 * @brief 
 * 
 * @author  Josh Ortner
 * @date    2019-12-30
 * @version 0.1
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#pragma once

#include "../Core/SysInclude.h"

namespace finapi
{
namespace models
{

    /**
     * @brief Encapsulates a time stamp
     * 
     */
    struct TimeStamp
    {
        int year;      
        int month;     
        int day; 

        TimeStamp() { } 

        TimeStamp(int, int, int);

        TimeStamp(const TimeStamp&);

        const bool operator == (TimeStamp const &);

        const bool operator > (TimeStamp const &);       

        const bool operator < (TimeStamp const &);
    };
}
}