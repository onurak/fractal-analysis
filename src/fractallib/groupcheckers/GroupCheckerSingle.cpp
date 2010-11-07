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

#include "GroupCheckerSingle.h"

using namespace FL;
using namespace FL::Patterns;

void GroupCheckerSingle::check(PatternCollection &patterns,
                               CheckContext &context,
                               std::vector<Pattern*> &applicablePatterns)
{
    applicablePatterns.clear();
    PatternCollection::Iterator itPattern;
    for_each_(itPattern, patterns)
    {
        FL::Pattern *pattern = *itPattern;
        if (pattern->check(context) == Pattern::crOK)
        {
            applicablePatterns.push_back(pattern);
            break;
        }
    }
}
