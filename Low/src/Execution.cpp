#include "finapi/finapi.h"

/* Final Calculations for Specific Broker */

/**
 * @brief Calculate the final cost of an order using define commission structure
 * 
 * @param quantity   : order quantity
 * @param fill_cost  : holdings value of single security in dollars
 * @param final_cost : reference to return the final calulated cost
 */
void calculate_alpaca_commission(unsigned int quantity, double fill_cost, double& commission)
{
    commission = 0.0;
}

namespace finapi
{

    typedef Cloud::ServerStream _Stream;
    

    /*        Event.h         */ 
namespace execution
{
    /* Protocols for commission calculation */

    void calculate_final_broker_commission(BrokerType broker, unsigned int quantity, double fill_cost, double& final_commission)
    {
        switch(broker)
        {
            case ALPACA:
                calculate_alpaca_commission(quantity, fill_cost, final_commission);
                break;
        }
    }

    /* Get directory containing symbols for specified DataModel */

    std::string get_symbol_directory(DataModelType model_id)
    {
        switch (model_id)
        {
            case EODADJ:
                return "historical-prices";
        }
    }

    /* Get directory containing binary files for specified DataModel */

    std::string get_binary_directory(DataModelType model_id)
    {
        switch (model_id)
        {
            case EODADJ:
                return "eod-adj";
        }
    }

    /* Build connection string to access binary files from server */
    //"/home/josh/Desktop/cpp/stock-platform/data/" + 
    std::string get_binary_file_dir(DataModelType model_id, std::string symbol, std::string fileName)
    {
        switch (model_id)
        {
            case EODADJ:
                return std::string(get_symbol_directory(model_id) + "/" + symbol + "/" + get_binary_directory(model_id) + "/" + fileName + ".bin");
        }
    }

#pragma region EVENT_H

    /**
     * EVENT IMPLEMENTATION
     */ 

    /* METHODS */

    /* CONSTRUCTORS */
        
    Event::Event(EventType e_type) :
        event_type(e_type) 
    { }
        
    /* METHODS */

    EventType Event::type()
        { return event_type; }


    /**
     * MARKET_EVENT IMPLEMENTATION
     */ 

    /* CONSTRUCTORS */

    MarketEvent::MarketEvent() :
            Event(MARKET) { }

    /**
     * SIGNAL_EVENT IMPLEMENTATION
     */   

    /* CONSTRUCTORS */

    /**
     * @brief Construct a new Signal Event:: Signal Event object
     * 
     * @param sym 
     * @param d_time 
     * @param sig_type 
     */
    SignalEvent::SignalEvent(std::string sym, models::TimeStamp d_time, SignalType sig_type) :
        Event(SIGNAL), _symbol(sym), _date_time(d_time), _signal_type(sig_type)
    { }

    /* METHODS */

    /**
     * @brief Output the values within the Order
     * 
     */
    void SignalEvent::print_event()
    {
        std::cout << "Signal: Symbol=" << _symbol
                  << ", Year=" << _date_time.year
                  << ", Month=" << _date_time.month
                  << ", Day="<< _date_time.day << std::endl;
    }

    /**
     * ORDER_EVENT IMPLEMENTATION
     */  

    /* CONSTRUCTORS */

    /**
     * @brief Construct a new Order Event:: Order Event object
     * 
     * @param sym 
     * @param ord_type 
     * @param dirx 
     * @param quant 
     */
    OrderEvent::OrderEvent(std::string sym, std::string ord_type, std::string dirx, unsigned int quant) :
        Event(ORDER), _symbol(sym), _order_type(ord_type), _direction(dirx), _quantity(quant)
    { }

    /* METHODS */

    /**
     * @brief Output the values within the Order
     * 
     */
    void OrderEvent::print_event()
    {
        std::cout << "Order: Symbol=" << _symbol
                  << ", Type=" << _order_type
                  << ", Quantity=" << _quantity
                  << ", Direction="<< _direction << std::endl;
    }

    /**
     * FILL_EVENT IMPLEMENTATION
     */ 

    /* CONSTRUCTORS */

    FillEvent::FillEvent(std::string t_index, std::string sym, std::string exch, std::string dir,
                         BrokerType t_broker, unsigned int quant, float f_cost) :
        Event(FILL), _time_index(t_index), _symbol(sym), _exchange(exch), 
        _direction(dir), _broker(t_broker), _quantity(quant), _fill_cost(f_cost)
    { }

