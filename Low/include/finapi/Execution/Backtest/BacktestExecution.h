/**
 * @file BacktestExecution.h
 *       Based of classes from: https://www.quantstart.com/articles/Event-Driven-Backtesting-with-Python-Part-VI/
 * 
 * @brief 
 * 
 * @author  Josh Ortner
 * @date    2012-1-14
 * @version 0.1
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#pragma once

#include "../ExecutionHandler.h"

namespace finapi
{
namespace execution
{
namespace backtest
{
    /**
     * @brief Variables required to run a backtest
     * 
     */
    struct BacktestVars
    {
        SymbolList all_symbols;
        std::pair<int, int> date_range;
        float starting_capital;
        models::TimeStamp start_date;

        /* CONSTRUCTOR */

        BacktestVars(const BacktestVars&);

        /* OPERATOR OVERLOADS */

        BacktestVars& operator = (const BacktestVars&);

    };


    /**
     * @brief Execution system for back testing
     * 
     */
    class BacktestExecution : public ExecutionHandler
    {

    public:

        /* CONSTRUCTORS */

        BacktestExecution() { }

        BacktestExecution(DataHandler&, Strategy&, Portfolio&, FillHandler&, std::queue<Event*>&);

        /* METHODS */

        bool run();

    };
    
}
}
}