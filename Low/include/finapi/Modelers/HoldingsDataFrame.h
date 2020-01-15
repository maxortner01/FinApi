/**
 * @file HoldingsDataFrame.h
 * 
 * @brief 
 * 
 * @author   Josh Ortner
 * @date     2020-1-13
 * @version  0.0.1
 * 
 * @copyright Copyright (c) 2019
 */

#pragma once

#include "../Core/SysInclude.h"
//#include "../Core/Models.h"

namespace finapi
{
namespace modeler
{
    /**
     * @brief Encapsulates periodic holdings data allowing the user to create a data 
     *        structure where a symbol is mapped to a value (i.e. num of assets owned)
     *        at a particular period.
     * 
     */
    template<typename T1, typename T2>
    class HoldingsDataFrame
    {

        std::vector<std::unordered_map<T1, T2>* > _holdings;
        typename std::vector<std::unordered_map<T1, T2>* >::iterator _curr_holdings;

    public:

        /* CONSTRUCTORS */
 

        /* DESTRUCTOR */

        ~HoldingsDataFrame();

        /* METHODS */

        void clear_data();

        bool add_field(T1, T2);

        T2& get_value(T1);

        void reset_current_holdings();

        void add_new_holdings(bool);

        void display_current_holdings();

        unsigned int size() const;

        /* OPERATOR OVERLOADS */

        std::unordered_map<T1, T2>& operator [](unsigned int) const;
        
    };

    /* CONSTRUCTORS */
 

    /* DESTRUCTOR */

    /**
     * @brief Destroy the Holdings Data Frame:: Holdings Data Frame object
     * 
     */
    template<typename T1, typename T2>
    HoldingsDataFrame<T1, T2>::~HoldingsDataFrame()
        { clear_data(); }

    /* METHODS */

    /**
     * @brief Deallocate data and clear vector
     * 
     */
    template<typename T1, typename T2>
    void HoldingsDataFrame<T1, T2>::clear_data()
    {
        for (int i = 0; i < _holdings.size(); i++)
            delete _holdings[i];
        _holdings.clear();
    }

    /**
     * @brief Adds a field and value to _curr_holdings
     * 
     * @tparam T1 
     * @tparam T2 
     * @return true 
     * @return false 
     */
    template<typename T1, typename T2>
    bool HoldingsDataFrame<T1, T2>::add_field(T1 field, T2 value)
    {
        if (!_holdings.size())
            return false;
        if (_curr_holdings != _holdings.end())
            (*(*_curr_holdings))[field] = value;
        return true;
    }

    /**
     * @brief Returns the reference to the value mapped to given field in _curr_holdings
     * 
     * @tparam T1 
     * @tparam T2 
     * @param field 
     */
    template<typename T1, typename T2>
    T2& HoldingsDataFrame<T1, T2>::get_value(T1 field)
        { return (*(*_curr_holdings))[field]; }

    /**
     * @brief Moves _curr_holdings to last element in vector
     * 
     * @tparam T1 
     * @tparam T2 
     */
    template<typename T1, typename T2>
    void HoldingsDataFrame<T1, T2>::reset_current_holdings()
        { _curr_holdings = _holdings.end() - 1; }


    /**
     * @brief Creates a new holdings map within _holdings vector.
     *        If copy is true, the new holdings is set equal to the previous.
     *        _curr_holdings iterator is incremented.
     * 
     * @tparam T1 
     * @tparam T2 
     * @param copy 
     */
    template<typename T1, typename T2>
    void HoldingsDataFrame<T1, T2>::add_new_holdings(bool copy)
    {
        std::unordered_map<T1, T2>* new_holdings = nullptr;
        // data is empty, or copy set to false
        if (!_holdings.size() || !copy)
            new_holdings = new std::unordered_map<T1, T2>();
        else
            new_holdings = new std::unordered_map<T1, T2>(*(*_curr_holdings));
        _holdings.push_back(new_holdings);
        _holdings.shrink_to_fit();
        reset_current_holdings();
    }

    /**
     * @brief Displays the holdings at a particular index
     * 
     * @tparam T1 
     * @tparam T2 
     * @param index 
     */
    template<typename T1, typename T2>
    void HoldingsDataFrame<T1, T2>::display_current_holdings()
    {
        //if (index < _holdings.size())
        for (std::pair<T1, T2> element : *(*_curr_holdings))
            std::cout << element.first << ":\t" << element.second << std::endl;
    }

    /**
     * @brief Returns the size of the holdings vector
     * 
     * @tparam T1 
     * @tparam T2 
     * @return int 
     */
    template<typename T1, typename T2>
    unsigned int HoldingsDataFrame<T1, T2>::size() const
        { return _holdings.size(); }

    /* OPERATOR OVERLOADS */

    template<typename T1, typename T2>
    std::unordered_map<T1, T2>& HoldingsDataFrame<T1, T2>::operator [](unsigned int index) const
    {
        assert(index < _holdings.size());
        return (*_holdings[index]);
    }
}
}