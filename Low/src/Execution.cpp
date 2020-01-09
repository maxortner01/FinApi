#include "finapi/finapi.h"

/* Final Calculations for Specific Broker */

/**
 * @brief Calculate the final cost of an order using define commission structure
 * 
 * @param quantity   : order quantity
 * @param fill_cost  : holdings value of single security in dollars
 * @param final_cost : reference to return the final calulated cost
 */
void alpaca_final_cost(unsigned int quantity, double fill_cost, double& final_cost)
{
    final_cost = (double)quantity * fill_cost;
}

namespace finapi
{

    typedef Cloud::ServerStream _Stream;
    

    /*        Event.h         */ 
namespace execution
{
    /* Protocols for commission calculation */

    void calculate_final_broker_cost(BrokerType broker, unsigned int quantity, double fill_cost, double& final_cost)
    {
        switch(broker)
        {
            case ALPACA:
                alpaca_final_cost(quantity, fill_cost, final_cost);
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

    /**
     * BAR IMPLEMENTATION
     */ 

    /* CONSTRUCTORS */

    //Bar::Bar(c_fref o, c_fref h, c_fref l, c_fref c, c_fref v) :
    //    _open(&o), _high(&h), _low(&l), _close(&c), _volume(&v) { }
//
    ///* METHODS */
//
    //float Bar::open() const
    //    { return (*_open); }
//
    //float Bar::high() const  
    //    { return (*_high); }
//
    //float Bar::low() const
    //    { return (*_low); }
//
    //float Bar::close() const
    //    { return (*_close); }
//
    //float Bar::volume() const
    //    { return (*_volume); }

#pragma region EVENT_H

    /**
     * EVENT IMPLEMENTATION
     */ 

    /* METHODS */

    /* CONSTRUCTORS */
        
    Event::Event(std::string e_type) :
        event_type(e_type) 
    { }
        
    /* METHODS */

    std::string Event::type()
        { return event_type; }


    /**
     * MARKET_EVENT IMPLEMENTATION
     */ 

    /* CONSTRUCTORS */

    MarketEvent::MarketEvent() :
            Event("MARKET") { }

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
    SignalEvent::SignalEvent(std::string sym, models::TimeStamp d_time, std::string sig_type) :
        Event("SIGNAL"), symbol(sym), date_time(d_time), signal_type(sig_type)
    { }

    /* METHODS */

    /**
     * @brief Output the values within the Order
     * 
     */
    void SignalEvent::print_event()
    {
        std::cout << "Signal: Symbol=" << symbol
                  << ", Year=" << date_time.year
                  << ", Month=" << date_time.month
                  << ", Day="<< date_time.day << std::endl;
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
        Event("ORDER"), symbol(sym), order_type(ord_type), direction(dirx), quantity(quant)
    { }

    /* METHODS */

    /**
     * @brief Output the values within the Order
     * 
     */
    void OrderEvent::print_event()
    {
        std::cout << "Order: Symbol=" << symbol
                  << ", Type=" << order_type
                  << ", Quantity=" << quantity
                  << ", Direction="<< direction << std::endl;
    }

    /**
     * FILL_EVENT IMPLEMENTATION
     */ 

    /* CONSTRUCTORS */

    FillEvent::FillEvent(std::string t_index, std::string sym, std::string exch, std::string dir,
                         BrokerType t_broker, unsigned int quant, float f_cost) :
        Event("FILL"), time_index(t_index), symbol(sym), exchange(exch), 
        direction(dir), broker(t_broker), quantity(quant), fill_cost(f_cost)
    { }

    /* METHODS */

    double FillEvent::calculate_final_cost()
    {
        double final_cost;
        calculate_final_broker_cost(broker, quantity, fill_cost, final_cost);
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

#pragma endregion

#pragma region STRATEGY_H

    /**
     * STRATEGY IMPLEMENTATION
     */ 

    /* CONSTRUCTORS */

    template class BuyAndHold<backtest::FinApiHandler<models::EodAdj>, models::EodAdj >;

    template<typename _DataHandler, typename _DataModel>
    BuyAndHold<_DataHandler, _DataModel>::BuyAndHold(SymbolList& s_list, EVENT_QUEUE_PTR events, _DataHandler& eod_d) :
        Strategy(), all_symbols(s_list), event_q(events), eod_data(&eod_d), data_model(nullptr)
    { calculate_initial_bought(); }


    /* PRIVATE METHODS */

    /**
     * @brief Adds keys to bought unordered map for
     *        all symbols and sets them to false
     * 
     */
    template<typename _DataHandler, typename _DataModel>
    void BuyAndHold<_DataHandler, _DataModel>::calculate_initial_bought()
    {
        std::string symbol;

        while (all_symbols.get_next_symbol(symbol))
        {
            std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
            bought[symbol] = false;
        }
    }

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
    template<typename _DataHandler, typename _DataModel>
    void BuyAndHold<_DataHandler, _DataModel>::calculate_signals(Event* event)
    {
        std::string symbol;
        SignalEvent* newSignal = nullptr;

        if (event->type() == "MARKET")
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
                        // see over loads for model get timestamp
                        newSignal = new SignalEvent(symbol, data_model->date(), "LONG");
                        event_q->push(newSignal);
                        bought[symbol] = true;
                    }
                }
            }    
        }        
    }

    /**
     * @brief Display all symbol holdings
     * 
     * @tparam _DataHandler 
     * @tparam _DataModel 
     */
    template<typename _DataHandler, typename _DataModel>
    void BuyAndHold<_DataHandler, _DataModel>::display_holdings()
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

#pragma endregion

#pragma region PORTFOLIO_H

