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

#include "../Execution/ExSystemStructs.h"
#include "BufferStruct.h"

namespace finapi
{
    struct EodAdj
    {
        TimeStamp date;
        float openPrice;
        float high;      
        float low;       
        float close;     
        float adjClose;  
        float volume;    
        float divAmount; 
        float splitCo;   

        /* METHODS */

        backtest::Bar generate_bar();

        bool greater_than(const EodAdj&, std::string);

        bool less_than(const EodAdj&, std::string);

        void display();

        DataModelType model_type();
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
    template<typename T>
    void deserialize(std::vector<EodAdj*>& data, T& file);
}