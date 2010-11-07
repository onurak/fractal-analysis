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

#ifndef ARGCNT_H
#define ARGCNT_H

#include "../Predicates.h"

namespace FL { namespace Predicates {

/*! \class ArgCnt
  * \brief Return count of passed arguments (usefull for debug)
  *
  * Arguments:
  *  - any number of any arguments
  */
class ArgCnt : public Predicate
{
public:
    //! Default constructor
    ArgCnt()
    {
        m_name = "ArgCnt";
    }

    //! Call operator
    virtual const GVariant& operator()(Patterns::CheckContext& context, PredicateArgs& args)
    {
        return m_result = double(args.size());
    }
};

}} // namespaces

#endif // ARGCNT_H
