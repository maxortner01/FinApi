#include "finapi/finapi.h"

#define TEMP_TYPES(type)\
    template void deserialize<std::ifstream>(type, std::ifstream&);\
    template void deserialize<Cloud::File*>(type, Cloud::File*&);

namespace finapi
{
    /*        BufferStruct.h         */ 
namespace filemethods
{
    void read(std::ifstream& file, STRING_FIELD& string)
    {
        const unsigned int size = read<unsigned int>(file);
        string = STRING_ALLOC(size);
        GET_CHAR(string, size) = '\0';

        file.read(string, size);
    }

    void read(Cloud::File* file, STRING_FIELD& string)
    {
        const unsigned int size = read<unsigned int>(file);
        string = STRING_ALLOC(size);
        GET_CHAR(string, size) = '\0';

        file->read(string, size);
    }

    template<typename T>
    unsigned int read_magic_number(T& file)
    {
        return filemethods::read<unsigned int>(file);
    }

    template unsigned int read_magic_number<std::ifstream>(std::ifstream&);
    template unsigned int read_magic_number<Cloud::File*>(Cloud::File*&);
}

    //   DataTag
    template<typename T>
    void deserialize(std::vector<DataTag*>& data, T& file)
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

    //   Company
    template<typename T>
    void deserialize(Company** data, T& file)
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

    //  Statement
    template<typename T>
    void deserialize(Statement** data, T& file)
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

    /*---------- Josh Ortner ----------*/

    //   EodAdj
    #undef NDEBUG
    void deserialize(std::vector<EodAdj*>& data, std::ifstream& file)
    {
        assert(file);

		// Read the magic number
		assert(  filemethods::read_magic_number(file) == EOD_ADJ_MN );

        // Clean the list and reserve memory for the alloted amount of objects
        clean_list(data);

        while(!file.eof())
        {
            // Create a new DataTag object and store in list as well as collecting a few
            // handles to the data
            EodAdj* newEod = new EodAdj;
            float* float_iter = (float*)newEod;
            data.push_back(newEod);

            // Read in date
            newEod->date = filemethods::read<TimeStamp>(file);

            // Field count specific to this data type
            const unsigned int FIELD_COUNT = 8;
            
            for (int j = 0; j < FIELD_COUNT; j++)
            {
                // Allocate a character buffer, and copy over
                // data from file into the buffer
                filemethods::read(file, float_iter + j);
            }
        }
        data.shrink_to_fit();
    }

    // Define template types
    TEMP_TYPES(Company**);
    TEMP_TYPES(Statement**);
    TEMP_TYPES(std::vector<DataTag*>&);
}