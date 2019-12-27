/**
 * @file Statement.h
 * 
 * @brief This data structure represents the information about a particular financial statement.
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
    /**
     * @brief Simple data model for holding information related to a financial statement.
     * 
     * While one statement holds many DataTags, there is only one Statement object per
     * financial statement. This holds the information unique to each individual statement,
     * as the Company type holds data related to a specific company.
     */
    struct Statement
    {
        STRING_FIELD end_date;
        STRING_FIELD filing_date;
        STRING_FIELD fiscal_period;
        STRING_FIELD id;
        STRING_FIELD start_date;
        STRING_FIELD statement_code;
        STRING_FIELD type;

        int          fiscal_year;

        CONSTRUCT_BUFF(Statement, 7);
        CONSTRUCT_DEST(Statement);
    };

    /**
     * @brief Deserializes a Statement object from a given file stream.
     * 
     * @param data Pointer to an unallocated Statement pointer.
     * @param file Binary file stream to read from.
     */
    template<typename T>
    static void deserialize(Statement** data, T& file);
}