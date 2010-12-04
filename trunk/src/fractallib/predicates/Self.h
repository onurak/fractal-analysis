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

#ifndef SELF_H
#define SELF_H

#include "../Predicates.h"

namespace FL { namespace Predicates {

/*! \class Self
  * \brief Return current recognizing pattern
  *
  * Usage exmaple:
  * \code
  *   p = a b a @ value(self()) > 100;
  * \endcode
  * Arguments:
  *  - no args
  */
class Self : public Predicate
{
public:
    //! Default constructor
    Self()
    {
        m_name = "Self";
    }

    //! Call operator
    virtual const GVariant& operator()(Patterns::CheckContext& context, PredicateArgs& args)
            throw (EPredicate)
    {
        if (args.size() != 0)
            throw EPredicate(m_name, INVALID_ARGS);
        return m_result = context.candidateNode;
    }
};


} } // namespaces

#endif // SELF_H
