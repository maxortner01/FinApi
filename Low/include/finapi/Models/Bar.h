/**
 * @file Bar.h
 * 
 * @brief Data structure to provide a uniform interface for data in all models
 * 
 * @author   Josh Ortner
 * @date     2020-1-13
 * @version  0.0.1
 * 
 * @copyright Copyright (c) 2019
 */

#pragma once

namespace finapi
{
namespace models
{
    typedef Field<float>*      c_Fptr;
    typedef Field<float> &     c_Fref;
    typedef Field<TimeStamp>*  c_TSptr;
    typedef Field<TimeStamp> & c_TSref;

    /**
     * @brief Encapsulates OHLVC Bar which can be created from other price 
     *        data structs without copying data unnecessarily
     * 
     */
    class Bar
    {
        c_Fptr _open;
        c_Fptr _high;
        c_Fptr _low;
        c_Fptr _close;
        c_Fptr _volume;
        c_TSptr _time;

    public:

        /* CONSTRUCTORS */

        Bar();

        Bar(c_Fref, c_Fref, c_Fref, c_Fref, c_Fref, c_TSref);

        /* METHODS */

        float open() const;

        float high() const;

        float low() const;

        float close() const;

        float volume() const;

        TimeStamp time() const;

    };   
}
}