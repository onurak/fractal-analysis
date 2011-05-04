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

#ifndef FUNCTIONFACTORY_H
#define FUNCTIONFACTORY_H

#include <map>
#include <string>
#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

class FunctionFactory
{
public:
    static Function* get(const std::string &name);
    static void registerFunction(Function *f);
    static void registerAll();
private:
    static std::map<std::string, Function*> m_functions;
};

}}} // namespaces

#endif // FUNCTIONFACTORY_H
