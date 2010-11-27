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

#include "TSMarkerSimple.h"

using namespace FL;

TSMarkerSimple::TSMarkerSimple()
    : TSMarker()
{
}

void TSMarkerSimple::analyse(TimeSeries *ts, ParseTreeSet &trees, int tsBegin, int tsEnd)
{
    // check params
    if (!ts || ts->size() == 0)
    {
        GError(GCritical, "", 0, "Time series is NULL or have invalid dimension size");
        return;
    }
    if (tsEnd == -1)
        tsEnd = ts->dimSize(0)-1;
    if (tsBegin < 0 || tsBegin > tsEnd || tsEnd > ts->dimSize(0)-1)
    {
        GError(GCritical, "TSMarkerSimple", 0, "Invalid interval to markup");
        return;
    }

    std::vector<double> v;
    ts->getTSByIndex(v, 0);
    if (v.size() < 2)
    {
        GError(GCritical, "", 0, "Time series have less than 2 values");
        return;
    }
    if (trees.size() == 0)
        trees.add(new ParseTree(&trees));
    ParseTreeSet::Iterator tree;
    for_each_(tree, trees.trees)
    {
        // analyse
        std::string name;
        for (int index = tsBegin; index != tsEnd; index++)
        {
            name = (v[index] <= v[index+1]) ? "a" : "b";
            ParseTreeNode *newNode = new ParseTreeNode(name, NULL, 0, index, index);
            (*tree)->addNode( newNode );
        }
    }
}
