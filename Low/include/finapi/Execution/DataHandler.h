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

#include "./ExSystemStructs.h"
#include "Event.h"

namespace finapi
{
namespace execution
{

    std::string get_model_connection(DataModelType model);

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

        SymbolList() { }

        SymbolList(std::ifstream&);

        SymbolList(const SymbolList&);

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
         * Update the last N bars from the lastest_symbol list,
         * or fewer if less bars are available 
         */
        //virtual void get_latest_bars(std::string sym, int N=1) = 0;

        /* 
         * Pushes the latest bar to the latest symbol structure for all
         * symbols in the symbol list.
         */
        virtual void update_latest_data() = 0;

    };
}
}