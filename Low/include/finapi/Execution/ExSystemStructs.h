/**
 * @file ExSystemStructs.h
 * 
 * @brief 
 * 
 * @author  Josh Ortner
 * @date    2019-12-30
 * @version 0.1
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#pragma once

#include "../Core/SysInclude.h"

namespace finapi
{

    /* All possible Brokers */

    enum BrokerType { ALPACA };

    /* All possible Data Models  */

    enum DataModelType { EODADJ };

    /**
     * @brief Encapsulates a time stamp
     * 
     */
    struct TimeStamp
    {
        int year;      
        int month;     
        int day; 

        TimeStamp() { } 

        TimeStamp(int, int, int);

        TimeStamp(const TimeStamp&);

        const bool operator == (TimeStamp const &);

        const bool operator > (TimeStamp const &);       

        const bool operator < (TimeStamp const &);
    };

    /**
     * @brief Used to compare two models given a desired field
     * 
     */
    template<typename ModelStruct>
    class ModelComparator
    {
    public:

        static std::string field_to_compare;

        static bool ascending;

        /* METHODS */

        static bool compare(ModelStruct, ModelStruct);

        static bool compare_ptr(ModelStruct*, ModelStruct*);

    };

    /**
     * MODEL_COMPARATOR IMPLEMENTATION
     */ 

    /* STATIC MEMBER INITIALIZATION */

    template<typename ModelStruct>
    std::string ModelComparator<ModelStruct>::field_to_compare;

    template<typename ModelStruct>
    bool ModelComparator<ModelStruct>::ascending;

    /* STATIC METHODS */

    /**
     * @brief Compare two ModelStruct objects
     *        NOTE: Static members ascending and field_to_compare must be initialized 
     * 
     * @tparam ModelStruct :
     * @param lhs          :
     * @param rhs          : 
     * @return true        : 
     * @return false       : 
     */
    template<typename ModelStruct>
    bool ModelComparator<ModelStruct>::compare(ModelStruct lhs, ModelStruct rhs)
    {
        if (ascending)
            return lhs.greater_than(rhs, field_to_compare);
        else
            return lhs.less_than(rhs, field_to_compare);
    } 

    /**
     * @brief Compare two ModelStruct pointers
     *        NOTE: Static members ascending and field_to_compare must be initialized 
     * 
     * @tparam ModelStruct :
     * @param lhs          :
     * @param rhs          : 
     * @return true        : 
     * @return false       : 
     */
    template<typename ModelStruct>
    bool ModelComparator<ModelStruct>::compare_ptr(ModelStruct* lhs, ModelStruct* rhs)
    {
        if (ascending)
            return lhs->less_than(*rhs, field_to_compare);
        else
            return lhs->greater_than(*rhs, field_to_compare);
    } 


namespace backtest
{
    typedef const float* const  c_fptr;
    typedef const float &       c_fref;

    /**
     * @brief Encapsulates OHLVC Bar which can be created from other price 
     *        data structs without copying data unnecessarily
     * 
     */
    class Bar
    {
        c_fptr _open;
        c_fptr _high;
        c_fptr _low;
        c_fptr _close;
        c_fptr _volume;

    public:

        /* CONSTRUCTORS */

        Bar(c_fref, c_fref, c_fref, c_fref, c_fref);

        /* METHODS */

        float open() const;

        float high() const;

        float low() const;

        float close() const;

        float volume() const;

    };
}
}