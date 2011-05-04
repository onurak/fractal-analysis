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

#ifndef MIN_H
#define MIN_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

/*! \class Min
  * \brief Get minimum of time series in the set of parse tree nodes
  *
  * Arguments are any number of FL::Trees::Node*.
  * It finds minimum of time series among this nodes.
  * Throw EArguments exception if arguments are not valid.
  */
class Min : public Function
{
public:
    Min();

    virtual const GVariant& operator()(Patterns::Context &context, FunctionArgs &args);
};

}}} // namespaces

#endif // MIN_H