    /* METHODS */

    double FillEvent::commission()
    {
        double final_commission;
        calculate_final_broker_commission(_broker, _quantity, _fill_cost, final_commission);
    }

#pragma endregion

#pragma region DATA_HANDLER_H

    /**
     * SYMBOL_LIST IMPLEMENTATION
     */ 

    /* CONSTRUCTORS */

    /**
     * @brief Construct a new Symbol list by filling the symbol vector with 
     *        the symbols in file. NOTE: Must be one symbol per line
     * 
     * @param inFile 
     */
    SymbolList::SymbolList(std::ifstream& inFile)
    {
        std::string sym;

        while(getline(inFile, sym))
            symbols.push_back(sym);

        symbols.shrink_to_fit();

        it = symbols.begin();
        inFile.close();
    }

    /**
     * @brief Construct a new Symbol List:: Symbol List object
     * 
     * @param obj 
     */
    SymbolList::SymbolList(const SymbolList& obj) :
        symbols(obj.symbols)
    { it = symbols.begin(); }

    /* METHODS */

    /**
     * @brief Function to iterate through all symbols
     * 
     * @param sym 
     * @return true 
     * @return false 
     */
    bool SymbolList::get_next_symbol(std::string& sym)
    {
        if (it == symbols.end())
        {
            it = symbols.begin();
            return false;
        }
        sym = *it;
        it++; 
        return true;
    }

    /* OPERATOR OVERLOADS*/

    /**
     * @brief Copy assignment
     * 
     * @param rhs 
     * @return true 
     * @return false 
     */
    SymbolList& SymbolList::operator = (const SymbolList& rhs)
    {
        if (this != &rhs) 
            symbols = rhs.symbols; 
        return (*this);
    }

#pragma endregion

#pragma region STRATEGY_H

    /**
     * STRATEGY IMPLEMENTATION
     */ 

    /* CONSTRUCTORS */

    Strategy::Strategy(SymbolList& s_list, std::queue<Event*>& events, DataHandler& eod_d) :
        all_symbols(s_list), event_q(&events), eod_data(&eod_d)
    { initialize_bought(); }

    /* PRIVATE METHODS */

    /**
     * @brief Adds keys to bought unordered map for
     *        all symbols and sets them to false
     * 
     */
    void Strategy::initialize_bought()
    {
        std::string symbol;

        while (all_symbols.get_next_symbol(symbol))
        {
            std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
            bought[symbol] = false;
        }
    }

    /* METHODS */

    /**
     * @brief Display all symbol holdings
     * 
     * @tparam _DataHandler 
     * @tparam _DataModel 
     */
    void Strategy::display_holdings()
    {
        for (std::pair<std::string, bool> symbol : bought)
        {
            std::cout << symbol.first << ":\t";
            if (symbol.second)
                std::cout << "TRUE" << std::endl;
            else
                std::cout << "FALSE" << std::endl;
        } 
    }

    /**
     * @brief Generates a signal event and pushes it to the event queue
     * 
     * @param sym 
     * @param d_time 
     * @param sig_type 
     */
    void Strategy::generate_signal(std::string sym, models::TimeStamp d_time, SignalType sig_type)
    {
        SignalEvent* newSignal = new SignalEvent(sym, d_time, sig_type);
        event_q->push(newSignal);
        bought[sym] = true;
    }

    /* CONSTRUCTORS */

    BuyAndHold::BuyAndHold(SymbolList& s_list, std::queue<Event*>& events, DataHandler& eod_d) :
        Strategy(s_list, events, eod_d)
    { }

    /**
     * @brief For "Buy and Hold" we generate a single signal per symbol
     *        and then no additional signals. This means we are 
     *        constantly long the market from the date of strategy
     *        initialisation.
     *
     *        Parameters
     *          - event - A MarketEvent object.
     * 
     * @param event 
     */
    void BuyAndHold::calculate_signals(Event* event)
    {
        std::string symbol;

        if (event->type() == MARKET)
        {
            // iterate through all symbols, retrieving latest available data
            while (all_symbols.get_next_symbol(symbol))
            {
                std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
                // get latest symbol data and if available buy symbol
                if (eod_data->get_latest_symbol_data(data_model, symbol))
                {
                    if (!bought[symbol])
                    {
                        this->generate_signal(symbol, data_model.time(), LONG);
                    }
                }
            }    
        }        
    }

    

#pragma endregion

#pragma region PORTFOLIO_H

