/**
 * @file EodAdj.h
 * 
 * @brief Data structure to represent End of Day Adjusted Stock price for a single date
 * 
 * @author   Josh Ortner
 * @date     2019-12-30
 * @version  0.0.1
 * 
 * @copyright Copyright (c) 2019
 */

#pragma once

//#include <string>

#include "../Backtester/ExSystemStructs.h"
#include "BufferStruct.h"

namespace finapi
{
    struct EodAdjFields : Fields
    {
        std::string openPrice;
        std::string high;
        std::string low;
        std::string close;
        std::string adjClose;
        std::string volume;
        std::string divAmount;
        std::string splitCo;
        std::string date;

        EodAdjFields() :
            openPrice("Open"), high("High"), low("Low"), close("Close"), adjClose("AdjClose"), 
            volume("Volume"), divAmount("DivAmount"), splitCo("SplitCo"), date("Date") { }

        void display()
        {
            std::cout << openPrice << ", "
                      << high << ", "
                      << low << ", "
                      << close << ", "
                      << adjClose << ", "
                      << volume << ", "
                      << divAmount << ", "
                      << splitCo << std::endl;
        }
    };

    struct EodAdj
    {
        float openPrice;
        float high;      
        float low;       
        float close;     
        float adjClose;  
        float volume;    
        float divAmount; 
        float splitCo;   

        TimeStamp date;

        /* METHODS */

        backtest::Bar generate_bar()
            { return backtest::Bar(openPrice, high, low, close, volume); }

        EodAdjFields* get_fields()
        {
            EodAdjFields* newFields = new EodAdjFields; 
            return newFields;
        }

        //bool is_valid_field(std::string field);

        float greater_than(const EodAdj& rhs, std::string field)
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

        float less_than(const EodAdj& rhs, std::string field)
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

        void display()
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
    };

    /**
     * @brief Deserializes a collection of EodAdj objects from a given file stream.
     * 
     * The EodAdj objects are built in this method, so before program termination, clean_list() must
     * be called.
     * 
     * @param data 
     * @param file 
     */
    template<typename T>
    void deserialize(std::vector<EodAdj*>& data, T& file);
}