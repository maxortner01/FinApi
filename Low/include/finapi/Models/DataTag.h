/**
 * @file DataTag.h
 * 
 * @brief First data structure that represents a tag of data within a financial statement.
 * 
 * @author   Max Ortner
 * @date     2019-12-13
 * @version  0.0.3
 * 
 * @copyright Copyright (c) 2019
 */

#pragma once

#include <iostream>

#include "BufferStruct.h"

namespace finapi
{
namespace models
{
    struct DataTag
    {
        STRING_FIELD balance;
        STRING_FIELD factor;
        STRING_FIELD id;
        STRING_FIELD name;
        STRING_FIELD parent;
        STRING_FIELD tag;
        STRING_FIELD unit;
        
        int   sequence;
        float value;

        CONSTRUCT_BUFF(DataTag, 7);
        CONSTRUCT_DEST(DataTag);
    };

    /**
     * @brief Deserializes a collection of DataTag objects from a given file stream.
     * 
     * The DataTag objects are built in this method, so before program termination, clean_list() must
     * be called.
     * 
     * @param data 
     * @param file 
     */
    template<typename T>
    void deserialize(std::vector<DataTag*>& data, T& file);
}
}