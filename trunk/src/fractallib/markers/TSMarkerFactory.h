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

#ifndef TSMARKERFACTORY_H
#define TSMARKERFACTORY_H

#include "TSMarker.h"
#include "TSMarkerSimple.h"
#include "TSMarkerAxiomatic.h"

namespace FL {

class TSMarkerFactory
{
public:
    static TSMarker* create(std::string name)
    {
        if (name == "abmarker" || name == "TSMarkerSimple")
            return new TSMarkerSimple();
        else
            return NULL;
    }

    static bool create(std::string name, TSMarker **out, bool wantDelete = true)
    {
        if (out == NULL)
            return false;
        TSMarker *tmp = create(name);
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

} // namespace

#endif // TSMARKERFACTORY_H
