/**
 * @file EquityCurve.h
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
#include "../Core/Models.h"

namespace finapi
{
namespace modeler
{
    struct EquityCurveNode
    {
        float total;
        float returns;
        float equity_curve;
        models::TimeStamp time;

        /* CONSTRUCTORS */

        EquityCurveNode();

        EquityCurveNode(const EquityCurveNode&);

        /* OPERATOR OVERLOADS */

        EquityCurveNode& operator = (const EquityCurveNode&);
    };

    /**
     * @brief Takes a holdings vector and produces an EquityCurve data structure
     *        Can be thought of as a matrix indexed by date with the following "cols":
     *          - "total"        : market value of owned securities and cash
     *          - "returns"      : percent change in total
     *          - "equity_curve" : cumulative product of returns normalized so the 
     *                             account initial size is equal to 1.0
     * 
     *        Structure of holdings vector:
     *          - "ticker" 
     *          - "cash"   : total cash
     *          - "comm"   : total commission paid
     *          - "total"  : market value of owned securities plus cash
     *          - "year"    
     *          - "month"  
     *          - "day"    
     */
    class EquityCurve
    {
        
        std::vector<EquityCurveNode*> _data;
        std::vector<EquityCurveNode*>::iterator it;

    public:

        /* CONSTRUCTOR */

        //EquityCurve(std::vector<std::unordered_map<std::string, float>* >);

        EquityCurve(const HoldingsDataFrame<std::string, float>&);

        EquityCurve(const EquityCurve&);

        EquityCurve(EquityCurve&&);

        /* DESTRUCTOR */

        ~EquityCurve();

        /* METHODS */

        void construct_equity_curve(const HoldingsDataFrame<std::string, float>&);

        void clear_data();

        bool to_file(std::string, std::string);

        /* OVERLOADS */

        EquityCurve& operator = (const EquityCurve&);

        EquityCurve& operator = (EquityCurve&&);

    };
}
}