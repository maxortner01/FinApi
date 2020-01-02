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

#include <vector>
#include <string>

namespace finapi
{

    /**
     * @brief Base struct for data models to define specified fields
     * 
     */
    struct Fields
    {
        virtual void display() = 0;
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

        //static bool compare(ModelStruct lhs, ModelStruct rhs)
        //{
        //    if (ascending)
        //        return lhs.greater_than(rhs, field_to_compare);
        //    else
        //        return lhs.less_than(rhs, field_to_compare);
        //} 

        static bool compare(ModelStruct* lhs, ModelStruct* rhs)
        {
            if (ascending)
                return lhs->less_than(*rhs, field_to_compare);
            else
                return lhs->greater_than(*rhs, field_to_compare);
        } 

    };

    template<typename ModelStruct>
    std::string ModelComparator<ModelStruct>::field_to_compare;

    template<typename ModelStruct>
    bool ModelComparator<ModelStruct>::ascending;

    /**
     * @brief 
     * 
     */
    struct TimeStamp
    {
        int year;      
        int month;     
        int day; 

        TimeStamp() { } 

        TimeStamp(int y, int m, int d) :
            year(y), month(m), day(d) { }

        const bool operator == (TimeStamp const &rhs)
            { return (year == rhs.year) && (month == rhs.month) && (day == rhs.day); }

        const bool operator > (TimeStamp const &rhs)
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

        const bool operator < (TimeStamp const &rhs)
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
    };



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

        Bar(c_fref o, c_fref h, c_fref l, c_fref c, c_fref v) :
            _open(&o), _high(&h), _low(&l), _close(&c), _volume(&v) { }

        /* METHODS */

        float open() const
            { return (*_open); }

        float high() const  
            { return (*_high); }

        float low() const
            { return (*_low); }

        float close() const
            { return (*_close); }

        float volume() const
            { return (*_volume); }

    };
}
}