/**
 * @file BufferStruct.h
 * 
 * @brief A few macros to share functionality to model structs without affecting its memory.
 * 
 * This problem could easily be solved using inheritance and a destructor, but doing 
 * this messes with the memory layout and thus defeats the purpose of the deserializers.
 * Now, instead of inheritance, one can just add these macros to the bottom of a list of
 * fields and pass the amount of string fields so they'll get freed up on destruction.
 * 
 * This is also where we can store our magic numbers individually.
 * 
 * @author   Max Ortner
 * @date     2019-12-15
 * @version  0.0.1
 * 
 * Created On: 12/15/2019
 * 
 * @copyright Copyright (c) 2019
 */

#pragma once

#include "../Core/Core.h"
#include "../Network/ServerStream.h"

#define CONSTRUCT_BUFF(class_name, f)\
    class_name() : fields(f) {    }\
    private:\
    const unsigned int fields;\
    public:

#define CONSTRUCT_DEST(class_name)\
    ~class_name() {\
        char** scalar_this = (char**)this;\
        for (unsigned int i = 0; i < fields; i++)\
            std::free(*(scalar_this + i));\
    }

/* ---------- MAGIC NUMBER DEFINITIONS --------- */
#define DATA_TAG_MN  0
#define STATEMENT_MN 1
#define COMPANY_MN   2
#define EOD_ADJ_MN   4
/* --------------------------------------------- */

/* ------------ STRING DEFINITIONS ------------ */
#define STRING_FIELD char*
#define STRING_LIST  char**
#define GET_STRING(var, index) *(var + index)
#define GET_CHAR(var, index) GET_STRING(var, index)
#define STRING_ALLOC(length) (char*)std::malloc(length + 1)
/* --------------------------------------------- */

#include <iostream>

namespace finapi
{
namespace models
{

    template<typename _Model>
    bool greater_than(const _Model& model1, const _Model& model2, const std::string& field);

    /**
     * @brief Cleans a given list by calling the destructor on each object.
     * 
     * @tparam T    Type stored in the list
     * @param list  List reference
     */
    template<typename T>
    static void clean_list(std::vector<T*>& list)
    {
        for (int i = 0; i < list.size(); i++)
            CLEAN_OBJ(list[i]);
        list.clear();
    }
    
namespace filemethods
{
    /**
     * @brief Reads a value from a given stream object and populates the data pointer.
     * 
     * @tparam _Stream  Stream object to read from
     * @tparam T        Type to pull from the file
     * @param file      File stream instance directed to the binary file
     * @param dest      Pointer pointing to the data to be populated
     */
    template<typename T, typename _Stream>
    bool read(_Stream& file, T* dest)
    {
        return (file.read((char*)dest, sizeof(T)).good());
    }

    /**
     * @brief Reads and returns a given type from a stream.
     * 
     * @tparam _Stream  Stream object to read from
     * @tparam T        Type to read from the file
     * @param file      File stream instance directed to the binary file
     * @return T        Value from the file
     */
    template<typename T, typename _Stream>
    T read(_Stream& file)
    {
        T r;
        read(file, &r);
        return r;
    }

    /**
     * @brief Reads in a string from a given file stream.
     * 
     * The string passed should be an unallocated pointer, since this function allocates
     * the right amount of memory as well as sets the string terminator.
     * 
     * @tparam _Stream  Type of stream to accept input from
     * @param  file     File stream instance directed to the binary file.
     * @param  string   Pointer to the string.
     */
    template<typename _Stream>
    void read(_Stream& file, STRING_FIELD& string);

    /**
     * @brief Simple function that reads in the magic number.
     * 
     * @tparam T            File buffer type
     * @param file          File stream instance directed to the binary file.
     * @return unsigned int The magic number of a file.
     */
    template<typename T>
    unsigned int read_magic_number(T& file);
}
}
}