    /* CONSTRUCTORS */

    Portfolio::Portfolio(DataHandler& d, std::queue<Event*>& e_ptr, SymbolList all_syms, models::TimeStamp s_date, float i_cap) :
        _data(&d), _events(&e_ptr), _all_symbols(all_syms), _start_date(s_date), _initial_capital(i_cap)
    { 
        initialize_all_positions();
        initialize_all_holdings();
        //_current_positions = _all_positions.begin();
        //_current_holdings  = _all_holdings.begin(); 
    }

    /* DESTRUCTOR */

    //Portfolio::~Portfolio()
    //{
    //    for (int i = 0; i < _all_positions.size(); i++)
    //        delete _all_positions[i];
    //    for (int i = 0; i < _all_holdings.size(); i++)
    //        delete _all_holdings[i];
    //}

    /* PRIVATE METHODS */

    /**
     * @brief Initializes the positions data
     *        _all_positions stores a list of all previous positions recorded
     *        at the timestamp of a market data event. Position = quantity of asset
     * 
     * @tparam _DataHandler 
     */
    void Portfolio::initialize_all_positions()
    {
        std::string symbol;
        //std::unordered_map<std::string, float>* new_positions;
        //new_positions = new std::unordered_map<std::string, float>();

        _asset_positions.add_new_holdings(false);

        // add keys for all symbols to map and set value to 0
        while (_all_symbols.get_next_symbol(symbol))
        {
            std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
            //(*new_positions)[symbol] = 0.0;
            _asset_positions.add_field(symbol, 0.0);
        }

        // set start date
        _asset_positions.add_field("year", _start_date.year);
        _asset_positions.add_field("month", _start_date.month);
        _asset_positions.add_field("day", _start_date.day);
        //(*new_positions)["year"]  = _start_date.year;
        //(*new_positions)["month"] = _start_date.month;
        //(*new_positions)["day"]   = _start_date.day;

        // add map to positions vector
        //_all_positions.push_back(new_positions);
        //_all_positions.shrink_to_fit();
    }

    /**
     * @brief Initializes the holdings data
     *        _all_holdings stores historical list of all symbol holdings.
     *        Holding = market value of positions held at time stamp
     * 
     * @tparam _DataHandler 
     */
    void Portfolio::initialize_all_holdings()
    {
        std::string symbol;
        //std::unordered_map<std::string, float>* new_holdings;
        //new_holdings = new std::unordered_map<std::string, float>();

        _asset_holdings.add_new_holdings(false);

        while (_all_symbols.get_next_symbol(symbol))
        {
            std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
            //(*new_holdings)[symbol] = 0.0;
            _asset_holdings.add_field(symbol, 0.0);
        }

        _asset_holdings.add_field("cash", _initial_capital);
        _asset_holdings.add_field("comm", 0.0);
        _asset_holdings.add_field("total", _initial_capital);
        _asset_holdings.add_field("year", _start_date.year);
        _asset_holdings.add_field("month", _start_date.month);
        _asset_holdings.add_field("day", _start_date.day);

        //(*new_holdings)["cash"]  = _initial_capital;
        //(*new_holdings)["comm"]  = 0.0;
        //(*new_holdings)["total"] = _initial_capital;
        //(*new_holdings)["year"]  = _start_date.year;
        //(*new_holdings)["month"] = _start_date.month;
        //(*new_holdings)["day"]   = _start_date.day;

        //_all_holdings.push_back(new_holdings);
        //_all_holdings.shrink_to_fit();
    }

