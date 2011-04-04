#ifndef ABSTRACT_MARKER_H
#define ABSTRACT_MARKER_H

#include "../Common.h"
#include "../TimeSeries.h"
#include "../trees/Forest.h"
#include "../patterns/Pattern.h"
#include "../exceptions/EException.h"

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
                      Patterns::PatternsSet &patterns) = 0;

    const Exceptions::EAnalyze& lastError() const { return m_lastError; }
    bool wasOk() const { return m_lastError.id() == E_OK; }
protected:
    Exceptions::EAnalyze m_lastError;
};

}} // namespaces

#endif // ABSTRACT_H
