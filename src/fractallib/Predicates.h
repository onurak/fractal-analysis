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

#ifndef PREDICATES_H
#define PREDICATES_H

#include <math.h>
#include "PatternDescription.h"
#include "Exceptions.h"
#include "G/GVariant.h"

namespace FL {

/*! \namespace FL::Predicates
  * \brief Contains predicates available for patterns
  */
namespace Predicates {

/*! \addtogroup FLAlgorithm
  */
/*@{*/

/*! \struct IndexedName
  * \brief Name and it's index
  */
struct IndexedName
{
    IndexedName(const std::string &name = "", int no = -2) : name(name), no(no) {}
    std::string name;
    int no;
};

//! Predicate's arguments
typedef std::vector<GVariant*> PredicateArgs;

/*! \class Predicate
  * \brief Predicate prototype.
  */
class Predicate
{
protected:
    //! Predicate name. Every child must set it in constructor
    std::string m_name;

    //! Internal result storage
    GVariant m_result;
public:
    //! Main function of predicate. Children must override it
    virtual const GVariant& operator()(Patterns::CheckContext&, PredicateArgs&) = 0;

    //! Getter of predicate's name
    std::string name() const { return m_name; }
};

/*@}*/

} // namespace
} // namespace

#endif // PREDICATES_H
