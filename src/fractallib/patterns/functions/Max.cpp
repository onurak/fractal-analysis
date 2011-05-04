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

#include "Max.h"
#include "../../TimeSeries.h"
#include "../../trees/Tree.h"

using namespace FL::Patterns::Functions;

Max::Max()
{
    m_name = "Max";
}


const GVariant& Max::operator()(Patterns::Context& context, FunctionArgs& args)
{
    if (args.size() == 0)
        throw FL::Exceptions::EArguments(m_name, -1, 0);
    const FL::TimeSeries *ts = context.timeSeries();

    // Get most left and most right indices of time series
    int begin = ts->values().size(), end = -1;

    FunctionArgs::iterator arg;
    forall(arg, args)
    {
        checkValidNode(*arg);
        FL::Trees::Node *node = **arg;
        if (node->begin() < begin)
            begin = node->begin();
        if (node->end() > end)
            end = node->end();
    }

    // Get max on interval
    double max = ts->values()[begin];
    for (++begin; begin <= end; ++begin)
        if (ts->values()[begin] > max)
            max = ts->values()[begin];

    return m_result = max;
}