    /**
     * @brief Updates positions and holdings when given a MarketEvent
     * 
     * @tparam _DataHandler 
     * @param event 
     */
    void Portfolio::update_timeindex(Event* event)
    {
        float market_value, curr_position_value, new_total;
        models::TimeStamp current_time;
        models::Bar       current_bar;
        // update POSITIONS
        std::string symbol;
        //std::unordered_map<std::string, float>* new_positions = nullptr;
        //new_positions = new std::unordered_map<std::string, float>();

        // add new holdings and positions
        _asset_positions.add_new_holdings(true);
        _asset_holdings.add_new_holdings(true);

        // add keys for all symbols to map and set value to previous value
        //while (_all_symbols.get_next_symbol(symbol))
        //{
        //    std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
        //    (*new_positions)[symbol] = (*(*_current_positions))[symbol];
        //}

        _data->get_latest_data_timestamp(current_time);
        
        // set new date
        _asset_positions.add_field("year", current_time.year);
        _asset_positions.add_field("month", current_time.month);
        _asset_positions.add_field("day", current_time.day);
        //(*new_positions)["year"]  = current_time.year;
        //(*new_positions)["month"] = current_time.month;
        //(*new_positions)["day"]   = current_time.day;
        
        // add map to positions vector
        //_all_positions.push_back(new_positions);
        //_all_positions.shrink_to_fit();

        //_current_positions = _all_positions.end() - 1;  

        // update HOLDINGS
        //std::unordered_map<std::string, float>* new_holdings = nullptr;
        //new_holdings = new std::unordered_map<std::string, float>();

        //(*new_holdings)["cash"]  = (*(*_current_holdings))["cash"];
        //(*new_holdings)["comm"]  = (*(*_current_holdings))["comm"];
        //(*new_holdings)["total"] = (*(*_current_holdings))["cash"];
        //(*new_holdings)["year"]  = current_time.year;
        //(*new_holdings)["month"] = current_time.month;
        //(*new_holdings)["day"]   = current_time.day;
        _asset_holdings.add_field("total", _asset_holdings.get_value("cash"));
        _asset_holdings.add_field("year", current_time.year);
        _asset_holdings.add_field("month", current_time.month);
        _asset_holdings.add_field("day", current_time.day);

        while (_all_symbols.get_next_symbol(symbol))
        {
            // approximation to the real value
            std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
            if (_data->get_latest_symbol_data(current_bar, symbol))
            {
                //market_value             = ((*(*_current_positions))[symbol]) * current_bar.close();
                market_value = _asset_positions.get_value(symbol) * current_bar.close();
                _asset_holdings.add_field(symbol, market_value);
                _asset_holdings.get_value("total") += market_value;
                //if (_asset_positions.get_value(symbol, curr_position_value))
                //{
                //    market_value = curr_position_value * current_bar.close();
                //    _asset_holdings.add_field(symbol, market_value);
                //    _asset_holdings.get_value("total", new_total);
                //    new_total += market_value;
                //    _asset_holdings.add_field("total", new_total);
                //}
                //(*new_holdings)[symbol]  = market_value;
                //(*new_holdings)["total"] += market_value;
            }
        }

        //std::cout << "HOLDINGS\n";
        //display_current_holdings();
        //std::cout << "POSITIONS\n";
        //display_current_positions();

        //_all_holdings.push_back(new_holdings);
        //_all_holdings.shrink_to_fit();

        //_current_holdings = _all_holdings.end() - 1;
    }

    void Portfolio::generate_order(std::string sym, std::string ord_type, std::string dirx, unsigned int quant)
    {
        OrderEvent* order = new OrderEvent(sym, ord_type, dirx, quant);
        _events->push(order);
    }

    modeler::EquityCurve Portfolio::create_equity_curve_dataframe()
        { return modeler::EquityCurve(_asset_holdings); }


    /**
     * @brief Display the current holdings data
     * 
     */
    void Portfolio::display_current_positions()
        { _asset_positions.display_current_holdings(); }

    /**
     * @brief Display the current position data
     * 
     */
    void Portfolio::display_current_holdings()
        { _asset_holdings.display_current_holdings(); }
    

    /* CONSTRUCTORS */

    /**
     * @brief Construct a new NaivePortfolio::NaivePortfolio object
     * 
     * @tparam _DataHandler 
     * @param d 
     * @param e_ptr 
     * @param all_syms 
     * @param s_date 
     * @param i_cap 
     */
    NaivePortfolio::NaivePortfolio(DataHandler& d, std::queue<Event*>& e_ptr, SymbolList all_syms, models::TimeStamp s_date, float i_cap) :
        Portfolio(d, e_ptr, all_syms, s_date, i_cap)
    { }

    /* PRIVATE METHODS */

