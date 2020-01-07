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


    /*     DataHandler.h      */

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

    /*     Strategy.h      */

    /**
     * STRATEGY IMPLEMENTATION
     */ 

    /* CONSTRUCTORS */

    template class BuyAndHold<backtest::FinApiHandler<models::EodAdj, Cloud::File>, models::EodAdj >;

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

    //std::unordered_map<std::string, bool> bought;
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

namespace backtest
{
    /**
     * FIN_API_HANDLER IMPLEMENTATION
     */ 

    template class FinApiHandler<models::EodAdj, Cloud::File>;    

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
    template<typename _DataModel, typename _Stream>
    FinApiHandler<_DataModel, _Stream>::FinApiHandler(std::string d_dir, EVENT_QUEUE_PTR events, SymbolList s_list, int start_year, int end_year) :
        DataHandler(), event_q(events), _continue_backtest(true), _data_dir(d_dir), model_id(_DataModel::model_type()), year_range(start_year, end_year), 
        all_symbols(s_list), _working_year(start_year) 
    { get_working_year_data(); }

    /* DESTRUCTOR */

    /**
     * @brief Destroy the Eod Adj Handler:: Eod Adj Handler object
     *        Deallocate dataframes for each symbol in unordered map
     * 
     */
    template<typename _DataModel, typename _Stream>
    FinApiHandler<_DataModel, _Stream>::~FinApiHandler()
    {
        for (std::pair<std::string, modeler::FinDataFrame<_DataModel, _Stream>* > element : symbol_data)
            delete element.second;        
    }

    /* PRIVATE METHODS */

    template<typename _DataModel, typename _Stream>
    void FinApiHandler<_DataModel, _Stream>::get_working_year_data()
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
            data = get_symbol_data(symbol);
            // add symbol data to map
            if (data)
                symbol_data[symbol] = data;                                            
        }
    }

    /**
     * @brief Returns the latest bar from the data feed. Moves iterator in data frame
     * 
     */
    template<typename _DataModel, typename _Stream>
    bool FinApiHandler<_DataModel, _Stream>::get_next_model(_DataModel*& data_model, std::string symbol)
    {
        typename std::unordered_map<std::string, modeler::FinDataFrame<_DataModel, _Stream>* >::const_iterator find = symbol_data.find(symbol);
        if (find == symbol_data.end())
            return false;
        if (symbol_data[symbol]->get_next_model(data_model))
            return true;
        else
            return false;
    }
    /**
     * @brief Creates a connection object to the desired symbol file
     * 
     * @param symbol : identifier
     * @return true  : connection was successful
     * @return false : connection was not successful
     */
    template<typename _DataModel, typename _Stream>
    bool FinApiHandler<_DataModel, _Stream>::get_connection(std::string symbol, std::ifstream& fileConn)
    {
        // CHECK IF YEAR IS IN DATA BASE, return false if not
        std::string connString(get_binary_file_dir(model_id, symbol, std::to_string(_working_year)));
        fileConn.open(connString);
        if (fileConn.good())
            return true;
        else
            return false;
    }

    /**
     * @brief Creates a FinDataFrame for symbol using _working_year
     * 
     * @param symbol                                  : identifier
     * @return FinDataFrame<_DataModel, _Stream>* : pointer to allocated data frame
     */
    template<typename _DataModel, typename _Stream>
    modeler::FinDataFrame<_DataModel, _Stream>* FinApiHandler<_DataModel, _Stream>::get_symbol_data(std::string symbol)
    {
        std::string connString(get_binary_file_dir(model_id, symbol, std::to_string(_working_year)));
        _Stream inFile(connString.c_str(), Cloud::LocalServer);
        modeler::FinDataFrame<_DataModel, _Stream>* newDataFrame = nullptr;
        std::cout << connString << std::endl;
        if (inFile.good())
        {               
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
    template<typename _DataModel, typename _Stream>
    bool FinApiHandler<_DataModel, _Stream>::get_latest_symbol_data(_DataModel*& data_model, std::string symbol)
    { 
        typename std::unordered_map<std::string, _DataModel*>::const_iterator find = latest_symbol_data.find(symbol);
        if (find == latest_symbol_data.end())
            return false;
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
    template<typename _DataModel, typename _Stream>
    void FinApiHandler<_DataModel, _Stream>::update_latest_data()
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
            typename std::unordered_map<std::string, modeler::FinDataFrame<_DataModel, _Stream>* >::const_iterator find = symbol_data.find(symbol);
            if (find == symbol_data.end())
                continue;

            // get next data model in each symbol data frame until the 
            // end of the data frame is reached
            if (get_next_model(data_bar, symbol))
                latest_symbol_data[symbol] = data_bar; 
            else
                get_next_year = true;
        }
        // update data or quit simulation
        if (get_next_year)
        {
            // end of data reach, load new year
            if (_working_year == year_range.second)
            {
                std::cout << "Year completed : " << _working_year << std::endl;
                _continue_backtest = false;
            }
            else
            {
                // increment year, clear data, and load new year data
                std::cout << "Year completed : " << _working_year << std::endl;
                //std::cin >> t;
                _working_year += 1;
                get_working_year_data();        
                update_latest_data();            
            }
        }

        if (_continue_backtest)
        {
            event = new MarketEvent();
            event_q->push(event);
        }
    }

    /**
     * @brief Displays all data in latest_symbol_data structure
     * 
     */
    template<typename _DataModel, typename _Stream>
    void FinApiHandler<_DataModel, _Stream>::display_latest_data()
    {
        for (std::pair<std::string, _DataModel*> element : latest_symbol_data)
        {
            std::cout << "|--------" << element.first << " --------|" << std::endl;
            models::display_model(*element.second);
            std::cout << std::endl;
        }       
    }
    
    template<typename _DataModel, typename _Stream>
    bool FinApiHandler<_DataModel, _Stream>::continue_backtest()
        { return _continue_backtest; }

    
    
}
}
}