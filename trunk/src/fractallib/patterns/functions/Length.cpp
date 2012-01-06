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

#include "Length.h"
#include "../../TimeSeries.h"
#include "../../trees/Tree.h"
#include <cmath>

using namespace FL::Patterns::Functions;

Length::Length()
{
    m_name = "Length";
}


const GVariant& Length::operator()(Patterns::Context& context, FunctionArgs& args)
{
    if (args.size() == 0)
        throw FL::Exceptions::EArguments(m_name, -1, 0);
    const FL::TimeSeries *ts = context.timeSeries();

    // Get min/max on segment
    checkValidNode(args[0]);
    FL::Trees::Node *node = *(args[0]);
    double vBeg = ts->value(node->begin());
    double vEnd = ts->value(node->end());
    double vMin = std::min(vBeg, vEnd);
    double vMax = std::max(vBeg, vEnd);

    double tMin = ts->time(node->begin());
    double tMax = ts->time(node->end());

    FunctionArgs::iterator arg;
    for (arg = args.begin() + 1; arg != args.end(); ++arg)
    {
        checkValidNode(*arg);
        node = **arg;

        vBeg = ts->value(node->begin());
        vEnd = ts->value(node->end());
        if (vBeg > vMax)
            vMax = vBeg;
        if (vEnd > vMax)
            vMax = vEnd;

        if (vBeg < vMin)
            vMin = vBeg;
        if (vEnd < vMin)
            vMin = vEnd;

        if (tMin > ts->time(node->begin()))
            tMin = ts->time(node->begin());

        if (tMax < ts->time(node->end()))
            tMax = ts->time(node->end());
    }

    return m_result = sqrt((vMax-vMin)*(vMax-vMin)  +  (tMax-tMin)*(tMax-tMin));
}
