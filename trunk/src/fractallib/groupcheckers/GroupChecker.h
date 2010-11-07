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

#ifndef GROUPCHECKER_H
#define GROUPCHECKER_H

#include "../Pattern.h"

namespace FL { namespace Patterns {

/*! \class GroupChecker
  * \brief Interface for group check for collection of patterns
  * While FL::Pattern::check(Patterns::CheckContext &context) check if only single
  * pattren can be applied, this method check it for all patterns in collection.
  * Children of this class can implement fast group checking algorithms like
  * GLR or CYK parsers which takes about O(n^3).
  * Default FL::GroupCheckerUniversal is checking each pattern with it own
  * checker so it very slow but independent from pattern structure.
  */
class GroupChecker
{
public:
    //! Default constructor
    GroupChecker() {}

    //! Virtual destructor
    virtual ~GroupChecker() {}

    //! Check if there is pattern in collection applicable for current context
    /*! \param patterns Pattern collection to check
      * \param context Context to check
      * \param applicablePatterns All applicable patterns will be returned in
      *        this colelction (it will be cleared)
      */
    virtual void check(FL::PatternCollection &patterns,
                       CheckContext &context,
                       std::vector<FL::Pattern*> &applicablePatterns) = 0;
};

}} // namespace

#endif // GROUPCHECKER_H
