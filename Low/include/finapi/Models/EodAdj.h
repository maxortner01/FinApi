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
#include "TimeStamp.h"

namespace finapi
{
namespace models
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

        static DataModelType model_type();
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

    /**
     * @brief Returns whether the lhs obj's field data is greater than the rhs
     *        obj's data
     * 
     * @param lhs 
     * @param rhs 
     * @param field 
     * @return true 
     * @return false 
     */
    bool greater_than(const EodAdj& lhs, const EodAdj& rhs, std::string field);

    /**
     * @brief Returns whether the lhs obj's field data is less than the rhs
     *        obj's data
     * 
     * @param lhs   : EodAdj Data Model object
     * @param rhs   : EodAdj Data Model object
     * @param field : Data field to compare
     * @return true 
     * @return false 
     */
    bool less_than(const EodAdj& lhs, const EodAdj& rhs, std::string field);
    
    /**
     * @brief Display data of Data Model object to terminal
     * 
     * @param obj 
     */
    void display_model(const EodAdj& obj);
}
}