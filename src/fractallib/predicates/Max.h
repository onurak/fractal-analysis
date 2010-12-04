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
#ifndef MAX_H
#define MAX_H

#include "../Predicates.h"

namespace FL { namespace Predicates {

/*! \class Max
  * \brief Maximum of the time series segment
  *
  * Arguments:
  *  - any number of ParseTreeNode*
  * Returns: maximum of time series segment
  *
  */
class Max : public Predicate
{
public:

    //! Default constructor
    Max()
    {
        m_name = "Max";
    }

    //! Call operator
    virtual GVariant& operator()(Patterns::CheckContext& context, PredicateArgs& args)
            throw (EPredicate)
    {
        if (args.size() == 0)
            throw EPredicate(m_name, INVALID_ARGS);

        std::vector<double> vec;
        context.ts->getTSByIndex(vec, 0);

        double result = -10e10; // minimum!
        FL::ParseTreeNode *node;

        std::vector<GVariant*>::const_iterator arg;
        for_each_(arg, args)
        {
            node = **arg;
            if (node == NULL)
                throw EPredicate(m_name, INVALID_NODE);
            for (int i = node->tsBegin; i <= node->tsEnd + 1; i++)
                if (vec[i] > result)
                    result = vec[i];
        }
        return m_result = result;
    }
};

}} // namespaces

#endif // MAX_H