    template class NaivePortfolio<backtest::FinApiHandler<models::EodAdj> >;

    /* CONSTRUCTORS */

    /**
     * @brief Construct a new NaivePortfolio<_DataHandler>::NaivePortfolio object
     * 
     * @tparam _DataHandler 
     * @param d 
     * @param e_ptr 
     * @param all_syms 
     * @param s_date 
     * @param i_cap 
     */
    template<typename _DataHandler>
    NaivePortfolio<_DataHandler>::NaivePortfolio(_DataHandler& d, EVENT_QUEUE_PTR e_ptr, SymbolList all_syms, models::TimeStamp s_date, float i_cap) :
        Portfolio(), _data(&d), _events(e_ptr), _all_symbols(all_syms), _start_date(s_date), _initial_capital(i_cap)
    { 
        construct_all_positions();
        construct_all_holdings();
        _current_positions = _all_positions.begin();
        _current_holdings  = _all_holdings.begin();

        for (std::pair<std::string, float> element : *(*_current_holdings))
        {
            std::cout << element.first << ":\t" << element.second << std::endl;
        } 
    }

    /* DESTRUCTOR */

    template<typename _DataHandler>
    NaivePortfolio<_DataHandler>::~NaivePortfolio()
    {
        for (int i = 0; i < _all_positions.size(); i++)
            delete _all_positions[i];
        for (int i = 0; i < _all_holdings.size(); i++)
            delete _all_holdings[i];
    }

    /* PRIVATE METHODS */

    /**
     * @brief Creates a positions map for the start date and adds to the 
     *        all_positions vector
     * 
     * @tparam _DataHandler 
     */
    template<typename _DataHandler>
    void NaivePortfolio<_DataHandler>::construct_all_positions()
    {
        std::string symbol;
        std::unordered_map<std::string, float>* new_positions;
        new_positions = new std::unordered_map<std::string, float>();

        while (_all_symbols.get_next_symbol(symbol))
        {
            std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
            (*new_positions)[symbol] = 0.0;
        }
    
        (*new_positions)["year"]  = _start_date.year;
        (*new_positions)["month"] = _start_date.month;
        (*new_positions)["day"]   = _start_date.day;

        _all_positions.push_back(new_positions);
        _all_positions.shrink_to_fit();

        //for (std::pair<std::string, float> element : _all_positions[0])
        //{
        //    std::cout << element.first << ": " << element.second << std::endl;
        //}  
    }

    template<typename _DataHandler>
    void NaivePortfolio<_DataHandler>::construct_all_holdings()
    {
        std::string symbol;
        std::unordered_map<std::string, float>* new_holdings;
        new_holdings = new std::unordered_map<std::string, float>();

        while (_all_symbols.get_next_symbol(symbol))
        {
            std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
            (*new_holdings)[symbol] = 0.0;
        }

        (*new_holdings)["cash"]  = _initial_capital;
        (*new_holdings)["comm"]  = 0.0;
        (*new_holdings)["total"] = _initial_capital;
        (*new_holdings)["year"]  = _start_date.year;
        (*new_holdings)["month"] = _start_date.month;
        (*new_holdings)["day"]   = _start_date.day;

        _all_holdings.push_back(new_holdings);
        _all_holdings.shrink_to_fit();
        
        
    }

    /* METHODS */

    /**
     * @brief Acts on a SignalEvent to generate new orders
     *        based on the portfolio logic.
     * 
     */
    template<typename _DataHandler>
    void NaivePortfolio<_DataHandler>::update_signal(Event* event)
    {
        std::cout << "IMPLEMENT" << std::endl;
    }

    /**
     * @brief Updates the portfolio current positions and 
     *        holdings from a FillEvent
     * 
     */
    template<typename _DataHandler>
    void NaivePortfolio<_DataHandler>::update_fill(Event* event)
    {
        std::cout << "IMPLEMENT" << std::endl;
    }
    

#pragma endregion

namespace backtest
{

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
    FinApiHandler<_DataModel>::FinApiHandler(std::string d_dir, EVENT_QUEUE_PTR events, SymbolList s_list, int start_year, int end_year) :
        DataHandler(), event_q(events), _continue_backtest(true), _data_dir(d_dir), model_id(_DataModel::model_type()), year_range(start_year, end_year), 
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
        
        if (inFile.good())
        {    
            std::cout << connString << std::endl;           
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
    bool FinApiHandler<_DataModel>::get_latest_symbol_data(_DataModel*& data_model, std::string symbol)
    { 
        if (!in_latest_data(symbol)) return false;
        else
        {
            data_model = latest_symbol_data[symbol];
            return true;
        }   
    }

    /**
     * @brief Pushes the latest bar to the latest_symbol_data structure
     *        for all symbols in the symbol list
     * 
     */
    template<typename _DataModel>
    void FinApiHandler<_DataModel>::update_latest_data()
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
            if (_working_year <= year_range.second)
            {
                std::cin >> t;
                _working_year += 1;
                get_working_year_data();        
                update_latest_data();   
            }
            else
            {
                _continue_backtest = false;
            }         
        }

        if (!get_next_year)
        {
            event = new MarketEvent();
            event_q->push(event);
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
    
    template<typename _DataModel>
    bool FinApiHandler<_DataModel>::continue_backtest()
        { return _continue_backtest; }

#pragma endregion
    
    
}
}
}