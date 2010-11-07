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

#ifndef GROUPCHECKERFACTORY_H
#define GROUPCHECKERFACTORY_H

#include "GroupChecker.h"
#include "GroupCheckerSingle.h"
#include "GroupCheckerUniversal.h"

namespace FL { namespace Patterns {

/*! \class GroupCheckerFactory
  * \brief Class Factory for FL::Patterns::GroupChecker
  */
class GroupCheckerFactory
{
public:
    static GroupChecker* create(std::string name)
    {
        if (name == "single" || name == "GroupCheckerSingle")
            return new GroupCheckerSingle();
        else
        if (name == "universal" || name == "GroupCheckerUniversal")
            return new GroupCheckerUniversal();
        else
            return NULL;
    }

    static bool create(std::string name, GroupChecker **out, bool wantDelete = true)
    {
        if (out == NULL)
            return false;
        GroupChecker *tmp = create(name);
        if (tmp)
        {
            if (wantDelete && *out != NULL)
                delete *out;
            *out = tmp;
            return true;
        }
        return false;
    }
};

}} // namespaces

#endif // GROUPCHECKERFACTORY_H
