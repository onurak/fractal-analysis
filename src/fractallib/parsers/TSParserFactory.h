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

#ifndef TSPARSERFACTORY_H
#define TSPARSERFACTORY_H

#include "TSParser.h"
#include "TSParserSimple.h"
#include "TSParserIdeal.h"

namespace FL {

/*! \class TSParserFactory
  * \brief Class Factory for time series parsers
  */
class TSParserFactory
{
public:
    static FL::TSParser* create(std::string name)
    {
        if (name == "simple" || name == "TSParserSimple")
            return new TSParserSimple();
        else
        if (name == "ideal" || name == "TSParserIdeal")
            return new TSParserIdeal();
        else
            return NULL;
    }

    static bool create(std::string name, TSParser **out, bool wantDelete = true)
    {
        if (out == NULL)
            return false;
        TSParser *tmp = create(name);
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

#endif // TSPARSERFACTORY_H
