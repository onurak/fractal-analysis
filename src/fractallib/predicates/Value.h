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
private:
    int m_no_min;
    int m_no_max;
public:
    //! Default constructor
    Value()
    {
        m_name = "Value";
        m_no_min = UniqueNamer::id_of_name("min");
        m_no_max = UniqueNamer::id_of_name("max");
    }

    //! Call operator
    virtual const GVariant& operator()(Patterns::CheckContext& context, PredicateArgs& args)
            throw (EPredicate)
    {
        if (args.size() == 0)
            return m_result = 0.0;

        enum
        {
            MIN, MAX
        } multiFuncType;

        if (args.size() == 1)
        {
            std::vector<double> vec;
            context.ts->getTSByIndex(vec, 0);

            double min = 10e10, max = -10e10;
            ParseTreeNode* node = *args[0];

            if (node == NULL)
                throw EPredicate(m_name, INVALID_ARGS);
            // find minimum and maximum of node's time series segment
            for (int i = node->tsBegin; i <= node->tsEnd + 1; i++)
            {
                if (vec[i] < min)
                    min = vec[i];
                if (vec[i] > max)
                    max = vec[i];
            }
            return m_result = max - min;
        }
        else
        {
            int minmax = *args[args.size()-1];
            if (minmax == m_no_min)
                multiFuncType = MIN;
            else if (minmax == m_no_max)
                multiFuncType = MIN;
            else
                throw EPredicate(m_name, INVALID_ARGS);

            std::vector<double> vec;
            context.ts->getTSByIndex(vec, 0);

            double min = 10e10, max = -10e10, nodeValue;
            double result = multiFuncType == MAX ? -10e10 : 10e10;
            std::vector<GVariant*>::const_iterator arg;
            for (arg = args.begin(); arg != args.end()-1; ++arg)
            {
                ParseTreeNode *node = **arg;

                if (node == NULL)
                {
                    GError(GWarning, m_name, 0, EInvalidTermArgs);
                    continue;
                }
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
    }
};


}} // namespaces

#endif // VALUE_H
