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

#ifndef ATONELINE_H
#define ATONELINE_H

#include "../Predicates.h"

namespace FL { namespace Predicates {

/*! \class AtOneLine
  * \brief Return true if few points are on the same line (with given tolerance)
  *
  * Arguments:
  *  - args[0], ..., args[n] where n >= 6 - each value (args[i], args[i+1]), i = 2k+1,
  *    is a pair of point coordinates (x, y). There are must be a least 3 points (6 values)
  *  - args[n+1] - tolerance
  * Returns: true if first n argument values are on the same line with given tolerance
  *
  */
class AtOneLine : public Predicate
{
public:

    //! Default constructor
    AtOneLine()
    {
        m_name = "AtOneLine";
    }

    //! Call operator
    virtual GVariant& operator()(Patterns::CheckContext& context, PredicateArgs& args)
            throw (EPredicate)
    {
        if (args.size() < 7)
            throw EPredicate(m_name, INVALID_ARGS);
        for (int i = 0; i < int(args.size()); ++i)
            if (!args[i]->canCastTo(G_VAR_DOUBLE))
                throw EPredicate(m_name, INVALID_ARGS);

        double x1 = *args[0];
        double y1 = *args[1];
        double x2 = *args[2];
        double y2 = *args[3];

        /* Solving system:
         * {y1 = k*x1 + b
         * {y2 = k*x2 + b
         */
        double k = (y1 - y2) / (x1 - x2);
        double b = y1 - k*x1;

        double tol = *args[args.size() - 1];
        // For each point check than y = k*x + b
        for (int i = 4; i < int(args.size())-1; )
        {
            double x = *args[i++];
            double y = *args[i++];
            if (fabs(y - (k*x + b)) >  tol)
                return m_result = false;
        }

        return m_result = true;
    }
};

}} // namespaces


#endif // ATONELINE_H
