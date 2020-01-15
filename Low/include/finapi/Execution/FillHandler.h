/**
 * @file FillHandler.h
 *       Based of classes from: https://www.quantstart.com/articles/Event-Driven-Backtesting-with-Python-Part-VI/
 * 
 * @brief 
 * 
 * @author  Josh Ortner
 * @date    2012-1-11
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
     * @brief Handles the interaction between a set of order objects generated
     *        by a Portfolio and the ultimate set of Fill objects that occur
     *        in the market.
     * 
     *        The handlers can be used to subclass simulated brokerages or live
     *        brokerages, with identical interfaces. This allows strategies to be 
     *        backtested in a similar manner to live trading engine.
     * 
     */
    class FillHandler
    {

    protected:

        EVENT_QUEUE_PTR _events;

    public:

        /* CONSTRUCTOR */

        FillHandler(std::queue<Event*>&);

        /* METHODS */

        virtual void fill_order(Event*) = 0;

        void generate_fill(std::string, std::string, std::string, std::string, BrokerType, unsigned int, float);

    };

    /**
     * @brief 
     * 
     */
    class SimulatedFillHandler : public FillHandler
    {

    public:

        /* CONSTRUCTOR */

        SimulatedFillHandler(std::queue<Event*>&);

        /* METHODS */

        void fill_order(Event*);

    };

}
}