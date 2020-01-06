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

#include "../Core/SysInclude.h"
#include "../Core/Models.h"

namespace finapi
{
namespace modeler
{
    /**
     * @brief Provides a useful interface to interacte with data
     *        after it has been deserialized into its Data Model
     * 
     *        - REQUIRED MODEL OVERLOADS: SEE EODADJ               
     * 
     * @tparam _DataModel 
     * @tparam _Stream 
     */
    template<typename _DataModel, typename _Stream>
    class FinDataFrame
    {
        std::vector<_DataModel*> data;
        typename std::vector<_DataModel*>::iterator it;

    public:

        /* CONSTRUCTORS */

        FinDataFrame(_Stream&);    

        /* DESTRUCTOR */

        ~FinDataFrame();

        /* METHODS */

        bool get_next_model(_DataModel*&);

        void sort_data(std::string, bool=true);

        void append(_Stream*&);

        void display_head();

        void display_tail();

        DataModelType get_model_type();
    };
}
}