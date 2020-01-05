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
    template<typename DataModel>
    class FinApiHandler : public DataHandler
    {
        std::queue<Event*>* event_q;
        bool _continue_backtest;
        DataModelType data_id;
        std::pair<int, int> year_range;
        std::string data_directory;
        SymbolList all_symbols;
        unsigned int _working_year; 
        

        std::unordered_map<std::string, FinDataFrame<DataModel, std::ifstream>* > symbol_data;
        std::unordered_map<std::string, DataModel*> latest_symbol_data;

        void get_working_year_data();

        bool get_next_model(DataModel*&, std::string);

        bool get_connection(std::string, std::ifstream&);

        FinDataFrame<DataModel, std::ifstream>* get_symbol_data(std::string);

    public:

        /* CONSTRUCTORS */

        FinApiHandler() { }

        FinApiHandler(int, int, std::string, SymbolList, std::queue<Event*>&);
 
        /* DESTRUCTOR */

        ~FinApiHandler();

        /* METHODS */

        bool get_latest_model(DataModel*&, std::string);

        void update_models();

        void display_latest_data();

        bool continue_backtest();
    };    

}
}
}