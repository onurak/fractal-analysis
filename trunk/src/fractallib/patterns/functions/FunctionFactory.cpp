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

#include "FunctionFactory.h"
#include "Min.h"
#include "Max.h"
#include "Value.h"
#include "AtOneLine.h"
#include "StartTime.h"
#include "EndTime.h"
#include "Prev.h"
#include "Self.h"
#include "IsPrev.h"
#include "Near.h"

using namespace FL::Patterns::Functions;

std::map<std::string, Function*> FunctionFactory::m_functions;

std::string upperCase(const std::string &s)
{
    std::string result;
    for (int i = 0; i < int(s.length()); ++i)
    {
        char c = s[i];
        if (isalpha(c) && islower(c)) c = toupper(c);
        result += c;
    }
    return result;
}


Function* FunctionFactory::get(const std::string &name)
{
    std::map<std::string, Function*>::iterator it;
    it = m_functions.find(upperCase(name));
    if (it == m_functions.end())
        return NULL;
    else
        return (*it).second;
}

void FunctionFactory::registerFunction(Function *f)
{
    if (f != NULL)
    {
        std::string name = upperCase(f->name());
        std::map<std::string, Function*>::iterator it;
        it = m_functions.find(name);
        if (it == m_functions.end())
            m_functions[name] = f;
    }
}

void FunctionFactory::registerAll()
{
    if (get("Max") != NULL) return;
    registerFunction(new Max());
    registerFunction(new Min());
    registerFunction(new Value());
    registerFunction(new AtOneLine());
    registerFunction(new StartTime());
    registerFunction(new EndTime());
    registerFunction(new Prev());
    registerFunction(new Self());
    registerFunction(new IsPrev());
    registerFunction(new Near());
}