    /**
     * @brief Takes a FillEvent object and updates the position to reflect
     *        the new portfolio
     * 
     * @tparam _DataHandler 
     * @param fill_event 
     */
    void NaivePortfolio::update_positions_from_fill(Event* event)
    {
        float fill_dir = 0;
        FillEvent* fill_event = dynamic_cast<FillEvent*>(event);
        float prev_value;

        if (fill_event->direction() == "BUY")
            fill_dir = 1.0;
        else
            fill_dir = -1.0;

        //prev_value = 
        //prev_value += fill_dir * fill_event->quantity(); 
        //_asset_positions.add_field(fill_event->symbol(), prev_value);
        _asset_positions.get_value(fill_event->symbol()) += fill_dir * fill_event->quantity(); 

        //(*(*_current_positions))[fill_event->symbol()] += fill_dir * fill_event->quantity();     
    }

    /**
     * @brief 
     * 
     * @tparam _DataHandler 
     * @param fill_event 
     */
    void NaivePortfolio::update_holdings_from_fill(Event* event)
    {
        FillEvent* fill_event = dynamic_cast<FillEvent*>(event);
        float fill_dir = 0;
        float fill_cost;
        float final_cost;
        models::Bar current_bar;

        if (_data->get_latest_symbol_data(current_bar, fill_event->symbol()))
        {
            if (fill_event->direction() == "BUY")
                fill_dir = 1.0;
            else
                fill_dir = -1.0;

                // close price
            fill_cost = current_bar.close();
            final_cost = fill_dir * fill_cost * fill_event->quantity();
            _asset_holdings.get_value(fill_event->symbol()) += final_cost;
            _asset_holdings.get_value("comm") += fill_event->commission();
            _asset_holdings.get_value("cash") -= (final_cost + fill_event->commission());
            _asset_holdings.get_value("total") -= (final_cost + fill_event->commission());
            //(*(*_current_positions))[fill_event->symbol()] += final_cost;
            //(*(*_current_positions))["comm"] += fill_event->commission();
            //(*(*_current_positions))["cash"] -= (final_cost + fill_event->commission());
            //(*(*_current_positions))["total"] -= (final_cost + fill_event->commission());
        }
            
    }

    /* METHODS */

     /**
     * @brief Creates an order event from a signal event
     * 
     * @tparam _DataHandler 
     * @param event 
     */
    void NaivePortfolio::update_signal(Event* event)
    {
        if (event)
            if (event->type() == SIGNAL)
                generate_naive_order(event);
    }

    /**
     * @brief Updates the portfolio current positions and 
     *        holdings from a FillEvent
     * 
     */
    void NaivePortfolio::update_fill(Event* event)
    {
        if (event)
        {
            if (event->type() == FILL)
            {
                update_positions_from_fill(event);
                update_holdings_from_fill(event);
            }
        }
    }    

    /**
     * @brief Transacts an OrderEvent object as a constant quantity sizing of the 
     *        signal object, without risk management or position sizing considereations.
     * 
     * @tparam _DataHandler 
     * @param signal_event 
     * @return Event* 
     */
    void NaivePortfolio::generate_naive_order(Event* signal_event)
    {
        if (signal_event->type() == SIGNAL)
        {
            // cast to signal event
            SignalEvent* fill_event = dynamic_cast<SignalEvent*>(signal_event);
            std::string symbol      = fill_event->symbol();
            SignalType direction    = fill_event->signal_type();

            float market_quantity   = 25.0;
            float current_quantity  = _asset_positions.get_value(symbol);//(*(*_current_positions))[symbol];
            std::string order_type  = "MKT";

            if (direction == LONG && current_quantity == 0)
                generate_order(symbol, order_type, "BUY", market_quantity);
            if (direction == SHORT && current_quantity == 0)
                generate_order(symbol, order_type, "SELL", market_quantity);
            
            if (direction == EXIT && current_quantity > 0)
                generate_order(symbol, order_type, "SELL", abs(current_quantity));
            if (direction == EXIT && current_quantity < 0)
                generate_order(symbol, order_type, "BUY", abs(current_quantity));
        }
    }

#pragma endregion

#pragma region FILL_HANDLER_H


        /* CONSTRUCTOR */

        FillHandler::FillHandler(std::queue<Event*>& e_queue) :
            _events(&e_queue)
        { }

        /* METHODS */

        /**
         * @brief Converts Order event to Fill event and pushes event to the stack
         * 
         * @param event 
         */
        void FillHandler::generate_fill(std::string t_index, std::string sym, std::string exch, std::string dir, BrokerType t_broker, unsigned int quant, float f_cost)
        {
            FillEvent* new_order = new FillEvent(t_index, sym, exch, dir, t_broker, quant, f_cost);
            _events->push(new_order);
        }

