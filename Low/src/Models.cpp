#include "finapi/finapi.h"

// Macro to forward declare a template function
#define TYPE_TEMP_FUNC(func_name, retrn, type, ...)\
    template retrn func_name<type>(__VA_ARGS__)

// Macro to forward declare a void template function
#define VOID_TEMP_FUNC(func_name, type, ...)\
    TYPE_TEMP_FUNC(func_name, void, type, __VA_ARGS__)

// Macro to forward declare a deserialize function
#define DES_TYPE(type1, type2)\
    VOID_TEMP_FUNC(deserialize, type2, type1, type2&)

// Macro that forward declares a given deserialize function
// for each stream type
#define TEMP_TYPES(type)\
    DES_TYPE(type, std::ifstream);\
    DES_TYPE(type, Cloud::File);\
    DES_TYPE(type, Cloud::ServerStream);\

namespace finapi
{
namespace models
{
#pragma region BUFFERSTRUCT_H

#pragma region SECTION_FIELDS

    template<>
    const std::string FinFields<EodAdj, 9>::fields[9] = {
        "Date", "Open", "High", "Low", "Close", "AdjClose", "Volume", "DivAmount", "SplitCo"
    };

    template<>
    const std::string FinFields<Company, 5>::fields[5] = {
        "cik", "id", "lei", "name", "ticker"
    };

    template<>
    const std::string FinFields<DataTag, 9>::fields[9] = {
        "balance", "factor", "id", "name", "parent", "tag", "unit", "sequence", "value"
    };

    template<>
    const std::string FinFields<Statement, 8>::fields[8] = {
        "end_date", "filing_date", "fiscal_period", "id", "start_date", "statement_code", "type", "fiscal_year"
    };

#pragma endregion SECTION_FIELDS

namespace filemethods
{
    template<typename _Stream>
    void read(_Stream& file, STRING_FIELD& string)
    {
        const unsigned int size = read<unsigned int>(file);
        string = STRING_ALLOC(size);
        GET_CHAR(string, size) = '\0';

        file.read(string, size);
    }

    VOID_TEMP_FUNC(read, Cloud::File,         Cloud::File&,         STRING_FIELD&);
    VOID_TEMP_FUNC(read, std::ifstream,       std::ifstream&,       STRING_FIELD&);
    VOID_TEMP_FUNC(read, Cloud::ServerStream, Cloud::ServerStream&, STRING_FIELD&);

    template<typename T>
    unsigned int read_magic_number(T& file)
    {
        return filemethods::read<unsigned int>(file);
    }

    TYPE_TEMP_FUNC(read_magic_number, unsigned int, Cloud::File,         Cloud::File&        );
    TYPE_TEMP_FUNC(read_magic_number, unsigned int, std::ifstream,       std::ifstream&      );
    TYPE_TEMP_FUNC(read_magic_number, unsigned int, Cloud::ServerStream, Cloud::ServerStream&);

#pragma endregion
}

#pragma region DATATAG_H

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

#pragma endregion

#pragma region COMPANY_H

