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
    static void deserialize(Company** data, std::ifstream& file)
    {
        assert(file);

        // Retreive the magic number
        assert( filemethods::read_magic_number(file) == COMPANY_MN );

        // Create a pointer reference and allocate the memory for a company
        // object as well as a string pointer
        Company*& company    = *(data);
        company              = new Company;
        STRING_LIST str_iter = (char**)company;

        // Get the count of fields (like with Statement, this should always
        // be 5)
        unsigned int count = filemethods::read<unsigned int>(file);

        // Go through each field and allocate and pull the string from the file
        // and into the object
        for (int i = 0; i < count; i++)
            filemethods::read(file, GET_STRING(str_iter, i));
    }
}