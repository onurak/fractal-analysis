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

#include "AB.h"

using namespace FL::Markers;
using namespace FL::Exceptions;
using namespace FL::Trees;
using namespace FL::Patterns;

AB::AB()
{
}

AB::~AB()
{

}

FL::ParseResult AB::analyze(
        const TimeSeries &ts, Forest &forest, PatternsSet &patterns)
{
    ParseResult result;

    try
    {
        if (ts.values().size() < 2)
            throw EAnalyze(E_INVALID_INPUT);

        Tree *tree = new Tree();
        const int up   = IDGenerator::idOf("a");
        const int down = IDGenerator::idOf("b");

        for (unsigned int i = 0; i < ts.values().size()-1; i += 1)
        {
            const int &id =  (ts.values()[i] <= ts.values()[i+1])  ?  up  :  down;
            tree->add(new Node(NULL, id, i, i+1, 0));
        }
        forest.push_back(tree);
        result.treesAdded = 1;
        result.nodesAdded = (ts.values().size() + 1) / 2;
    }
    catch (const EAnalyze &e)
    {
        m_lastError = e;
    }
    return result;
}
