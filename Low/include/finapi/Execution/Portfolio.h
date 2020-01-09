/**
 * @file Portfolio.h
 * 
 * @brief The role of the Portfolio class is to keep track of all current market positions
 *        as well as the market value of the positions (known as "holdings"). This is an
 *        estimation of the liquidation value of the position and is derived from the data
 *        handling facility of the system.
 * 
 *        In addition to the positions and holdings management the portfolio must be aware
 *        of risk factors and position sizing techniques in order to optimise orders that
 *        are sent to a brokerage or other form of market access.
 * 
 *        Portfolio must be able to handle a SignalEvent, generate OrderEvents, and interpret
 *        FillEvents to update positions.
 * 
 * @author  Josh Ortner
 * @date    2012-1-7
 * @version 0.1
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#pragma once

#include "DataHandler.h"

namespace finapi
{
namespace execution
{

    /**
     * @brief Handles the positions and market value of all instruments
     *        at a resolution of a "bar": secondly, minutely, ... EOD.
     * 
     */
    class Portfolio
    {

    public:

        /**
         * @brief Acts on a SignalEvent to generate new orders
         *        based on the portfolio logic.
         * 
         */
        virtual void update_signal(Event*) = 0;

        /**
         * @brief Updates the portfolio current positions and 
         *        holdings from a FillEvent
         * 
         */
        virtual void update_fill(Event*) = 0;
    };

    /**
     * @brief Designed to send orders to a brokerage object with
     *        a constant quantity size blindly. i.e. without risk
     *        management or position sizing. Used to test strategies.
     * 
     *        Parameters:
     *          - bars            - The DataHandler object with current market data.
     *          - events          - The Event Queue object.
     *          - start_date      - The start date (bar) of the portfolio.
     *          - initial_capital - The starting capital in USD.
     * 
     *        Members:
     *          - _all_positions - contains each time stamps positions for all symbols
     *          - _current_positions - most up to date map of symbol positions
     *          - _all_holdings - historical list of all symbol holdings
     *          - _current_holdings - most up to date map of symbol holdings values
     * 
     *        "Holdings" describes the market value of the positions held. "Current market value"
     *        in this instance means the closing price obtained from the current market bar
     * 
     */
    template<typename _DataHandler>
    class NaivePortfolio : public Portfolio
    {

        _DataHandler*     _data;
        EVENT_QUEUE_PTR   _events;
        SymbolList        _all_symbols;
        models::TimeStamp _start_date;
        float             _initial_capital;

        std::vector<std::unordered_map<std::string, float>* > _all_positions;
        std::vector<std::unordered_map<std::string, float>* >::iterator _current_positions;

        std::vector<std::unordered_map<std::string, float>* > _all_holdings;
        std::vector<std::unordered_map<std::string, float>* >::iterator _current_holdings;

        /* PRIVATE METHODS */

        void construct_all_positions();

        void construct_all_holdings();

    public:

        /* CONSTRUCTOR */

        NaivePortfolio(_DataHandler&, EVENT_QUEUE_PTR, SymbolList, models::TimeStamp, float);

        /* DESTRUCTOR */

        ~NaivePortfolio();

        /* METHODS */

        void update_signal(Event*);
        
        void update_fill(Event*);
    };

}
}