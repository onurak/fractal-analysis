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

#ifndef GROUPCHECKERSINGLE_H
#define GROUPCHECKERSINGLE_H

#include "GroupChecker.h"

namespace FL { namespace Patterns {

/*! \class GroupCheckerSingle
  * \brief Group checker independent from pattern's structure and recognizing only first pattern
  *
  * It use conrete pattern's checker so there is no real group checking algorithm.
  * But it simple, independent from pattern's actual structure and works with
  * with all patterns descriptions that implement they're checkers.
  *
  * It will always return only first one applicable pattern.
  */
class GroupCheckerSingle : public GroupChecker
{
public:
    //! @copydoc void GroupChecker::check(PatternCollection &patterns, CheckContext &context, std::vector<Pattern*> &applicablePatterns)
    virtual void check(PatternCollection &patterns,
                       CheckContext &context,
                       std::vector<Pattern*> &applicablePatterns);
};

}} // namespaces

#endif // GROUPCHECKERSINGLE_H
