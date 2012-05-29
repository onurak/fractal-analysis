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

#include "Forecaster.h"
#include "../patterns/Context.h"
#include "../patterns/Pattern.h"
#include <math.h>

using namespace FL;
using namespace FL::Trees;
using namespace FL::Forecasting;
using namespace FL::Patterns;

Forecaster::Forecaster()
{
}

void Forecaster::forecast(TimeSeries &ts, Forest &forest, Forecast &forecast)
{
    forecast.clear();
    if (ts.size() < 2)
        return;

    Forest::ConstIterator tree;
    forall(tree, forest)
        forecastForTree(ts, **tree, forecast);
}

/** At line specified by two points (x1,y1) and (x2,y2)
  * find (x,y) coordinates of point that is at @length distance from (x2,y2)
  */
inline void posOnLine(
        double x1, double y1, double x2, double y2,
        double seglength, double length,
        double &x, double &y)
{
    x = x2 + length * (x2-x1) / seglength;
    y = y2 + length * (y2-y1) / seglength;

    x -= x2;
    y -= y2;
}


void Forecaster::forecastForTree(TimeSeries &ts, Tree &tree, Forecast &forecast)
{
    forecastMaxLength(ts, tree, forecast);
    //forecastUnfinished(ts, tree, forecast);
}

void Forecaster::forecastUnfinished(TimeSeries &ts, Trees::Tree &tree, Forecast &forecast)
{
    // Create dummy analysis context
//    FL::Trees::Tree *tempTree = tree.copy();

//    FL::Patterns::Context context;
//    context.setParseTree(tempTree);
//    context.setOutputTree(tempTree);
//    context.setTimeSeries(&ts);

//    double x1 = ts.time(-2), x2 = ts.time(-1);
//    double y1 = ts.value(-2), y2 = ts.value(-1);

//    // Calc analysis area params
//    double segLength = sqrt((x1-x2)*(x1-x2)  +  (y1-y2)*(y1-y2));
//    double xLength = cos(segLength);
//    double yLength = sin(segLength);

//    const int up   = IDGenerator::idOf("a");
//    const int down = IDGenerator::idOf("b");

//    for (double x = -xLength; x < xLength; x += xLength * 0.2)
//    {
//        for (double y = -yLength; y < yLength; y += yLength * 0.2)
//        {
//            // Add new value to time series, update tree structure
//            ts.append(x, y);

//            const int &id =  y > ts.value(-2)  ?  up  :  down;
//            tree->add(new Node(NULL, id, ts.size()-2, ts.size()-1, 0, nsFinished));


//            for (int lvl = 1; lvl < tree.levelCount(); ++lvl)
//            {
//                Node *lastNode = tree.nodesByLevel(lvl).getLastNode();
//                if (lastNode == NULL || lastNode->isFinished())
//                    continue;


//            }

//            ts.remove(ts.size()-1);
//        }
//    }

//    delete tempTree;
}


void Forecaster::forecastMaxLength(TimeSeries &ts, Tree &tree, Forecast &forecast)
{
    // Create dummy analysis context
    FL::Patterns::Context context;
    context.setParseTree(&tree);
    context.setOutputTree(&tree);
    context.setTimeSeries(&ts);

    // Analysis last node at each level of tree
    for (int lvl = 1; lvl < tree.levelCount(); ++lvl)
    {
        Node *lastNode = tree.nodesByLevel(lvl).getLastNode();
        if (lastNode == NULL || lastNode->end() < ts.size()-1)
            continue;

        double x1 = ts.time(lastNode->begin()), x2 = ts.time(lastNode->end());
        double y1 = ts.value(lastNode->begin()), y2 = ts.value(lastNode->end());

        // Calc analysis area params
        double segLength = sqrt((x1-x2)*(x1-x2)  +  (y1-y2)*(y1-y2));

        // Analyse limits of guard on specified area
        bool somethingCatched = false;
        double minLength = 10e10, maxLength = 0.0;

        for (double length = 0; length < segLength * 2; length += segLength * 0.05)
        {
            ts.value(-1) += length;

            bool catched = checkGuard(context, lastNode);
            if (catched)
            {
                if (minLength > length)
                    minLength = length;
                if (maxLength < length)
                    maxLength = length;
                somethingCatched = true;
            }

            ts.value(-1) -= length;
        }

        // Find out min/max duration and value from min/max length
        if (somethingCatched && maxLength != minLength)
        {
            ForecastItem fi;
            posOnLine(x1, y1, x2, y2, segLength, minLength, fi.minDuration, fi.minValue);
            posOnLine(x1, y1, x2, y2, segLength, maxLength, fi.maxDuration, fi.maxValue);

            fi.pos = ts.size()-1;
            fi.tree = &tree;
            fi.node = lastNode;
            forecast.push_back(fi);
        }

    }
}


bool Forecaster::checkGuard(Context &context, Trees::Node *node)
{
    // Find pattern with the same id
    Pattern* origPattern = PatternsRegistry::patternById(node->id());
    if (origPattern == NULL)
        return false;

    // Create fake context for parent analysis
    context.setCandidateNode(node);

    // Set roots of fake context to
    // 1. First all nodes of the same level as current node that are BEFORE current node;
    // 2. Then children of current node.
    // This roots are just the same as they were when current node was created.
    Layer rootsLayer;

    // 1.
    const Layer& nodeLevelNodes =
            context.outputTree().nodesByLevel(node->level());

    Layer::ConstIterator iNode;
    forall(iNode, nodeLevelNodes)
    {
        if (*iNode == node) break;
        rootsLayer.push_back(*iNode);
    }

    // 2.
    forall(iNode, node->children())
        rootsLayer.push_back(*iNode);

    context.setRoots(rootsLayer);

    // Build last parsed sequence as children of current parent
    context.buildLastParsed(
                node->origSequence(),
                node->children().size(),
                &node->children());

    // Check the guard
    return origPattern->guard()->check(context);
}
