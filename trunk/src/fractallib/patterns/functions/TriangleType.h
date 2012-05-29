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

#ifndef TRIANGLETYPE_H
#define TRIANGLETYPE_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

/*! \class TriangleType
  * \brief Return type of Elliott Waves triangle (0 - parallel, 1 - closing, 2 - expanding)
  *
  * Arguments are 8 numbers:
  *     (x1, y1, x2, y2) - coordinates of first line
  *     (x3, y3, x4, y4) - coordinates of second line
  */



class TriangleType : public Function
{
public:
    TriangleType();

    //! Main function of class
    virtual const GVariant& operator()(Patterns::Context& context, FunctionArgs& args);

};

}}} // namespaces

#endif // TRIANGLETYPE_H