        /* CONSTRUCTOR */

        SimulatedFillHandler::SimulatedFillHandler(std::queue<Event*>& e_queue) :
            FillHandler(e_queue)
        { }

        /* METHODS */

        /**
         * @brief Converts Order event to Fill event 
         *        NOTE: Does NOT take into account latency, slippage, fill ratio problems
         * 
         * @param event 
         */
        void SimulatedFillHandler::fill_order(Event* event)
        {
            OrderEvent* order_event = dynamic_cast<OrderEvent*>(event);
            this->generate_fill("NOW", order_event->symbol(), "EXCHANGE", order_event->direction(), ALPACA, order_event->quantity(), 0.0);
        }


#pragma endregion

#pragma region EXECUTION_HANDLER_H

    /* CONSTRUCTORS */

    ExecutionHandler::ExecutionHandler() :
     _data_handler(nullptr), _strategy(nullptr), _portfolio(nullptr), _fill_handler(nullptr)
    { }

    ExecutionHandler::ExecutionHandler(DataHandler& data, Strategy& strat, Portfolio& port, FillHandler& filler, std::queue<Event*>& e_queue) 
        { init_components(data, strat, port, filler, e_queue); }

    /* METHODS */

    /**
     * @brief Clears the event queue
     * 
     */
    void ExecutionHandler::clear_queue()
    {
        if (_events)
        {
            while (_events->size())
            {
                delete _events->front();
                _events->pop();
            }
        }
    }    

    /**
     * @brief Initialize all components required to run
     * 
     * @param data 
     * @param strat 
     * @param port 
     * @param filler 
     * @return true 
     * @return false 
     */
    bool ExecutionHandler::init_components(DataHandler& data, Strategy& strat, Portfolio& port, FillHandler& filler, std::queue<Event*>& e_queue) 
    {
        _data_handler = &data;
        _strategy     = &strat;
        _portfolio    = &port;
        _fill_handler = &filler;
        _events       = &e_queue;
        clear_queue();
    }

    /**
     * @brief Pop event from event queue
     * 
     */
    void ExecutionHandler::pop_queue()
        { _events->pop(); }


#pragma endregion

namespace backtest
{

#pragma region BACK_TEST_HANDLER_H

    /* CONSTRUCTOR */

    /**
     * @brief Copy Constructor
     *        Construct a new Backtest Vars:: Backtest Vars object
     * 
     * @param obj 
     */
    BacktestVars::BacktestVars(const BacktestVars& obj) :
        all_symbols(obj.all_symbols), date_range(obj.date_range), starting_capital(obj.starting_capital), start_date(obj.start_date)
    { }

    /* OPERATOR OVERLOADS */

    /**
     * @brief Copy assignment
     * 
     * @param rhs 
     * @return BacktestVars& 
     */
    BacktestVars& BacktestVars::operator = (const BacktestVars& rhs)
    {
        if (this != &rhs)
        {
            all_symbols      = rhs.all_symbols;
            date_range       = rhs.date_range;
            starting_capital = rhs.starting_capital;
            start_date       = rhs.start_date;
        }
        return (*this);
    }
    
    /* CONSTRUCTORS */

    BacktestExecution::BacktestExecution(DataHandler& data, Strategy& strat, Portfolio& port, FillHandler& filler, std::queue<Event*>& e_queue) :
        ExecutionHandler(data, strat, port, filler, e_queue)
    { }

    /* METHODS */

