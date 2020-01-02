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

    /*  CLASS : OrderEvent */    

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

    /*  CLASS : FillEvent */  

    double FillEvent::calculate_final_cost()
    {
        double final_cost;
        calculate_final_broker_cost(broker, quantity, fill_cost, final_cost);
    }


    /*     DataHandler.h      */

    /*      SymbolList        */

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

}
}