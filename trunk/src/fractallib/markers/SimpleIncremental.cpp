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

#include "SimpleIncremental.h"

using namespace FL::Markers;
using namespace FL::Exceptions;
using namespace FL::Trees;
using namespace FL::Patterns;


SimpleIncremental::SimpleIncremental()
{
}

SimpleIncremental::~SimpleIncremental()
{
}

FL::ParseResult SimpleIncremental::analyze(
    const TimeSeries &ts, Trees::Forest &forest, Patterns::PatternsSet &patterns)
{
    ParseResult result;
    return result;

//    try
//    {
//        if (ts.values().size() < 2)
//            throw EAnalyze(E_INVALID_INPUT);
//        if (forest.size() == 0)
//            throw EAnalyze(E_INVALID_INPUT);

//        const int up   = IDGenerator::idOf("up");
//        const int down = IDGenerator::idOf("down");
//        int begin, end;

//        Forest::Iterator itTree;
//        forall(itTree, forest)
//        {
//            // Markup last non-marked up segment of time series with "a" and "b"
//            Tree *tree = *itTree;
//            tree->validateStructure();
//            tree->getSegment(begin, end);
//            begin = end;
//            end = ts.values().size()-1;
//            Layer& roots = const_cast<Layer&>(tree->nodesByLevel(0));
//            roots.sortByTime();

//            for (int i = begin; i < end; i += 1)
//            {
//                const int &id =  (ts.values()[i] <= ts.values()[i+1])  ?  up  :  down;

//                // Special case if this node same as previous
//                if (id == roots.back()->id())
//                {
//                    Node *node = roots.back();
//                    //int oldEnd = node->end();
//                    do
//                    {
//                        node->setEnd(i+1);
//                        node = node->parent();
//                    } while (node);
//                }
//                else
//                    tree->add(new Node(NULL, id, i, i+1, 0, nsFloating));
//            }
//        }
//    }
//    catch (const EAnalyze &e)
//    {
//        m_lastError = e;
//    }

//    return result;
}