    /**
     * @brief If variables have 
     * 
     * @return true 
     * @return false 
     */
    bool BacktestExecution::run()
    {
        // return false if components have not been initialized
        if (!_data_handler)
            return false;
        else
        {
            clear_queue();
            _data_handler->update_latest_data();
            _data_handler->display_latest_data();
            // update data until end of date range is reached
            while (true)
            {
                // create market event if events queue is empty
                if (!_events->size())
                {
                    if (!_data_handler->update_latest_data())
                        break;
                    _data_handler->display_latest_data();
                }
                    

                if (_events->front()->type() == MARKET)
                {
                    //std::cout << "MARKET\n";
                    _portfolio->update_timeindex(_events->front());
                    _strategy->calculate_signals(_events->front());
                    pop_queue();
                }
                else if (_events->front()->type() == SIGNAL)
                {
                    while (_events->size() && _events->front()->type() == SIGNAL)
                    {
                        //std::cout << "SIGNAL\n";
                        _portfolio->update_signal(_events->front());
                        pop_queue();
                    }
                }
                else if (_events->front()->type() == ORDER)
                {
                    while (_events->size() && _events->front()->type() == ORDER)
                    {
                        //std::cout << "ORDER\n";
                        _fill_handler->fill_order(_events->front());
                        pop_queue();
                    }
                }
                else if (_events->front()->type() == FILL)
                {
                    while (_events->size() && _events->front()->type() == FILL)
                    {
                        //std::cout << "FILL\n";
                        _portfolio->update_fill(_events->front());
                        pop_queue();
                    }
                }

                
            }
            return true;
        }
    }

    


#pragma endregion

#pragma region FIN_API_HANDLER_H

    /**
     * FIN_API_HANDLER IMPLEMENTATION
     */ 

    template class FinApiHandler<models::EodAdj>;

    /* CONSTRUCTORS */

    /**
     * @brief Construct a new FinApiHandler<_DataModel>::FinApiHandler object
     * 
     * @tparam _DataModel 
     * @param start_year 
     * @param end_year 
     * @param d_dir 
     * @param s_list 
     * @param events 
     */
    template<typename _DataModel>
    FinApiHandler<_DataModel>::FinApiHandler(std::string d_dir, std::queue<Event*>& events, SymbolList s_list, int start_year, int end_year) :
        DataHandler(), event_q(&events), _continue_backtest(true), _data_dir(d_dir), model_id(_DataModel::model_type()), year_range(start_year, end_year), 
        all_symbols(s_list), _working_year(start_year) 
    { get_working_year_data(); }

    /* DESTRUCTOR */

    /**
     * @brief Destroy the Eod Adj Handler:: Eod Adj Handler object
     *        Deallocate dataframes for each symbol in unordered map
     * 
     */
    template<typename _DataModel>
    FinApiHandler<_DataModel>::~FinApiHandler()
    {
        for (std::pair<std::string, modeler::FinDataFrame<_DataModel, _Stream>* > element : symbol_data)
            delete element.second;        
    }

    /* PRIVATE METHODS */

    template<typename _DataModel>
    void FinApiHandler<_DataModel>::get_working_year_data()
    {
        std::string symbol;
        modeler::FinDataFrame<_DataModel, _Stream>* data = nullptr;

        for (std::pair<std::string, modeler::FinDataFrame<_DataModel, _Stream>* > element : symbol_data)
            delete element.second; 

        symbol_data.clear();

        // iterate through all symbols in symbol list
        while (all_symbols.get_next_symbol(symbol))
        {
            // create connection string
            std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
            data = download_symbol_data(symbol);
            // add symbol data to map
            if (data)
                symbol_data[symbol] = data;                                            
        }
    }

    /**
     * @brief Returns the latest bar from the data feed. Moves iterator in data frame
     * 
     */
    template<typename _DataModel>
    bool FinApiHandler<_DataModel>::get_next_model(_DataModel*& data_model, std::string symbol)
    {
        if (!in_symbol_data(symbol)) return false;
        if (symbol_data[symbol]->get_next_model(data_model)) return true;
        else return false;
    }

    /**
     * @brief Checks whether a symbol is in the symbol data map
     * 
     * @tparam _DataModel :
     * @tparam _Stream    :
     * @param symbol      :
     * @return true       :
     * @return false      :
     */
    template<typename _DataModel>
    bool FinApiHandler<_DataModel>::in_symbol_data(std::string symbol)
    {
        typename std::unordered_map<std::string, modeler::FinDataFrame<_DataModel, _Stream>* >::const_iterator find = symbol_data.find(symbol);
        if (find == symbol_data.end()) return false;
        else return true;
    }

    /**
     * @brief Checks whether a symbol is in the symbol data map
     * 
     * @tparam _DataModel 
     * @tparam _Stream 
     * @param symbol 
     * @return true 
     * @return false 
     */
    template<typename _DataModel>
    bool FinApiHandler<_DataModel>::in_latest_data(std::string symbol)
    {
        typename std::unordered_map<std::string, _DataModel*>::const_iterator find = latest_symbol_data.find(symbol);
        if (find == latest_symbol_data.end()) return false;
        else return true;
    }

