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

#ifndef SUM_H
#define SUM_H

#include "../Predicates.h"

namespace FL { namespace Predicates {

/*! \class Sum
  * \brief Return sum of passed numeric arguments
  *
  * If one of arguments is not numeric than return value will be zero
  * Arguments:
  *  - any number of numeric arguments
  */
class Sum : public Predicate
{
public:
    //! Default constructor
    Sum()
    {
        m_name = "Sum";
    }

    //! Call operator
    virtual const GVariant& operator()(Patterns::CheckContext& context, PredicateArgs& args)
    {
        double sum = 0.0;
        for (int i = 0; i < int(args.size()); ++i)
            if ((*args[i]).canCastTo(G_VAR_DOUBLE))
                sum += double(*args[i]);
            else
                return m_result = 0.0;
        return m_result = sum;
    }
};

}} // namespaces

#endif // SUM_H
