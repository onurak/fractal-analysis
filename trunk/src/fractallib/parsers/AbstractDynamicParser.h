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

#ifndef ABSTRACTDYNAMICPARSER_H
#define ABSTRACTDYNAMICPARSER_H

#include "AbstractParser.h"
#include "../Forecast.h"

namespace FL { namespace Parsers {

/*! \class AbstractDynamicParser
  * \brief Base class for all dynamic parsers
  */
class AbstractDynamicParser
{
public:
    AbstractDynamicParser();
    virtual ~AbstractDynamicParser();

    //! Main analysis method
    /*! Last value of time series must be just added.
      */
    virtual ParseResult analyze(
        const TimeSeries &ts,
        Trees::Forest &forest,
        Patterns::PatternsSet &patterns,
        Trees::MetricsSet &metrics,
        Patterns::Matcher &matcher,
        Forecast &forecast) = 0;

    //! Get last error
    const Exceptions::EAnalyze& lastError() const { return m_lastError; }

    //! Is there any errors during last analysis
    bool wasOk() const { return m_lastError.id() == E_OK; }

    //! Progress update notification
    NotifyProgress onProgress;
protected:
    Exceptions::EAnalyze m_lastError;
    //virtual const char* descriptionOf(int errNo);
};

}} // namespaces

#endif // ABSTRACTDYNAMICPARSER_H
