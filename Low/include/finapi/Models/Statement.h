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
    static void deserialize(Statement** data, std::ifstream& file)
    {
        assert(file);

        // Read the magic number
        assert( filemethods::read_magic_number(file) == STATEMENT_MN );

        // Create a reference pointer to the Statement in which we are manipulating
        // as well as a string list pointer to the fields of the statement.
        Statement*& statement = *(data);
        statement = new Statement;
        STRING_LIST str_iter = (char**)statement;

        // Get the count of fields (though this should always be the same so long
        // as the Statement type is being populated)
        unsigned int count = filemethods::read<unsigned int>(file);

        for (int i = 0; i < count - 1; i++)
        {
            // The third object is an integer in the file
            if (i == 3) filemethods::read(file, &statement->fiscal_year);

            // Everything else is a string field. So pull the next string from the file
            filemethods::read(file, GET_STRING(str_iter, i));
        }
    }
}