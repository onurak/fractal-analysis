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

#ifndef GUARD_H
#define GUARD_H

#include "Description.h"
#include "functions/Function.h"

namespace FL { namespace Patterns {

class Context;

class Guard
{
public:
    Guard(const FL::Patterns::Description &description);
    virtual ~Guard() {}

    FL::Exceptions::EParsing compile(const std::string &s);
    virtual FL::Exceptions::EParsing compile(Compilers::Input &i) = 0;

    virtual bool check(Context &c) = 0;
protected:
    const FL::Patterns::Description &m_description;
};

}} // namespaces

#endif // GUARD_H
