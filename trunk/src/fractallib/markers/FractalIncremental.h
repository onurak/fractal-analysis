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


class FractalIncremental : public AbstractMarker
{
public:
    FractalIncremental();

    virtual ~FractalIncremental();

//    virtual ParseResult analyze(
//                      const TimeSeries &ts,
//                      Trees::Forest &forest,
//                      Patterns::PatternsSet &patterns);
//protected:
//    void growTree(const TimeSeries &ts, Trees::Tree &tree,
//                  Patterns::PatternsSet &patterns, int begin, int end);
//    bool growLayer(const TimeSeries &ts, Trees::Tree &tree,
//                   Patterns::PatternsSet &patterns, int begin, int end);
//    bool applyPattern(Patterns::Pattern &pattern, Patterns::Context &context);
};

}} // namespaces

#endif // FRACTAL_INCREMENTAL_H
