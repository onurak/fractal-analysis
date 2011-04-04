#ifndef ABSTRACT_PARSER_H
#define ABSTRACT_PARSER_H

#include "../Common.h"
#include "../TimeSeries.h"
#include "../trees/Forest.h"
#include "../patterns/Pattern.h"
#include "../exceptions/EException.h"

namespace FL
{
    /* Standart errors */
    const int E_EMPTY_TIME_SERIES = 0;
    const int E_EMPTY_PATTERNS    = 1;
    const int E_EMPTY_FOREST      = 2;
}

namespace FL { namespace Parsers {

class AbstractParser
{
public:
    AbstractParser();
    virtual ~AbstractParser();

    virtual ParseResult analyze(
                      const TimeSeries &ts,
                      Trees::Forest &forest,
                      Patterns::PatternsSet &patterns) = 0;

    const Exceptions::EAnalyze& lastError() const { return m_lastError; }
    bool wasOk() const { return m_lastError.id() == E_OK; }
protected:
    Exceptions::EAnalyze m_lastError;
    virtual const char* descriptionOf(int errNo);
};

}} // namespaces

#endif // ABSTRACT_H
