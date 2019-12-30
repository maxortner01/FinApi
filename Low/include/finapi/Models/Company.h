/**
 * @file Company.h
 * 
 * @brief Basic data structure representing a single company.
 * 
 * There should be one file per ticker that has been pulled into the database.
 * 
 * @author   Max Ortner
 * @date     2019-12-15
 * @version  0.0.1
 * 
 * @copyright Copyright (c) 2019
 */

#pragma once

#include "BufferStruct.h"

namespace finapi
{
    struct Company
    {
        STRING_FIELD cik;
        STRING_FIELD id;
        STRING_FIELD lei;
        STRING_FIELD name;
        STRING_FIELD ticker;

        CONSTRUCT_BUFF(Company, 5);
        CONSTRUCT_DEST(Company);
    };

    /**
     * @brief Deserializes a Company object from a given binary file stream.
     * 
     * @param data Pointer to an unallocated Company pointer.
     * @param file Binary file stream.
     */
    template<typename T>
    void deserialize(Company** data, T& file);
}