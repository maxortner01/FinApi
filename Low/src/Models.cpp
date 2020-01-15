#include "finapi/finapi.h"

#include <iomanip>

// Macro to forward declare a template function
#define TYPE_TEMP_FUNC(func_name, retrn, type, ...)\
    template retrn func_name<type>(__VA_ARGS__)

// Macro to forward declare a void template function
#define VOID_TEMP_FUNC(func_name, type, ...)\
    TYPE_TEMP_FUNC(func_name, void, type, __VA_ARGS__)

// Macro to forward declare a deserialize function
#define DES_TYPE(type1, type2)\
    VOID_TEMP_FUNC(deserialize, type2, type1, type2&);

// Macro that forward declares a given deserialize function
// for each stream type
#define TEMP_TYPES(type)\
    DES_TYPE(type, std::ifstream);\
    DES_TYPE(type, Cloud::File);\
    DES_TYPE(type, Cloud::ServerStream);\

#define FIN_FIELD_NOT_FOUND 0

namespace finapi
{
namespace models
{

#pragma region FIELDS_H

    template<>
    c_uint FinFields<EodAdj>::count = 9;
    template<>
    const std::string FinFields<EodAdj>::fields[9] = {
        "Date", "Open", "High", "Low", "Close", "AdjClose", "Volume", "DivAmount", "SplitCo"
    };

    template<>
    c_uint FinFields<Company>::count = 5;
    template<>
    const std::string FinFields<Company>::fields[5] = {
        "cik", "id", "lei", "name", "ticker"
    };

    template<>
    c_uint FinFields<DataTag>::count = 9;
    template<>
    const std::string FinFields<DataTag>::fields[9] = {
        "balance", "factor", "id", "name", "parent", "tag", "unit", "sequence", "value"
    };

    template<>
    c_uint FinFields<Statement>::count = 8;
    template<>
    const std::string FinFields<Statement>::fields[8] = {
        "end_date", "filing_date", "fiscal_period", "id", "start_date", "statement_code", "type", "fiscal_year"
    };

#define FIELD_OPERATOR_TEMP(name, type1, type2)\
    template bool name<type1, type2> (const type1&, const type1&, const std::string&);

#define FIELD_OPERATOR_FUNCTION(name, operator)\
    template<typename _Model, typename T>\
    bool name(const _Model& model1, const _Model& model2, const std::string& field)\
    {\
        for (int i = 0; i < FinFields<_Model>::count; i++)\
            if (FinFields<_Model>::fields[i] == field)\
                return get_field<T>(model1, field) operator get_field<T>(model2, field);\
        assert(FIN_FIELD_NOT_FOUND);\
    }\
    FIELD_OPERATOR_TEMP(name, EodAdj, float);\
    FIELD_OPERATOR_TEMP(name, EodAdj, TimeStamp);\
    FIELD_OPERATOR_TEMP(name, DataTag, float);\
    FIELD_OPERATOR_TEMP(name, DataTag, TimeStamp);\
    FIELD_OPERATOR_TEMP(name, Company, float);\
    FIELD_OPERATOR_TEMP(name, Company, TimeStamp);\
    FIELD_OPERATOR_TEMP(name, Statement, float);\
    FIELD_OPERATOR_TEMP(name, Statement, TimeStamp);

    FIELD_OPERATOR_FUNCTION(greater_than, >);
    FIELD_OPERATOR_FUNCTION(less_than,    <);

#undef FIELD_OPERATOR_FUNCTION
#undef FIELD_OPERATOR_TEMP

#pragma endregion

#pragma region BUFFERSTRUCT_H

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

#pragma BAR_H

    /**
     * BAR IMPLEMENTATION
     */ 

    /* CONSTRUCTORS */

    Bar::Bar() :
        _open(nullptr), _high(nullptr), _low(nullptr), _close(nullptr), _volume(nullptr), _time(nullptr) { }

    Bar::Bar(c_Fref o, c_Fref h, c_Fref l, c_Fref c, c_Fref v, c_TSref ts) :
        _open(&o), _high(&h), _low(&l), _close(&c), _volume(&v), _time(&ts) { }

    /* METHODS */

    float Bar::open() const
        { return _open->value; }

    float Bar::high() const  
        { return _high->value; }

    float Bar::low() const
        { return _low->value; }

    float Bar::close() const
        { return _close->value; }

    float Bar::volume() const
        { return _volume->value; }

