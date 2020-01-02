/**
 * @file Event.h
 * 
 * @brief Event class for backtesting library. Determines how an event will be
 *        handeled in the main event-loop of backtester.
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

namespace finapi
{

namespace backtest
{

    /* Names of possible Brokers */

    enum BrokerType { ALPACA }; 

    /**
     * @brief Base class providing an interface for all inherited events
     *        that will trigger further events in the trading infrastructure.
     * 
     */
    class Event
    { 
        /* As of right now there will be no specific implementation or interface for base */
    };

    /**
     * @brief Handles the event of receiving a new market update with corresponding bars
     * 
     */
    class MarketEvent : public Event
    {

        std::string event_type;

    public:

        /* CONSTRUCTORS */

        MarketEvent() :
            Event(), event_type("MARKET") { }

    };

    /**
     * @brief Handles the event of sending a Signal from a Strategy object.
     *        This is received by a Portfolio object and acted upon.
     * 
     *        Parameters:
     *        - symbol      : ticker
     *        - date_time   : signal generation timestamp
     *        - signal_type : "LONG" or "SHORT"     
     * 
     */
    class SignalEvent : public Event
    {

        std::string event_type;
        std::string symbol;
        TimeStamp   date_time;
        std::string signal_type;
        

    public:

        /* CONSTRUCTORS */

        SignalEvent(std::string sym, TimeStamp d_time, std::string sig_type) :
            Event(), event_type("SIGNAL"), symbol(sym), date_time(d_time), signal_type(sig_type)
        { }

    };

    /**
     * @brief Handles the event of sending an Order to an execution system.
     *        The order contains a symbol (i.e. MSFT), a type (market or limit),
     *        quantity and a direction.
     * 
     *        Parameters:
     *        - symbol     : ticker
     *        - order_type : "MKT" or "LMT" for Market Order or Limit Order
     *        - quantity   : Non-negative integer for quantity
     *        - direction  : "BUY" or "SELL" for long or short
     * 
     */
    class OrderEvent : public Event
    {

        std::string  event_type;
        std::string  symbol;
        std::string  order_type;
        std::string  direction;
        unsigned int quantity;

    public:

        /* CONSTRUCTORS */

        OrderEvent(std::string sym, std::string ord_type, std::string dirx, unsigned int quant) :
            Event(), event_type("ORDER"), symbol(sym), order_type(ord_type), direction(dirx), quantity(quant)
        { }

        /* METHODS */

        void printOrder();

    };
   
    /**
     * @brief Encapsulates the notion of a Filled Order, as returned from a 
     *        brokerage. Stores the quantity of an instrument actually filled
     *        and at what price. In addition, stores the commission of the trade.
     * 
     *        Parameters:
     *        - time_index  : Bar-resolution when the order was filled
     *        - symbol      : Instrument which was filled
     *        - exchange    : Exchange where the order was filled
     *        - direction   : Direction of fill ("BUY" or "SELL")
     *        - broker_id   : Used to define the commission calculation protocol
     *        - quantity    : Filled quantity
     *        - fill_cost   : Holdings value in dollars
     * 
     */
    class FillEvent : public Event
    {

        std::string  event_type; 
        std::string  time_index;
        std::string  symbol;
        std::string  exchange;
        std::string  direction;
        BrokerType   broker;
        unsigned int quantity;
        float        fill_cost;

    public:

        /* CONSTRUCTORS */

        FillEvent(std::string t_index, std::string sym, std::string exch, std::string dir,
                  BrokerType t_broker, unsigned int quant, float f_cost) :
            Event(), event_type("FILL"), time_index(t_index), symbol(sym), exchange(exch), 
            direction(dir), broker(t_broker), quantity(quant), fill_cost(f_cost)
        { }

        /* METHODS */

        double calculate_final_cost();

    };
}
}