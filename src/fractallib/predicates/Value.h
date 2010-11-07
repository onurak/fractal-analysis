/* Author: Vladimir Belikov
 *
 * This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Fractal Libray.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VALUE_H
#define VALUE_H

#include "../Predicates.h"

namespace FL { namespace Predicates {

/*! \class Value
  * \brief Value of pattern as sum of values of it's segments
  *
  * Arguments:
  *  - args[0] .. args[n] -- ParseTreeNode*
  *  - args[n+1] -- ('min', 'max') (must be specified only if indexed number is wildcard *)
  *     get 'min' or 'max' value among few founded nodes if wildcard * specified
  */
class Value : public Predicate
{
public:
    //! Default constructor
    Value()
    {
        m_name = "Value";
    }

    //! Call operator
    virtual const GVariant& operator()(Patterns::CheckContext& context, PredicateArgs& args)
    {
        if (args.size() == 0)
            return m_result = 0.0;

        enum
        {
            MIN, MAX
        } multiFuncType;

        if (args.size() > 1)
        {
            const std::string &minmax = *args[args.size()-1];
            if (minmax == "min")
                multiFuncType = MIN;
            else if (minmax == "max")
                multiFuncType = MIN;
            else
            {
                GError(GCritical, m_name, 0, EInvalidTermArgs);
                return m_result = 0.0;
            }
        }

        std::vector<double> vec;
        context.ts->getTSByIndex(vec, 0);

        double min = 10e10, max = -10e10, nodeValue;
        double result = multiFuncType == MAX ? -10e10 : 10e10;
        std::vector<GVariant*>::const_iterator arg;
        for (arg = args.begin(); arg != args.end() - 1; ++arg)
        {
            ParseTreeNode *node = **arg;

            // find minimum and maximum of node's time series segment
            for (int i = node->tsBegin; i <= node->tsEnd + 1; i++)
            {
                if (vec[i] < min)
                    min = vec[i];
                if (vec[i] > max)
                    max = vec[i];
            }

            // calc node value
            nodeValue = (max - min);
            if (arg == args.begin())
                result = nodeValue;
            else if (multiFuncType == MAX)
            {
                if (nodeValue > result)
                    result = nodeValue;
            }
            else
            {
                if (nodeValue < result)
                    result = nodeValue;
            }
        }

        return m_result = result;
    }
};


}} // namespaces

#endif // VALUE_H