    TimeStamp Bar::time() const
        { return _time->value; }

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
        unsigned int size = sizeof(EodAdj);
        while(filemethods::read(file, newEod->date.ptr()))
        {
            // Field count specific to this data type
            const unsigned int FIELD_COUNT = FinFields<EodAdj>::count - 1;

            Field<float>* float_iter = &newEod->openPrice;
            
            for (int j = 0; j < FIELD_COUNT; j++)
                filemethods::read(file, (float_iter + j)->ptr());
            //assert(newEod->date().year != 0);
            data.push_back(newEod);
            newEod = new EodAdj;
        }
        
        data.shrink_to_fit();
    }

    /**
     * EODADJ IMPLEMENTATION
     */

    
    void display_model(const EodAdj& obj)
    {
        std::cout << "\nDate=\t"   << obj.date().month << "/" << obj.date().day << "/" << obj.date().year
                  << "\nOpen=\t"   << obj.openPrice()
                  << "\nHigh=\t"   << obj.high()
                  << "\nLow=\t"    << obj.low()
                  << "\nClose=\t"  << obj.close()
                  << "\nadjCl=\t"  << obj.adjClose()
                  << "\nvolume=\t" << obj.volume()
                  << "\ndivAm=\t"  << obj.divAmount()
                  << "\nSplit=\t"  << obj.splitCo() << std::endl; 
    }

    /**
     * @brief Returns the name of the designated directory
     * 
     * @return std::string 
     */
    DataModelType EodAdj::model_type()
        { return EODADJ; }

    /**
     * @brief Returns the data in Bar form to be used in execution system
     * 
     * @return Bar 
     */
    Bar EodAdj::get_bar()
        { return Bar(openPrice, high, low, adjClose, volume, date); }

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
     * @param lhs        :
     * @param rhs        : 
     * @return true      : 
     * @return false     : 
     */
    template<typename DataModel>
    bool ModelComparator<DataModel>::compare(DataModel lhs, DataModel rhs)
    {
        // I hate this second if statement but oh well
        if (ascending)
        {
            if (field_to_compare == "Date")
            {
                std::cout << "YO WHATS UP\n";
                return less_than<DataModel, TimeStamp>(lhs, rhs, field_to_compare);
            }
            return less_than<DataModel>(lhs, rhs, field_to_compare);
        }
        else
        {
            if (field_to_compare == "Date")
            {
                std::cout << "YO WHATS UP\n";
                return greater_than<DataModel, TimeStamp>(lhs, rhs, field_to_compare);
            }
            
            return greater_than<DataModel>(lhs, rhs, field_to_compare);
        }
    } 

