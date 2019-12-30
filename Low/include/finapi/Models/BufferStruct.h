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
#include "../Network/CClient.h"

#define assert(expr) if (!(expr)) exit(0);

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
#define GET_CHAR(var, index)   GET_STRING(var, index)
#define STRING_ALLOC(length) (char*)std::malloc(length + 1)
/* --------------------------------------------- */

namespace finapi
{
    template<typename T>
    static void clean_list(std::vector<T*>& list)
    {
        for (int i = 0; i < list.size(); i++)
            CLEAN_OBJ(list[i]);
    }
    
/**
 * @brief Common utility methods for extracting binary data from a given file stream.
 */
namespace filemethods
{
    /**
     * @brief Reads and returns a given type from a binary file.
     * 
     * @tparam T Type to read from the file.
     * @param file File stream instance directed to the binary file.
     * @return T Value from the file.
     */
    template<typename T>
    static T read(std::ifstream& file)
    {
        T r;
        file.read((char*)&r, sizeof(T));
        return r;
    }

    /**
     * @brief Reads and returns a given type from a binary file buffer.
     * 
     * @tparam T    Type to read from the file
     * @param file  Binary file buffer object
     * @return T    Value from the file
     */
    template<typename T>
    static T read(Cloud::File* file)
    {
        T r;
        file->read(&r, sizeof(T));
        return r;
    }

    /**
     * @brief Reads a value from a given binary file stream and populates the data pointer.
     * 
     * @tparam T Type to pull from the file.
     * @param file File stream instance directed to the binary file.
     * @param dest Pointer pointing to the data to be populated.
     */
    template<typename T>
    static void read(std::ifstream& file, T* dest)
    {
        file.read((char*)dest, sizeof(T));
    }

    /**
     * @brief Reads a value from a given binary file buffer and populates the data pointer.
     * 
     * @tparam T    Type to pull from the file
     * @param file  Binary file buffer object
     * @param dest  Destination of the data point
     */
    template<typename T>
    static void read(Cloud::File* file, T* dest)
    {
        file->read(dest, sizeof(T));
    }

    /**
     * @brief Reads in a string from a given file stream.
     * 
     * The string passed should be an unallocated pointer, since this function allocates
     * the right amount of memory as well as sets the string terminator.
     * 
     * @param file File stream instance directed to the binary file.
     * @param string Pointer to the string.
     */
    void read(std::ifstream& file, STRING_FIELD& string);

    /**
     * @brief Reads in a string from a given file stream.
     * 
     * As with the other function, the string must be an unallocated pointer.
     * 
     * @param file      Pointer to a binary file buffer
     * @param string    Pointer to an unallocated string
     */
    void read(Cloud::File* file, STRING_FIELD& string);

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