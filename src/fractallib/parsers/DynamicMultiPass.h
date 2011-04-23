#ifndef DYNAMICMULTIPASS_H
#define DYNAMICMULTIPASS_H

#include "AbstractDynamicParser.h"

namespace FL { namespace Parsers {

/*! \class DynamicMultiPass
  * \brief Dynamic multi pass (recursive) parser. Can find all parse trees.
  *
  */
class DynamicMultiPass : public AbstractDynamicParser
{
public:
    DynamicMultiPass();

    virtual ParseResult analyze(
        const TimeSeries &ts,
        Trees::Forest &forest,
        Patterns::PatternsSet &patterns,
        Trees::MetricsSet &metrics,
        Forecast &forecast);

protected:

    //! Update markup level 0
    void updateMarkup(const TimeSeries &ts,
                      Trees::Forest &forest,
                      Patterns::PatternsSet &patterns,
                      Trees::MetricsSet &metrics,
                      Forecast &forecast);

    //! Find all possible patterns at the end of parse tree
    void forecastTreeStructure(const TimeSeries &ts,
                               Trees::Forest &forest,
                               Patterns::PatternsSet &patterns,
                               Trees::MetricsSet &metrics,
                               Forecast &forecast);

    //! Calculate trees metrics, eclude those which are not satisfy metrics conditions
    void calcMetrics(const TimeSeries &ts,
                Trees::Forest &forest,
                Patterns::PatternsSet &patterns,
                Trees::MetricsSet &metrics,
                Forecast &forecast);

    //! Make time series forecast
    void forecastTimeSeries(const TimeSeries &ts,
                       Trees::Forest &forest,
                       Patterns::PatternsSet &patterns,
                       Trees::MetricsSet &metrics,
                       Forecast &forecast);
protected:
    bool m_interruption;
    FL::ParseResult m_result;
};

}} // namespaces

#endif // DYNAMICMULTIPASS_H
