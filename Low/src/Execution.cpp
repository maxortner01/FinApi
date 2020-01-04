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
    /*        ExSystemStructs.h         */ 

    /**
     * TIME_STAMP IMPLEMENTATION
     */ 

    /* CONSTRUCTORS */

    /**
     * @brief Construct a new Time Stamp:: Time Stamp object
     * 
     * @param y : year
     * @param m : month
     * @param d : day
     */
    TimeStamp::TimeStamp(int y, int m, int d) :
        year(y), month(m), day(d) { }

    TimeStamp::TimeStamp(const TimeStamp& rhs) :
        year(rhs.year), month(rhs.month), day(rhs.day) { }

    /* OPERATOR OVERLOADS */

    const bool TimeStamp::operator == (TimeStamp const &rhs)
        { return (year == rhs.year) && (month == rhs.month) && (day == rhs.day); }

    const bool TimeStamp::operator > (TimeStamp const &rhs)
    {
        if (year > rhs.year)
            return true;
        else if (year < rhs.year)
            return false;
        else if (month > rhs.month)
            return true;
        else if (month < rhs.month)
            return false;
        else if (day > rhs.day)
            return true;
        else
            return false;           
    }

    const bool TimeStamp::operator < (TimeStamp const &rhs)
    {
        if (year < rhs.year)
            return true;
        else if (year > rhs.year)
            return false;
        else if (month < rhs.month)
            return true;
        else if (month > rhs.month)
            return false;
        else if (day < rhs.day)
            return true;
        else
            return false;           
    }

    /*        Event.h         */ 