    /**
     * @brief Creates a FinDataFrame for symbol using _working_year
     * 
     * @param symbol                                  : identifier
     * @return FinDataFrame<_DataModel, _Stream>* : pointer to allocated data frame
     */
    template<typename _DataModel>
    modeler::FinDataFrame<_DataModel, _Stream>* FinApiHandler<_DataModel>::download_symbol_data(std::string symbol)
    {
        std::string connString(_data_dir + get_binary_file_dir(model_id, symbol, std::to_string(_working_year)));
        // TO DO: WRITE THIS TO ACCOMODATE IFSTREAM
        _Stream inFile(connString.c_str(), Cloud::LocalServer);
        
        modeler::FinDataFrame<_DataModel, _Stream>* newDataFrame = nullptr;
        //std::cout << inFile.status() << std::endl;
        if (inFile.good())
        {    
            //std::cout << connString << std::endl;           
            newDataFrame = new modeler::FinDataFrame<_DataModel, _Stream>(inFile);
            newDataFrame->sort_data("Date");
        }   
        return newDataFrame;
    }

    /* METHODS */

    /**
     * @brief Returns the pointer to the data model of a given symbol from latest_symbol_data
     *        if the symbol is in the unordered_map
     * 
     * @tparam _DataModel :
     * @param data_model  :
     * @param symbol      :
     * @return true       :
     * @return false      :
     */
    template<typename _DataModel>
    bool FinApiHandler<_DataModel>::get_latest_symbol_data(models::Bar& data_model, std::string symbol)
    { 
        if (!in_latest_data(symbol)) return false;
        else
        {
            data_model = latest_symbol_data[symbol]->get_bar();
            return true;
        }   
    }

    /**
     * @brief Pushes the latest bar to the latest_symbol_data structure
     *        for all symbols in the symbol list
     * 
     */
    template<typename _DataModel>
    bool FinApiHandler<_DataModel>::update_latest_data()
    {
        bool get_next_year = false;
        MarketEvent* event = nullptr;
        char t;

        // clear latest_symbol_data
        latest_symbol_data.clear();

        std::string symbol;
        _DataModel* data_bar = nullptr;
        while (all_symbols.get_next_symbol(symbol))
        {
            // symbol to lower case
            std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
            // confirm symbol is in symbol data
            if (!in_symbol_data(symbol)) continue;

            // get next data model in each symbol's data frame until the 
            // end of the data frame is reached
            if (get_next_model(data_bar, symbol)) latest_symbol_data[symbol] = data_bar; 
            else get_next_year = true;
        }
        // update data or quit simulation
        if (get_next_year)
        {
            if (_working_year < year_range.second)
            {
                std::cin >> t;
                _working_year += 1;
                get_working_year_data();        
                update_latest_data();   
            }
            else
            {
                _continue_backtest = false;
                return false;  
            }   
        }

        if (!get_next_year)
        {
            event = new MarketEvent();
            event_q->push(event);
            return true;
        }
    }

    /**
     * @brief Displays all data in latest_symbol_data structure
     * 
     */
    template<typename _DataModel>
    void FinApiHandler<_DataModel>::display_latest_data()
    {
        for (std::pair<std::string, _DataModel*> element : latest_symbol_data)
        {
            std::cout << "|--------" << element.first << " --------|" << std::endl;
            models::display_model(*element.second);
            std::cout << std::endl;
        }       
    }

    /**
     * @brief Returns the date of latest data
     * 
     * @tparam _DataModel 
     * @param time 
     * @return true 
     * @return false 
     */
    template<typename _DataModel>
    bool FinApiHandler<_DataModel>::get_latest_data_timestamp(models::TimeStamp& time)
    {
        if (!latest_symbol_data.size())
            return false;
        else
        {
            time = latest_symbol_data.begin()->second->date();
            return true;
        }
    }
    
    template<typename _DataModel>
    bool FinApiHandler<_DataModel>::continue_backtest()
        { return _continue_backtest; }

    /**
     * @brief Reset working year and reset data
     * 
     * @tparam _DataModel 
     */
    template<typename _DataModel>
    void FinApiHandler<_DataModel>::reset_backtest()
    {
        _continue_backtest = true;
        _working_year = year_range.first;
        get_working_year_data();
    }

#pragma endregion
    
    
}
}
}