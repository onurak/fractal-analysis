#ifndef AB_H
#define AB_H

#include "AbstractMarker.h"

namespace FL { namespace Markers {

class AB : public FL::Markers::AbstractMarker
{
public:
    AB();
    virtual ~AB();

    virtual ParseResult analyze(
                      const TimeSeries &ts,
                      Trees::Forest &forest,
                      Patterns::PatternsSet &patterns);
};

}} // namespaces

#endif // AB_H
