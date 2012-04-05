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

#ifndef ABSTRACT_PARSER_H
#define ABSTRACT_PARSER_H

#include "../Common.h"
#include "../TimeSeries.h"
#include "../trees/Metrics.h"
#include "../patterns/Matcher.h"
#include "../patterns/Pattern.h"
#include "../exceptions/EException.h"


namespace FL
{
    /* Standart errors */
    const int E_EMPTY_TIME_SERIES = 0;
    const int E_EMPTY_PATTERNS    = 1;
    const int E_EMPTY_FOREST      = 2;
    const int E_INVALID_SEGMENT   = 3;
}

namespace FL { namespace Parsers {

/*! \typedef NotifyProgress
  * \brief Parser progress notification
  *
  * It must return true to continue parsing and false for interruption.
  */
typedef GDelegate1<bool, FL::Trees::Forest*> NotifyProgress;

class AbstractParser
{
public:

    //! Default constructor
    AbstractParser();

    //! Destructor
    virtual ~AbstractParser();

    //! Do analysis
    virtual ParseResult analyze(
                      const TimeSeries &ts,
                      Trees::Forest &forest,
                      Patterns::PatternsSet &patterns,
                      Patterns::Matcher &matcher,
                      Trees::MetricsSet &metrics,
                      int tsBegin = 0) = 0;

    //! Get last error
    const Exceptions::EAnalyze& lastError() const { return m_lastError; }

    //! Is there any errors during last analysis
    bool wasOk() const { return m_lastError.id() == E_OK; }

    //! Progress update notification
    NotifyProgress onProgress;
protected:
    Exceptions::EAnalyze m_lastError;
    virtual const char* descriptionOf(int errNo);

protected:
    bool m_interruption;
    FL::ParseResult m_result;
};

}} // namespaces

#endif // ABSTRACT_H
