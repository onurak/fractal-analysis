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

#ifndef STARTTIME_H
#define STARTTIME_H

#include "../Predicates.h"

namespace FL { namespace Predicates {

/*! \class StartTime
  * \brief Return time of begining of node
  *
  * Arguments:
  *  - args[0] - ParsTreeNode*
  * Returns: start time of node
  *
  */
class StartTime : public Predicate
{
public:

    //! Default constructor
    StartTime()
    {
        m_name = "StartTime";
    }

    //! Call operator
    virtual GVariant& operator()(Patterns::CheckContext& context, PredicateArgs& args)
            throw (EPredicate)
    {
        if (args.size() != 1)
            throw EPredicate(m_name, INVALID_ARGS);
        ParseTreeNode *node = *args[0];

        return m_result = node->tsBegin;
    }
};

}} // namespaces


#endif // STARTTIME_H
