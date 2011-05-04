/** This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Fractal Library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ISPREV_H
#define ISPREV_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

/*! \class IsPrev
  * \brief Check if previous to current node have specified name
  *
  * Argument is a string node name.
  * Result is true or false.
  */
class IsPrev : public Function
{
public:
    IsPrev();

    virtual const GVariant& operator()(Patterns::Context &context, FunctionArgs &args);
};

}}} // namespaces

#endif // ISPREV_H
