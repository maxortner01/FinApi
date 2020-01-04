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

//#include <string>

#include "../Backtester/ExSystemStructs.h"
#include "BufferStruct.h"

namespace finapi
{
    struct EodAdjFields : Fields
    {
        std::string openPrice;
        std::string high;
        std::string low;
        std::string close;
        std::string adjClose;
        std::string volume;
        std::string divAmount;
        std::string splitCo;
        std::string date;

        /* CONSTRUCTOR */

        EodAdjFields();

        /* METHODS */

        void display();
    };

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

        EodAdjFields* get_fields();

        float greater_than(const EodAdj&, std::string);

        float less_than(const EodAdj&, std::string);

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