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


#ifndef NEAR_H
#define NEAR_H

#include "../Predicates.h"

namespace FL { namespace Predicates {

/*! \class Near
  * \brief Check if abs difference of two values is less than third parameter
  *
  * Arguments:
  *  - args[0] -- first float value
  *  - args[1] -- second float value
  *  - args[2] -- maximal difference of args[0] and args[1]
  */
class Near : public Predicate
{
public:
    //! Default constructor
    Near()
    {
        m_name = "Near";
    }

    //! Call operator
    virtual const GVariant& operator()(Patterns::CheckContext& context, PredicateArgs& args)
            throw (EPredicate)
    {
        if (args.size() != 3 ||
            !args[0]->canCastTo(G_VAR_DOUBLE) ||
            !args[1]->canCastTo(G_VAR_DOUBLE) ||
            !args[2]->canCastTo(G_VAR_DOUBLE))
            throw EPredicate(m_name, INVALID_ARGS);

        double v1 = *args[0];
        double v2 = *args[1];
        double diff = *args[2];

        return m_result = bool( fabs(v1-v2) <= diff );
    }
};

}} // namespaces


#endif // NEAR_H
