#ifndef ABSTRACT_PARSER_H
#define ABSTRACT_PARSER_H

#include "../Common.h"
#include "../TimeSeries.h"
#include "../trees/Metrics.h"
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
typedef GDelegate2<bool, FL::ParseResult, FL::Trees::Forest*> NotifyProgress;

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
                      Trees::MetricsSet &metrics,
                      int begin = 0,
                      int end = -1) = 0;

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
