/**
 * @file Strategy.h
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
#include "../Core/Models.h"

#include "DataHandler.h"
#include "Backtest/FinApiHandler.h"

namespace finapi
{
namespace execution
{

    /**
     * @brief An abstract base class providing an interface for all subsequent
     *        strategy handling objects.
     * 
     *        The goal of a derived Strategy object is to generate Signal objects
     *        for particular symbols based on the input of Bars (OLHCVI) generated
     *        by a DataHandler object.
     * 
     *        This is designed to work both with historic and live data as the 
     *        Strategy object is agnostic to the data source, since it obtains the
     *        bar from a queue object.
     * 
     */
    class Strategy
    {

    public:

        /* CONSTRUCTORS */

        Strategy() { } 

        /**
         * @brief Provides the mechanisms to calculate the list of signals
         * 
         */
        virtual void calculate_signals(Event*) = 0;

    };

    /**
     * @brief Extremely simple test Strategy. Goes LONG on all of the symbols
     *        as soon as a bar is received. It will never exit a position.
     * 
     *        Should be used primarily as a testing mechanism for the Strategy
     *        class as well as a benchmark upon with to compare other strategies.
     *  
     *        Parameters:
     *          - bars   : The DataHandler object that provides bar information for specified ticker
     *          - events : The Event Queue object.
     * 
     *        Memebers:
     *          - bought : unordered map of symbols bought: true, false
     * 
     */
    class BuyAndHold : public Strategy
    {
        
        SymbolList             all_symbols;
        std::queue<Event*>*    events;
        backtest::FinApiHandler<models::EodAdj>* eod_data;

        std::unordered_map<std::string, bool> bought;

        /* PRIVATE METHODS */

        void calculate_initial_bought();

    public:

        /* CONSTRUCTORS */

        BuyAndHold(SymbolList&, std::queue<Event*>&, backtest::FinApiHandler<models::EodAdj>&);

        /* METHODS */

        void calculate_signals(Event*);

    };


}
}