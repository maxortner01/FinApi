/**
 * @file FinDataHandler.h
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

#include "../ExSystemStructs.h"
#include "../Event.h"
#include "../DataHandler.h"

namespace finapi
{
namespace execution
{
namespace backtest
{
    typedef Cloud::ServerStream _Stream;

    /**
     * @brief Designed to read files for each requested symbol from 
     *        server database and provide an interface to obtain the
     *        "latest" bar similar to a live trading interface
     * 
     *        Parameters:
     *          - event_q            : the event queue
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
    template<typename _DataModel>
    class FinApiHandler : public DataHandler
    {
        std::string _data_dir;
        EVENT_QUEUE_PTR event_q;
        bool _continue_backtest;
        DataModelType model_id;
        std::pair<int, int> year_range;
        SymbolList all_symbols;
        unsigned int _working_year; 
        
        std::unordered_map<std::string, modeler::FinDataFrame<_DataModel, _Stream>* > symbol_data;
        std::unordered_map<std::string, _DataModel*> latest_symbol_data;

        void get_working_year_data();

        bool get_next_model(_DataModel*&, std::string);

        bool in_symbol_data(std::string);

        bool in_latest_data(std::string);

        modeler::FinDataFrame<_DataModel, _Stream>* download_symbol_data(std::string);

    public:

        /* CONSTRUCTORS */

        FinApiHandler() { }

        FinApiHandler(std::string, EVENT_QUEUE_PTR, SymbolList, int, int);
 
        /* DESTRUCTOR */

        ~FinApiHandler();

        /* METHODS */

        bool get_latest_symbol_data(_DataModel*&, std::string);

        void update_latest_data();

        void display_latest_data();

        bool continue_backtest();
    };   

}
}
}