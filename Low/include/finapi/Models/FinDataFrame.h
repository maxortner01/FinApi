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

    template<typename ModelStruct, typename ConnType>
    class FinDataFrame
    {

        std::vector<ModelStruct*> data;
        typename std::vector<ModelStruct*>::iterator it;

    public:

        /* CONSTRUCTORS */

        FinDataFrame(ConnType&);

        /* DESTRUCTOR */

        ~FinDataFrame();

        /* METHODS */

        bool get_next_model(ModelStruct*&);

        void sort_data(std::string, bool=true);

    };

    /* CONSTRUCTORS */

    template<typename ModelStruct, typename ConnType>
    FinDataFrame<ModelStruct, ConnType>::FinDataFrame(ConnType& connection)
    {
        deserialize(data, connection);
        it = data.begin();
    }

    /* DESTRUCTOR */

    template<typename ModelStruct, typename ConnType>
    FinDataFrame<ModelStruct, ConnType>::~FinDataFrame()
        { clean_list(data); }

    /* METHODS */
    template<typename ModelStruct, typename ConnType>
    bool FinDataFrame<ModelStruct, ConnType>::get_next_model(ModelStruct*& dat)
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

    template<typename ModelStruct, typename ConnType>
    void FinDataFrame<ModelStruct, ConnType>::sort_data(std::string field, bool ascd)
    { 
        ModelComparator<ModelStruct> comparator;
        ModelComparator<ModelStruct>::field_to_compare = field;
        ModelComparator<ModelStruct>::ascending = ascd;
        sort(data.begin(), data.end(), &ModelComparator<ModelStruct>::compare);
    }

}