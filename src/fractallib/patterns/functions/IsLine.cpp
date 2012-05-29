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

#include "IsLine.h"
#include "../../trees/Tree.h"
#include <cmath>

using namespace FL::Patterns::Functions;

IsLine::IsLine()
{
    m_name = "IsLine";
}


const GVariant& IsLine::operator()(Patterns::Context& context, FunctionArgs& args)
{
    context.unused();

    if (args.size() < 6)
        throw FL::Exceptions::EArguments(m_name, -6, 0);

    for (size_t i = 0; i < args.size(); ++i)
        if (!args[i]->canCastTo(G_VAR_DOUBLE))
            throw FL::Exceptions::EArguments(m_name, GVariant::typeName(G_VAR_DOUBLE), args[i]->typeName(), i);

    // Last parameter is acceptable error
    size_t argsCount = args.size();
    double eps = 0.0;
    if (args.size() % 2 == 1)
    {
        eps = double(*args.back());
        argsCount--;
    }

    /* Find line params (k,b) from first two nodes: y(x) = k*x + b.
       Solve system for (k,b):
          { y1 = k*x1 + b,
          { y2 = k*x2 + b
       Solution:
            k = (y1 - y2) / (x1 - x2)
            b = y2 - k*x2
     */
    double x1 = double(*args[0]);
    double y1 = double(*args[1]);
    double x2 = double(*args[2]);
    double y2 = double(*args[3]);

    double k = (y1 - y2) / (x1 - x2);
    double b = y2 - k*x2;

    // Check if all other points are close enoght to the same line
    for (size_t i = 4; i < argsCount; ++i)
    {
        double x = double(*args[i]);
        double y = double(*args[++i]);
        if (fabs(k*x + b - y) > eps)
            return m_result = false;
    }


    return m_result = true;
}
