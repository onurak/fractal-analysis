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

void TSMarkerSimple::analyse(TimeSeries *ts, ParseTreeSet &trees)
{
    // check params
    if (!ts || ts->size() == 0)
    {
        GError(GCritical, "", 0, "Time series is NULL or have invalid dimension size");
        return;
    }
    std::vector<double> v;
    ts->getTSByIndex(v, 0);
    if (v.size() < 2)
    {
        GError(GCritical, "", 0, "Time series have less than 2 values");
    }
    ParseTree* result = new ParseTree(&trees);
    std::vector<double>::iterator p;

    // analyse
    int index = 0;
    std::string name;
    for (p = v.begin(); p+1 != v.end(); p++, index++)
    {
        name = (*p <= *(p+1)) ? "a" : "b";
        ParseTreeNode *newNode = new ParseTreeNode(name, NULL, 0, index, index);
        result->addNode( newNode );

    }

    trees.add(result);
}