    /**
     * @brief Compare two DataModel pointers
     *        NOTE: Static members ascending and field_to_compare must be initialized 
     * 
     * @tparam DataModel :
     * @param lhs        :
     * @param rhs        : 
     * @return true      : 
     * @return false     : 
     */
    template<typename DataModel>
    bool ModelComparator<DataModel>::compare_ptr(DataModel* lhs, DataModel* rhs)
    {
        if (ascending)
            return less_than<DataModel>(*lhs, *rhs, field_to_compare);
        else
            return greater_than<DataModel>(*lhs, *rhs, field_to_compare);
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

    TimeStamp& TimeStamp::operator = (const TimeStamp& rhs)
    {
        if (this != &rhs)
        {
            year  = rhs.year;
            month = rhs.month;
            day   = rhs.day;
        }
        return (*this);
    }

    bool TimeStamp::operator == (const TimeStamp& rhs) const 
        { return (year == rhs.year) && (month == rhs.month) && (day == rhs.day); }

    bool TimeStamp::operator > (const TimeStamp& rhs) const 
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

    bool TimeStamp::operator < (const TimeStamp& rhs) const
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
namespace modeler
{

#pragma region FIN_DATA_FRAME_H

    template class FinDataFrame<models::EodAdj, Cloud::File>;
    template class FinDataFrame<models::EodAdj, Cloud::ServerStream>;
    template class FinDataFrame<models::EodAdj, std::ifstream>; 

    /* CONSTRUCTORS */

    /**
     * @brief Construct a new Fin Data Frame< Model Struct,  Conn Type>:: Fin Data Frame object
     * 
     * @tparam _DataModel :
     * @tparam _Stream    :
     * @param connection : data stream
     */
    template<typename _DataModel, typename _Stream>
    FinDataFrame<_DataModel, _Stream>::FinDataFrame(_Stream& connection)
    {
        deserialize(data, connection);
        it = data.begin();
        //connection.close();             
    }

    /* DESTRUCTOR */

    /**
     * @brief Destroy the Fin Data Frame< Model Struct,  Conn Type>:: Fin Data Frame object
     * 
     * @tparam _DataModel :
     * @tparam _Stream    :
     */
    template<typename _DataModel, typename _Stream>
    FinDataFrame<_DataModel, _Stream>::~FinDataFrame()
        { clean_list(data); }

    /* METHODS */

    /**
     * @brief Iterates through the data return a pointer to the current data model
     *        until the end of the data is reached
     * 
     * @tparam _DataModel :
     * @tparam _Stream    :
     * @param dat        : pointer that will be assigned the location of data struct
     * @return true      : data returned
     * @return false     : end of data reached
     */
    template<typename _DataModel, typename _Stream>
    bool FinDataFrame<_DataModel, _Stream>::get_next_model(_DataModel*& dat)
    {
        if (it == data.end())
        {
            it = data.begin();
            return false;
        }

        dat = (*it);
        it++;

        return true;
    }

    /**
     * @brief Iterates through the data return a pointer to the current data model
     *        until the end of the data is reached 
     * 
     * @tparam _DataModel 
     * @tparam _Stream 
     * @param bar 
     * @return true 
     * @return false 
     */
    template<typename _DataModel, typename _Stream>
    bool FinDataFrame<_DataModel, _Stream>::get_next_bar(models::Bar& bar)
    {
        if (it == data.end())
        {
            it = data.begin();
            return false;
        }

        bar = (*it)->get_bar();
        it++;

        return true;
    }

    /**
     * @brief Sorts the data by a specified field
     * 
     * @tparam _DataModel :
     * @tparam _Stream    : 
     * @param field      : field to sort by
     * @param ascd       : sort direction, true - ascending, false - descending
     */
    template<typename _DataModel, typename _Stream>
    void FinDataFrame<_DataModel, _Stream>::sort_data(std::string field, bool ascd)
    { 
        // set comparator variables
        //models::ModelComparator<_DataModel> comparator;
        models::ModelComparator<_DataModel>::field_to_compare = field;
        models::ModelComparator<_DataModel>::ascending = ascd;
        sort(data.begin(), data.end(), &models::ModelComparator<_DataModel>::compare_ptr);
    }

    /**
     * @brief Append data from another stream to data vector
     *        NOTE: Data may need to be reordered after data is appended
     * 
     * @tparam _DataModel :
     * @tparam _Stream    :
     * @param connection : data _stream
     */
    template<typename _DataModel, typename _Stream>
    void FinDataFrame<_DataModel, _Stream>::append(_Stream*& connection)
    { 
        // deserialize new connection
        std::vector<_DataModel*> newData;
        deserialize(newData, *connection);
        
        for (int i = 0; i < newData.size(); i++)
            data.push_back(newData[i]);

        it = data.begin();
        if (connection)
            delete connection;
    }

    /**
     * @brief Print the first ten structs to the terminal
     * 
     * @tparam _DataModel 
     * @tparam _Stream 
     */
    template<typename _DataModel, typename _Stream>
    void FinDataFrame<_DataModel, _Stream>::display_head()
    {
        for (int i = 0; i < 10; i++)
            models::display_model(*data[i]);
    }

    /**
     * @brief Print the last ten structs to the terminal
     * 
     * @tparam _DataModel 
     * @tparam _Stream 
     */
    template<typename _DataModel, typename _Stream>
    void FinDataFrame<_DataModel, _Stream>::display_tail()
    {
        for (int i = data.size() - 10; i < data.size(); i++)
            models::display_model(*data[i]);
    }

    /**
     * @brief Returns the DataModelType Enum associated with data
     * 
     * @tparam _DataModel 
     * @tparam _Stream 
     */
    template<typename _DataModel, typename _Stream>
    DataModelType FinDataFrame<_DataModel, _Stream>::get_model_type()
        { return data[0]->model_type(); }

#pragma endregion

#pragma region EQUITY_CURVE_H

    /* CONSTRUCTORS */

    EquityCurveNode::EquityCurveNode() { }

    EquityCurveNode::EquityCurveNode(const EquityCurveNode& obj)
    {
        total        = obj.total;
        returns      = obj.returns;
        equity_curve = obj.equity_curve;
        time         = obj.time;
    }

    /* OPERATOR OVERLOADS */

    EquityCurveNode& EquityCurveNode::operator = (const EquityCurveNode& rhs)
    {
        if (this != &rhs)
        {
            total        = rhs.total;
            returns      = rhs.returns;
            equity_curve = rhs.equity_curve;
            time         = rhs.time;
        }
        return (*this);
    }

    /* CONSTRUCTOR */

    /**
     * @brief Construct a new Equity Curve:: Equity Curve object
     *        Uses the holdings construct used in the portfolio class
     * 
     * @param holdings 
     */
    EquityCurve::EquityCurve(const HoldingsDataFrame<std::string, float>& holdings) 
        { construct_equity_curve(holdings); }

    /**
     * @brief Copy Constructor
     *        Construct a new Equity Curve:: Equity Curve object
     * 
     * @param obj 
     */
    EquityCurve::EquityCurve(const EquityCurve& obj)
    {
        clear_data();
        _data.reserve(obj._data.size());
        for (int i = 0; i < _data.size(); i++)
            _data[i] = new EquityCurveNode(*obj._data[i]);
    }

    /**
     * @brief Move Constructor
     *        Construct a new Equity Curve:: Equity Curve object
     * 
     * @param obj 
     */
    EquityCurve::EquityCurve(EquityCurve&& obj)
    {
        clear_data();
        _data.reserve(obj._data.size());
        // copy data pointer
        std::copy(obj._data.begin(), obj._data.end(), _data.begin());
        obj.clear_data();
    }

    /* DESTRUCTOR */

    /**
     * @brief Destroy the Equity Curve:: Equity Curve object
     * 
     */
    EquityCurve::~EquityCurve()
        { clear_data(); }

    /* METHODS */

    /**
     * @brief Holdings must contain the following fields for each map in vector:
     *          - total
     *          - year
     *          - month
     *          - day
     * 
     * @param holdings 
     */
    void EquityCurve::construct_equity_curve(const HoldingsDataFrame<std::string, float>& holdings)
    {
        clear_data();
        _data.reserve(holdings.size());

        EquityCurveNode* prev_node;

        // set values for first node
        _data[0] = new EquityCurveNode;
        _data[0]->total        = holdings[0]["total"];
        _data[0]->returns      = 0.0;
        _data[0]->equity_curve = 1.0;
        _data[0]->time.year    = holdings[0]["year"]; 
        _data[0]->time.month   = holdings[0]["month"];
        _data[0]->time.day     = holdings[0]["day"];
        prev_node = _data[0];

        // iterate through remaining nodes
        for (int i = 1; i < holdings.size(); i++)
        {
            _data[i]->total = holdings[i]["total"];
            // percent change calc
            _data[i]->returns = (_data[i]->total - prev_node->total) / prev_node->total;
            _data[i]->equity_curve = (1.0 + _data[i]->returns) * prev_node->returns;
            _data[i]->time.year    = holdings[0]["year"]; 
            _data[i]->time.month   = holdings[0]["month"];
            _data[i]->time.day     = holdings[0]["day"];

            prev_node = _data[i];
        }

        it = _data.begin();
    }

    /**
     * @brief Deallocate pointers in vector and clear vector
     * 
     */
    void EquityCurve::clear_data()
    {
        for (int i = 0; i < _data.size(); i++)
            delete _data[i];
        _data.clear();
    }

    /**
     * @brief Writes all data to a file in a standardized format
     *        so it can easily be read in by Python for plotting.
     *        FILE NAME SHOULD NOT INCLUDE EXTENSION: ".eqc" will be added.
     * 
     */
    bool EquityCurve::to_file(std::string file_dir, std::string file_name)
    {
        std::string final_dir = file_dir + file_name + ".eqc";
        if (_data.size())
        {
            std::ofstream out_file(final_dir);
            if (out_file.is_open())
            {
                // write fields to file
                out_file << "year,month,day,total,returns,equity_curve\n";
                // iterate through each map in fields
                for (int i = 0; i < _data.size(); i++)
                {
                    out_file << _data[i]->time.year    << "," 
                             << _data[i]->time.month   << ","
                             << _data[i]->time.day     << ","
                             << _data[i]->total        << ","
                             << _data[i]->returns      << ","
                             << _data[i]->equity_curve << "\n";
                }
                out_file.close();
                return true;
            }
        }
        return false;
    }

    /* OVERLOADS */

    /**
     * @brief Copy assignment
     * 
     * @param rhs 
     * @return EquityCurve& 
     */
    EquityCurve& EquityCurve::operator = (const EquityCurve& rhs)
    {
        if (this != &rhs)
        {
            clear_data();
            _data.reserve(rhs._data.size());
            for (int i = 0; i < _data.size(); i++)
                _data[i] = new EquityCurveNode(*rhs._data[i]);
        }
        return (*this);
    }

    /**
     * @brief Move assignment
     *        TO DO : CAN THIS BE OPTIMIZED MORE??
     * 
     * @param rhs 
     * @return EquityCurve& 
     */
    EquityCurve& EquityCurve::operator = (EquityCurve&& rhs)
    {
        if (this != &rhs)
        {
            clear_data();
            _data.reserve(rhs._data.size());
            // copy data pointer
            std::copy(rhs._data.begin(), rhs._data.end(), _data.begin());
            rhs.clear_data();
        }
        return (*this);
    }

#pragma endregion


}
}