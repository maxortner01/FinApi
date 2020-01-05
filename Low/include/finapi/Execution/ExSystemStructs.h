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
    //struct TimeStamp
    //{
    //    int year;      
    //    int month;     
    //    int day; 
//
    //    TimeStamp() { } 
//
    //    TimeStamp(int, int, int);
//
    //    TimeStamp(const TimeStamp&);
//
    //    const bool operator == (TimeStamp const &);
//
    //    const bool operator > (TimeStamp const &);       
//
    //    const bool operator < (TimeStamp const &);
    //};

    

    


namespace execution
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