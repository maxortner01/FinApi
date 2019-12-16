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
    static void deserialize(std::vector<DataTag*>& data, std::ifstream& file)
    {
        assert(file);

		// Read the magic number
		assert( filemethods::read_magic_number(file) == DATA_TAG_MN );
		
        // Read in the object count
        unsigned int count = filemethods::read<unsigned int>(file);

        // Clean the list and reserve memory for the alloted amount of objects
        clean_list(data);
        data.reserve(count);

        for (int i = 0; i < count; i++)
        {
            // Create a new DataTag object and store in list as well as collecting a few
            // handles to the data
            data.push_back(new DataTag);
            DataTag*    scalar_tag = data.back();
            STRING_LIST field_iter = (char**)scalar_tag;

            // Field count specific to this data type
            const unsigned int FIELD_COUNT = 7;
            
            for (int j = 0; j < FIELD_COUNT; j++)
            {
                // At the fifth data point, read in an integer that is the sequence field
                if (j == 5) filemethods::read(file, &scalar_tag->sequence);

                // Read the size of the string, then pass the string to the object
                unsigned int size = filemethods::read<unsigned int>(file);

                // Allocate a character buffer, and copy over
                // data from file into the buffer
                filemethods::read(file, GET_STRING(field_iter, j));
            }

            // Finally, pull the value
            filemethods::read(file, &scalar_tag->value);
        }
    }
}