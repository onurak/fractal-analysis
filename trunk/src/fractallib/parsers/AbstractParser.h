#ifndef ABSTRACT_PARSER_H
#define ABSTRACT_PARSER_H

#include "../Common.h"
#include "../TimeSeries.h"
#include "../trees/Forest.h"
#include "../patterns/Pattern.h"
#include "../exceptions/EException.h"

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
protected:
    Exceptions::EAnalyze m_lastError;
};

}} // namespaces

#endif // ABSTRACT_H
