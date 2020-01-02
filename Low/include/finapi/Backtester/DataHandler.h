/**
 * @file DataHandler.h
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

#include <fstream>
#include <queue>
#include <map>
#include <string>
#include <vector>
#include <utility>
#include "./ExSystemStructs.h"

namespace finapi
{
namespace backtest
{
    /**
     * @brief Encapsulates a file of symbols with an interface to easily
     *        access the symbols
     * 
     */
    class SymbolList
    {

        std::vector<std::string> symbols;
        std::vector<std::string>::iterator it;

    public:

        /* CONSTRUCTORS */

        SymbolList(std::ifstream&);

        /* METHODS */

        bool get_next_symbol(std::string&);        

    };

    /**
     * @brief Abstract base class providing an interface for all subsequent
     *        data handlers (live and historic).
     * 
     *        The goal of a derived DataHandler object is to output a generated
     *        set of bars (OLHCVI) for each symbol requested.
     * 
     */
    class DataHandler
    {
    public:

        /* 
         * Returns the last N bars from the lastest_symbol list,
         * or fewer if less bars are available 
         */
        virtual std::vector<Bar> get_latest_bars(std::string sym, int N=1) = 0;

        /* 
         * Pushes the latest bar to the latest symbol structure for all
         * symbols in the symbol list.
         */
        virtual void update_bars() = 0;

    };

    /**
     * @brief Designed to read files for each requested symbol from 
     *        server database and provide an interface to obtain the
     *        "latest" bar similar to a live trading interface
     * 
     *        Parameters:
     *          - event_q            : the event queue
     *          - 
     *          - data_directory     : directory where all symbol data is located
     *          - symbol_file        : file containing list of desired symbols
     * 
     *        Data Members:
     *          - continue_backtest  : bool - execution control
     *          - symbol_data        : a symbol mapped to its entire loaded data file
     *          - latest_symbol_data : a symbol mapped to its most recent bar
     *          - working_year       : current year of data in symbol_data
     * 
     */
    template<typename PriceStruct>
    class SecuritiesMasterDataHandler : public DataHandler
    {
        bool continue_backtest;
        std::pair<int, int> year_range;
        std::string data_directory;
        SymbolList all_symbols;
        unsigned int working_year; 
        std::queue<Event*> event_q;

        std::map<std::string, std::vector<PriceStruct*> > symbol_data; // vector should be created data structure
        std::map<std::string, Bar> latest_symbol_data;

        void get_working_year_data();

    public:

        /* CONSTRUCTORS */

        SecuritiesMasterDataHandler(int start_year, int end_year, std::string d_dir, std::ifstream& s_file) :
            DataHandler(), continue_backtest(true), year_range(start_year, end_year), 
            data_directory(d_dir), all_symbols(s_file), working_year(start_year) 
        { get_working_year_data(); }

        /* METHODS */




    };

    /* METHODS */

    /* PRIVATE */

    template<typename PriceStruct>
    void SecuritiesMasterDataHandler<PriceStruct>::get_working_year_data()
    {

    }

}
}