namespace backtest
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

    std::string get_symbol_directory(DataModelType model)
    {
        switch (model)
        {
            case EODADJ:
                return "historical-prices";
        }
    }

    /* Get directory containing binary files for specified DataModel */

    std::string get_binary_directory(DataModelType model)
    {
        switch (model)
        {
            case EODADJ:
                return "eod-adj";
        }
    }

    /**
     * BAR IMPLEMENTATION
     */ 

    /* CONSTRUCTORS */

    Bar::Bar(c_fref o, c_fref h, c_fref l, c_fref c, c_fref v) :
        _open(&o), _high(&h), _low(&l), _close(&c), _volume(&v) { }

    /* METHODS */

    float Bar::open() const
        { return (*_open); }

    float Bar::high() const  
        { return (*_high); }

    float Bar::low() const
        { return (*_low); }

    float Bar::close() const
        { return (*_close); }

    float Bar::volume() const
        { return (*_volume); }

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
    SignalEvent::SignalEvent(std::string sym, TimeStamp d_time, std::string sig_type) :
        Event("SIGNAL"), symbol(sym), date_time(d_time), signal_type(sig_type)
    { }

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
    void OrderEvent::printOrder()
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

    /**
     * FIN_API_HANDLER IMPLEMENTATION
     */ 

    template class FinApiHandler<EodAdj>;    

    /* CONSTRUCTORS */

    template<typename DataModel>
    FinApiHandler<DataModel>::FinApiHandler(int start_year, int end_year, std::string d_dir, SymbolList s_list, std::queue<Event*>& events) :
        DataHandler(), event_q(&events), _continue_backtest(true), data_id(EODADJ), year_range(start_year, end_year), 
        data_directory(d_dir), all_symbols(s_list), _working_year(start_year) 
    { get_working_year_data(); }

    /* DESTRUCTOR */

    /**
     * @brief Destroy the Eod Adj Handler:: Eod Adj Handler object
     *        Deallocate dataframes for each symbol in unordered map
     * 
     */
    template<typename DataModel>
    FinApiHandler<DataModel>::~FinApiHandler<DataModel>()
    {
        for (auto data : symbol_data)
            delete data.second;            
    }

    /* PRIVATE METHODS */

    template<typename DataModel>
    void FinApiHandler<DataModel>::get_working_year_data()
    {
        std::string symbol;
        FinDataFrame<DataModel, std::ifstream>* data = nullptr;

        symbol_data.clear();

        // iterate through all symbols in symbol list
        while (all_symbols.get_next_symbol(symbol))
        {

            std::cout << "test 2 " << symbol << std::endl;
            // create connection string
            std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
            FinDataFrame<DataModel, std::ifstream>* data = get_symbol_data(symbol);
            // add symbol data to map
            if (data != nullptr)
                symbol_data[symbol] = data;
        }
    }

    /**
     * @brief Returns the latest bar from the data feed. Moves iterator in data frame
     * 
     */
    template<typename DataModel>
    bool FinApiHandler<DataModel>::get_next_model(DataModel*& data_model, std::string symbol)
    {
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
    template<typename DataModel>
    bool FinApiHandler<DataModel>::get_connection(std::string symbol, std::ifstream& fileConn)
    {
        // CHECK IF YEAR IS IN DATA BASE, return false if not
        std::string connString;
        connString = data_directory + "/" + symbol + "/" + get_binary_directory(data_id) + "/" + std::to_string(_working_year) + ".bin";

        fileConn.open(connString);
        if (fileConn.good())
            return true;
        else
            return false;
    }

    /**
     * @brief Creates a FinDataFrame for symbol using _working_year
     * 
     * @param symbol                                : identifier
     * @return FinDataFrame<DataModel, std::ifstream>* : pointer to allocated data frame
     */
    template<typename DataModel>
    FinDataFrame<DataModel, std::ifstream>* FinApiHandler<DataModel>::get_symbol_data(std::string symbol)
    {
        std::cout << "test " << symbol << std::endl;
        std::ifstream inFile;
        FinDataFrame<DataModel, std::ifstream>* newDataFrame = nullptr;

        if (get_connection(symbol, inFile))
        {   
            newDataFrame = new FinDataFrame<DataModel, std::ifstream>(inFile);
            newDataFrame->sort_data("Date");
            inFile.close();
        }       

        return newDataFrame;
    }

    /* METHODS */

    template<typename DataModel>
    bool FinApiHandler<DataModel>::get_latest_model(DataModel*& data_model, std::string symbol)
        { return latest_symbol_data[symbol]; }

    /**
     * @brief Pushes the latest bar to the latest_symbol_data structure
     *        for all symbols in the symbol list
     * 
     */
    template<typename DataModel>
    void FinApiHandler<DataModel>::update_models()
    {
        bool get_next_year = false;
        MarketEvent* event = nullptr;
        char t;

        // clear latest_symbol_data
        latest_symbol_data.clear();

        std::string symbol;
        DataModel* data_bar = nullptr;

        while (all_symbols.get_next_symbol(symbol))
        {
            std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
            typename std::unordered_map<std::string, FinDataFrame<DataModel, std::ifstream>* >::const_iterator got = symbol_data.find(symbol);
            if (got == symbol_data.end())
                break;

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
                _continue_backtest = false;
            }
            else
            {
                // increment year, clear data, and load new year data
                std::cout << "Year completed : " << _working_year << std::endl;
                std::cin >> t;
                _working_year += 1;
                std::cout << _working_year << std::endl;
                get_working_year_data();                    
            }
        }

        event = new MarketEvent();
        event_q->push(event);
    }

    /**
     * @brief Displays all data in latest_symbol_data structure
     * 
     */
    template<typename DataModel>
    void FinApiHandler<DataModel>::display_latest_data()
    {
        std::cout << "Size: " << latest_symbol_data.size() << std::endl;
        for (auto data : latest_symbol_data)
        {
            std::cout << "|--------" << data.first << " --------|" << std::endl;
            data.second->display();
            std::cout << std::endl;
        }       
    }
    
    template<typename DataModel>
    bool FinApiHandler<DataModel>::continue_backtest()
        { return _continue_backtest; }

    
    /*     Strategy.h      */

    /**
     * STRATEGY IMPLEMENTATION
     */ 

    /* CONSTRUCTORS */

    BuyAndHold::BuyAndHold(SymbolList& s_list, std::queue<Event*>& events, FinApiHandler<EodAdj>& eod_d) :
        Strategy(), all_symbols(s_list), events(&events), eod_data(&eod_d)
    { calculate_initial_bought(); }


    /* PRIVATE METHODS */

    /**
     * @brief Adds keys to bought unordered map for
     *        all symbols and sets them to false
     * 
     */
    void BuyAndHold::calculate_initial_bought()
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
    void BuyAndHold::calculate_signals(Event* event)
    {
        std::string symbol;

        if (event->type() == "MARKET")
        {
            while (all_symbols.get_next_symbol(symbol))
            {
                
            }    
        }
        
        
    }

}
}