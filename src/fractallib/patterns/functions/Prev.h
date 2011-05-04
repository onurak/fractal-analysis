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

#ifndef PREV_H
#define PREV_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

/*! \class Prev
  * \brief Return pointer to one of the previous root node.
  *
  * If no arguments specified, than it returns pointer to previous root node
  * or NULL if it is first root node.
  *
  * Optional string argument "samelevel" could be passed to indicate that root
  * must be at the same level. You also could specify "nosamelevel" to indicate
  * default behaviour.
  *
  * Return pointer to specified FL::Trees::Node* or NULL if there is no such node.
  */
class Prev : public Function
{
public:
    Prev();

    //! Main function of class
    virtual const GVariant& operator()(Patterns::Context& context, FunctionArgs& args);
};

}}} // namespaces
#endif // PREV_H
