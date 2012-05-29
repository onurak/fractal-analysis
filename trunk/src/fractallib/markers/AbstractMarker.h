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

#ifndef ABSTRACT_MARKER_H
#define ABSTRACT_MARKER_H

#include "../Common.h"
#include "../TimeSeries.h"
#include "../trees/Forest.h"
#include "../patterns/Pattern.h"
#include "../exceptions/EException.h"
#include "../patterns/Matcher.h"
#include "../trees/Metrics.h"

namespace FL
{
    const int E_INVALID_ARGS = 1;
}

namespace FL { namespace Markers {

class AbstractMarker
{
public:
    AbstractMarker();
    virtual ~AbstractMarker();

    virtual ParseResult analyze(
                      const TimeSeries &ts,
                      Trees::Forest &forest,
                      Patterns::Matcher &matcher,
                      Patterns::PatternsSet &patterns,
                      Trees::MetricsSet &metrics) = 0;

    const Exceptions::EAnalyze& lastError() const { return m_lastError; }
    bool wasOk() const { return m_lastError.id() == E_OK; }
protected:
    Exceptions::EAnalyze m_lastError;
};

}} // namespaces

#endif // ABSTRACT_H