    template<typename T>
    void deserialize(Company*& company, T& file)
    {
        assert(file);

        // Retreive the magic number
        assert( filemethods::read_magic_number(file) == COMPANY_MN );

        // Create a pointer reference and allocate the memory for a company
        // object as well as a string pointer
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

#pragma endregion

#pragma region STATEMENT_H

    template<typename T>
    void deserialize(Statement*& statement, T& file)
    {
        assert(file);

        // Read the magic number
        assert( filemethods::read_magic_number(file) == STATEMENT_MN );

        // Create a reference pointer to the Statement in which we are manipulating
        // as well as a string list pointer to the fields of the statement.
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

#pragma endregion

#pragma region EODADJ_H

    template<typename Stream>
    void deserialize(std::vector<EodAdj*>& data, Stream& file)
    {
        assert(file);
        int magic_number;
		
        // Read the magic number
		filemethods::read(file, &magic_number);

        // Clean the list and reserve memory for the alloted amount of objects
        clean_list(data);

        EodAdj* newEod = new EodAdj;

        while(filemethods::read(file, &newEod->date))
        {
            // Field count specific to this data type
            const unsigned int FIELD_COUNT = 8;

            float* float_iter = &newEod->openPrice;
            
            for (int j = 0; j < FIELD_COUNT; j++)
            {
                // Allocate a character buffer, and copy over
                // data from file into the buffer
                filemethods::read(file, float_iter + j);
            }
            newEod->display();
            data.push_back(newEod);
            newEod = new EodAdj;
        }
        data.shrink_to_fit();
    }

    /**
     * EODADJ IMPLEMENTATION
     */

    /* METHODS */

    bool EodAdj::greater_than(const EodAdj& rhs, std::string field)
    {
        if (field == "Open")
            return openPrice > rhs.openPrice;
        else if (field == "High")
            return high > rhs.high;
        else if (field == "Low")
            return low > rhs.low;
        else if (field == "Close")
            return close > rhs.close;
        else if (field == "AdjClose")
            return adjClose > rhs.adjClose;
        else if (field == "Volume")
            return volume > rhs.volume;
        else if (field == "DivAmount")
            return divAmount > rhs.divAmount;
        else if (field == "SplitCo")
            return splitCo > rhs.splitCo;
        else if (field == "Date")
            return date > rhs.date;
    }

    bool EodAdj::less_than(const EodAdj& rhs, std::string field)
    {
        if (field == "Open")
            return openPrice < rhs.openPrice;
        else if (field == "High")
            return high < rhs.high;
        else if (field == "Low")
            return low < rhs.low;
        else if (field == "Close")
            return close < rhs.close;
        else if (field == "AdjClose")
            return adjClose < rhs.adjClose;
        else if (field == "Volume")
            return volume < rhs.volume;
        else if (field == "DivAmount")
            return divAmount < rhs.divAmount;
        else if (field == "SplitCo")
            return splitCo < rhs.splitCo;
        else if (field == "Date")
            return date < rhs.date;
    }

    void EodAdj::display()
    {
        std::cout << "\nDate=\t"   << date.month << "/" << date.day << "/" << date.year
                  << "\nOpen=\t"   << openPrice
                  << "\nHigh=\t"   << high
                  << "\nLow=\t"    << low
                  << "\nClose=\t"  << close 
                  << "\nadjCl=\t"  << adjClose
                  << "\nvolume=\t" << volume
                  << "\ndivAm=\t"  << divAmount
                  << "\nSplit=\t"  << splitCo << std::endl;
    }

    /**
     * @brief Returns the name of the designated directory
     * 
     * @return std::string 
     */
    DataModelType EodAdj::model_type()
        { return EODADJ; }

#pragma endregion

#pragma region MODEL_COMPARATOR_H

    template class ModelComparator<EodAdj>; 

    /**
     * MODEL_COMPARATOR IMPLEMENTATION
     */ 

    /* STATIC MEMBER INITIALIZATION */

    template<typename DataModel>
    std::string ModelComparator<DataModel>::field_to_compare;

    template<typename DataModel>
    bool ModelComparator<DataModel>::ascending;

    /* STATIC METHODS */

    /**
     * @brief Compare two DataModel objects
     *        NOTE: Static members ascending and field_to_compare must be initialized 
     * 
     * @tparam DataModel :
     * @param lhs          :
     * @param rhs          : 
     * @return true        : 
     * @return false       : 
     */
    template<typename DataModel>
    bool ModelComparator<DataModel>::compare(DataModel lhs, DataModel rhs)
    {
        if (ascending)
            return lhs.greater_than(rhs, field_to_compare);
        else
            return lhs.less_than(rhs, field_to_compare);
    } 

    /**
     * @brief Compare two DataModel pointers
     *        NOTE: Static members ascending and field_to_compare must be initialized 
     * 
     * @tparam DataModel :
     * @param lhs          :
     * @param rhs          : 
     * @return true        : 
     * @return false       : 
     */
    template<typename DataModel>
    bool ModelComparator<DataModel>::compare_ptr(DataModel* lhs, DataModel* rhs)
    {
        if (ascending)
            return lhs->less_than(*rhs, field_to_compare);
        else
            return lhs->greater_than(*rhs, field_to_compare);
    } 

#pragma endregion

#pragma region TIME_STAMP_H

    /* CONSTRUCTORS */

    /**
     * @brief Construct a new Time Stamp:: Time Stamp object
     * 
     * @param y : year
     * @param m : month
     * @param d : day
     */
    TimeStamp::TimeStamp(int y, int m, int d) :
        year(y), month(m), day(d) { }

    TimeStamp::TimeStamp(const TimeStamp& rhs) :
        year(rhs.year), month(rhs.month), day(rhs.day) { }

    /* OPERATOR OVERLOADS */

    const bool TimeStamp::operator == (TimeStamp const &rhs)
        { return (year == rhs.year) && (month == rhs.month) && (day == rhs.day); }

    const bool TimeStamp::operator > (TimeStamp const &rhs)
    {
        if (year > rhs.year)
            return true;
        else if (year < rhs.year)
            return false;
        else if (month > rhs.month)
            return true;
        else if (month < rhs.month)
            return false;
        else if (day > rhs.day)
            return true;
        else
            return false;           
    }

    const bool TimeStamp::operator < (TimeStamp const &rhs)
    {
        if (year < rhs.year)
            return true;
        else if (year > rhs.year)
            return false;
        else if (month < rhs.month)
            return true;
        else if (month > rhs.month)
            return false;
        else if (day < rhs.day)
            return true;
        else
            return false;           
    }

#pragma endregion

    // Define template types
    TEMP_TYPES(Company*&             );
    TEMP_TYPES(Statement*&           );
    TEMP_TYPES(std::vector<EodAdj*>& );
    TEMP_TYPES(std::vector<DataTag*>&);
}
}