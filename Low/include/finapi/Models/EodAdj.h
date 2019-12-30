/**
 * @file EodAdj.h
 * 
 * @brief Data structure to represent End of Day Adjusted Stock price for a single date
 * 
 * @author   Josh Ortner
 * @date     2019-12-30
 * @version  0.0.1
 * 
 * @copyright Copyright (c) 2019
 */

#pragma once

#include <iostream>

#include "BufferStruct.h"

namespace finapi
{
    struct TimeStamp
    {
        int   year;      
        int   month;     
        int   day; 
    };

    struct EodAdj
    {
        float openPrice;
        float high;      
        float low;       
        float close;     
        float adjClose;  
        float volume;    
        float divAmount; 
        float splitCo;   

        TimeStamp date;
    };

    /**
     * @brief Deserializes a collection of EodAdj objects from a given file stream.
     * 
     * The EodAdj objects are built in this method, so before program termination, clean_list() must
     * be called.
     * 
     * @param data 
     * @param file 
     */
    void deserialize(std::vector<EodAdj*>& data, std::ifstream& file);
}