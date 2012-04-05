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

#ifndef FRACTAL_INCREMENTAL_H
#define FRACTAL_INCREMENTAL_H

#include "AbstractMarker.h"

namespace FL { namespace Markers {


class ABIncremental : public AbstractMarker
{
public:
    ABIncremental();

    virtual ~ABIncremental();

    virtual ParseResult analyze(
                      const TimeSeries &ts,
                      Trees::Forest &forest,
                      Patterns::Matcher &matcher,
                      Patterns::PatternsSet &patterns);
protected:
    void growTree(
        const TimeSeries &ts, Trees::Tree &tree,
        Patterns::Matcher &matcher, int begin, int end,
        Patterns::PatternsSet &patterns,
        Trees::Node oldLastSymbol);

    bool growLayer(
        const TimeSeries &ts, Trees::Tree &tree,
        Patterns::Matcher &matcher, int begin, int end, Patterns::PatternsSet &patterns);

    bool match(Patterns::Matcher &matcher, Patterns::Context &context, Patterns::PatternsSet &patterns);

    void insertNode(Patterns::Context &context, Patterns::CheckInfo &ci,
                    Patterns::PatternsSet &patterns);

    bool recheckNodeGuard(Patterns::Context &context, Trees::Node *node);

    //! Update and check that node's parents (direct and indirect) are still valid
    /**
      * @returns NULL if ok or first parent that is invalid
      */
    Trees::Node* recheckNodeParents(Patterns::Context &context, Trees::Node *node);

    void eraseNode(Trees::Tree &tree, Trees::Node *node);
};

}} // namespaces

#endif // FRACTAL_INCREMENTAL_H
