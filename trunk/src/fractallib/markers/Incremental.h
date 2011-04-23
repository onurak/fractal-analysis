#ifndef INCREMENTAL_H
#define INCREMENTAL_H

#include "AbstractMarker.h"

namespace FL { namespace Markers {


class Incremental : public AbstractMarker
{
public:
    Incremental();

    virtual ~Incremental();

    virtual ParseResult analyze(
                      const TimeSeries &ts,
                      Trees::Forest &forest,
                      Patterns::PatternsSet &patterns);
protected:
    void growTree(const TimeSeries &ts, Trees::Tree &tree,
                  Patterns::PatternsSet &patterns, int begin, int end);
    bool growLayer(const TimeSeries &ts, Trees::Tree &tree,
                   Patterns::PatternsSet &patterns, int begin, int end);
    bool applyPattern(Patterns::Pattern &pattern, Patterns::Context &context);
};

}} // namespaces

#endif // INCREMENTAL_H
