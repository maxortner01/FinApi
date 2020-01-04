/**
 * @file FinDataFrame.h
 * 
 * @brief 
 * 
 * @author   Josh Ortner
 * @date     2020-1-1
 * @version  0.0.1
 * 
 * @copyright Copyright (c) 2019
 */

#pragma once

#include <vector>
#include <algorithm>

#include "Models.h"

namespace finapi
{
    //template<typename T>
    //struct isValue
    //{
    //    T check_value;
//
    //    isValue(T val) : check_value(val) { }
//
    //    bool operator()(const T *rhs) const
    //    {
    //        return (rhs->)
    //    }
    //};

    /**
     * @brief Provides a useful interface to interacte with data
     *        after it has been deserialized into its Data Model
     * 
     *        - REQUIRED MODEL OVERLOADS: SEE EODADJ               
     * 
     * @tparam DataModel 
     * @tparam Stream 
     */
    template<typename DataModel, typename Stream>
    class FinDataFrame
    {
        std::vector<DataModel*> data;
        typename std::vector<DataModel*>::iterator it;

    public:

        /* CONSTRUCTORS */

        FinDataFrame(Stream&);

        /* DESTRUCTOR */

        ~FinDataFrame();

        /* METHODS */

        bool get_next_model(DataModel*&);

        void sort_data(std::string, bool=true);

        void append(Stream&);

        void display_head();

        void display_tail();

        DataModelType get_model_type();

    };

    /* CONSTRUCTORS */

    /**
     * @brief Construct a new Fin Data Frame< Model Struct,  Conn Type>:: Fin Data Frame object
     * 
     * @tparam DataModel :
     * @tparam Stream    :
     * @param connection : data stream
     */
    template<typename DataModel, typename Stream>
    FinDataFrame<DataModel, Stream>::FinDataFrame(Stream& connection)
    {
        deserialize(data, connection);
        it = data.begin();
    }

    /* DESTRUCTOR */

    /**
     * @brief Destroy the Fin Data Frame< Model Struct,  Conn Type>:: Fin Data Frame object
     * 
     * @tparam DataModel :
     * @tparam Stream    :
     */
    template<typename DataModel, typename Stream>
    FinDataFrame<DataModel, Stream>::~FinDataFrame()
        { clean_list(data); }

    /* METHODS */

    /**
     * @brief Iterates through the data return a pointer to the current data model
     *        until the end of the data is reached
     * 
     * @tparam DataModel :
     * @tparam Stream    :
     * @param dat        : pointer that will be assigned the location of data struct
     * @return true      : data returned
     * @return false     : end of data reached
     */
    template<typename DataModel, typename Stream>
    bool FinDataFrame<DataModel, Stream>::get_next_model(DataModel*& dat)
    {
        if (it == data.end())
        {
            it = data.begin();
            return false;
        }

        dat = (*it);
        it++;

        return true;
    }

    /**
     * @brief Sorts the data by a specified field
     * 
     * @tparam DataModel :
     * @tparam Stream    : 
     * @param field      : field to sort by
     * @param ascd       : sort direction, true - ascending, false - descending
     */
    template<typename DataModel, typename Stream>
    void FinDataFrame<DataModel, Stream>::sort_data(std::string field, bool ascd)
    { 
        // set comparator variables
        ModelComparator<DataModel> comparator;
        ModelComparator<DataModel>::field_to_compare = field;
        ModelComparator<DataModel>::ascending = ascd;
        sort(data.begin(), data.end(), &ModelComparator<DataModel>::compare_ptr);
    }

    /**
     * @brief Append data from another stream to data vector
     *        NOTE: Data may need to be reordered after data is appended
     * 
     * @tparam DataModel :
     * @tparam Stream    :
     * @param connection : data stream
     */
    template<typename DataModel, typename Stream>
    void FinDataFrame<DataModel, Stream>::append(Stream& connection)
    { 
        // deserialize new connection
        std::vector<DataModel*> newData;
        deserialize(newData, connection);
        
        for (int i = 0; i < newData.size(); i++)
            data.push_back(newData[i]);

        it = data.begin();
    }

    /**
     * @brief Print the first ten structs to the terminal
     * 
     * @tparam DataModel 
     * @tparam Stream 
     */
    template<typename DataModel, typename Stream>
    void FinDataFrame<DataModel, Stream>::display_head()
    {
        for (int i = 0; i < 10; i++)
            data[i]->display();
    }

    /**
     * @brief Print the last ten structs to the terminal
     * 
     * @tparam DataModel 
     * @tparam Stream 
     */
    template<typename DataModel, typename Stream>
    void FinDataFrame<DataModel, Stream>::display_tail()
    {
        for (int i = data.size() - 10; i < data.size(); i++)
            data[i]->display();
    }

    /**
     * @brief Returns the DataModelType Enum associated with data
     * 
     * @tparam DataModel 
     * @tparam Stream 
     */
    template<typename DataModel, typename Stream>
    DataModelType FinDataFrame<DataModel, Stream>::get_model_type()
        { return data[0]->model_type(); }


}