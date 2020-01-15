/**
 * @file ExecutionHandler.h
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

#include "DataHandler.h"
#include "Strategy.h"
#include "Portfolio.h"
#include "FillHandler.h"

namespace finapi
{
namespace execution
{
    /**
     * @brief 
     * 
     */
    class ExecutionHandler
    {

    protected:

        EVENT_QUEUE_PTR _events;
        DataHandler*    _data_handler;
        Strategy*       _strategy;
        Portfolio*      _portfolio;
        FillHandler*    _fill_handler;

    public:

        /* CONSTRUCTORS */

        ExecutionHandler();

        ExecutionHandler(DataHandler&, Strategy&, Portfolio&, FillHandler&, std::queue<Event*>&);

        /* METHODS */

        void clear_queue();

        bool init_components(DataHandler&, Strategy&, Portfolio&, FillHandler&, std::queue<Event*>&);

        virtual bool run() = 0;

        void pop_queue();

    };
    

}
}