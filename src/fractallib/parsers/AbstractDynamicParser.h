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
