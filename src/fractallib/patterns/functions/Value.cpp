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

#include "Value.h"
#include "../../TimeSeries.h"
#include "../../trees/Tree.h"

using namespace FL::Patterns::Functions;

Value::Value()
{
    m_name = "Value";
}


const GVariant& Value::operator()(Patterns::Context& context, FunctionArgs& args)
{
    if (args.size() == 0)
        throw FL::Exceptions::EArguments(m_name, -1, 0);
    const FL::TimeSeries *ts = context.timeSeries();

    // Get min/max on segment
    checkValidNode(args[0]);
    FL::Trees::Node *node = *(args[0]);
    double vBeg = ts->value(node->begin());
    double vEnd = ts->value(node->end());
    double minimum = std::min(vBeg, vEnd);
    double maximum = std::max(vBeg, vEnd);

    FunctionArgs::iterator arg;
    for (arg = args.begin() + 1; arg != args.end(); ++arg)
    {
        checkValidNode(*arg);
        node = **arg;

        vBeg = ts->value(node->begin());
        vEnd = ts->value(node->end());
        if (vBeg > maximum)
            maximum = vBeg;
        if (vEnd > maximum)
            maximum = vEnd;

        if (vBeg < minimum)
            minimum = vBeg;
        if (vEnd < minimum)
            minimum = vEnd;
    }

    return m_result = (maximum - minimum);
}
