/**
 * @file ModelComparator.h
 * 
 * @brief Data Structure providing a commmon interface to compare models
 * 
 * @author   Josh Ortner
 * @date     2019-12-30
 * @version  0.0.1
 * 
 * @copyright Copyright (c) 2019
 */

#pragma once

#include <string>

namespace finapi
{
namespace models
{
    /**
     * @brief Used to compare the data fields of two models
     * 
     *        NOTE: field_to_compare must be initialized before 
     *              methods can be used.
     * 
     */
    template<typename DataModel>
    class ModelComparator
    {
    public:
        static std::string field_to_compare;

        static bool ascending;

        /* METHODS */

        static bool compare(DataModel, DataModel);

        static bool compare_ptr(DataModel*, DataModel*);

    };
}
}