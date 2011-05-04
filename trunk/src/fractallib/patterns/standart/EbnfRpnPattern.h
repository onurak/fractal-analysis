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

#ifndef EBNFRPNPATTERN_H
#define EBNFRPNPATTERN_H

#include "../Pattern.h"
#include "GuardRpn.h"
#include "DescriptionEbnf.h"

namespace FL { namespace Patterns { namespace Standart {

class EbnfRpnPattern : public PatternConstructor
{
public:
    EbnfRpnPattern() : PatternConstructor() {}
    virtual Guard* createGuard(Description &d) const;
    virtual Description* createDescription() const;
};

}}} // namespaces

#endif // EBNFRPNPATTERN_H
