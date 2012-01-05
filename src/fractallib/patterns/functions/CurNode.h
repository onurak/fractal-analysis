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

#ifndef NODE_H
#define NODE_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

/*! \class Node
  * \brief Return pointer to current indexed subnode of currently analyzing node.
  *
  * It don't want any arguments.
  * Return pointer to FL::Trees::Node*.
  */
class CurNode : public Function
{
public:
    CurNode();

    //! Main function of class
    virtual const GVariant& operator()(Patterns::Context& context, FunctionArgs& args);
};

}}} // namespaces

#endif // NODE_H
