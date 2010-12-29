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

#ifndef ABS_H
#define ABS_H

#include "../Predicates.h"

namespace FL { namespace Predicates {

/*! \class Abs
  * \brief Return absolute value of passed parameter
  *
  * Arguments:
  *  - args[0] - float value
  */
class Abs : public Predicate
{
public:
    //! Default constructor
    Abs()
    {
        m_name = "Abs";
    }

    //! Call operator
    virtual const GVariant& operator()(Patterns::CheckContext& context, PredicateArgs& args)
            throw (EPredicate)
    {
        if (args.size() != 1 || !args[0]->canCastTo(G_VAR_DOUBLE))
            throw EPredicate(m_name, INVALID_ARGS);

        return m_result = fabs(double(*args[0]));
    }
};

}} // namespaces

#endif // ABS_H
