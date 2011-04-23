#include "DynamicMultiPass.h"

using namespace FL::Parsers;

using namespace FL::Parsers;
using namespace FL::Patterns;
using namespace FL::Exceptions;
using namespace FL::Trees;


DynamicMultiPass::DynamicMultiPass()
{
}


FL::ParseResult DynamicMultiPass::analyze(
    const TimeSeries &ts,
    Trees::Forest &forest,
    Patterns::PatternsSet &patterns,
    Trees::MetricsSet &metrics,
    Forecast &forecast)
{
    m_result.reset();
    m_interruption = false;

    try
    {
        if (ts.values().size() < 3)
            throw EAnalyze(E_EMPTY_TIME_SERIES);
        if (forest.size() == 0)
            throw EAnalyze(E_EMPTY_FOREST);
        if (patterns.size() == 0)
            throw EAnalyze(E_EMPTY_PATTERNS);

        // 1. Update markup level 0
        updateMarkup(ts, forest, patterns, metrics, forecast);

        // 2. Find all possible patterns at the end of parse tree
        forecastTreeStructure(ts, forest, patterns, metrics, forecast);

        // 3. Calculate trees metrics, eclude those which are not satisfy metrics conditions
        calcMetrics(ts, forest, patterns, metrics, forecast);

        // 4. Make time series forecast
        forecastTimeSeries(ts, forest, patterns, metrics, forecast);
    }
    catch (const FL::Exceptions::EAnalyze &e)
    {
        m_lastError = e;
        //m_lastError.setArg(descriptionOf(e.id()));
    }

    return m_result;
}

//! Update markup level 0
void DynamicMultiPass::updateMarkup(
    const TimeSeries &ts,
    Trees::Forest &forest,
    Patterns::PatternsSet &patterns,
    Trees::MetricsSet &metrics,
    Forecast &forecast)
{
    int terminal;
    if (ts.values()[ts.values().size()-1] >= ts.values()[ts.values().size()-2])
        terminal = FL::IDGenerator::idOf("a");
    else
        terminal = FL::IDGenerator::idOf("b");

    int lastPosition = ts.values().size()-2;

    FL::Trees::Forest::Iterator tree;
    forall(tree, forest)
    {
        Node *lastTerminal = (*tree)->nodesByLevel(0).back();

        if (lastTerminal->end() == lastPosition &&
            lastTerminal->id()  == terminal)
        {
            lastTerminal->setEnd(lastTerminal->end() + 1);
        }
        else
        {
            Node *newNode = new Node(NULL, terminal, lastPosition, lastPosition+1, 0);
            (*tree)->add(newNode);
        }
    }
}

//! Find all possible patterns at the end of parse tree
void DynamicMultiPass::forecastTreeStructure(
    const TimeSeries &ts,
    Trees::Forest &forest,
    Patterns::PatternsSet &patterns,
    Trees::MetricsSet &metrics,
    Forecast &forecast)
{

}

//! Calculate trees metrics, eclude those which are not satisfy metrics conditions
void DynamicMultiPass::calcMetrics(
    const TimeSeries &ts,
    Trees::Forest &forest,
    Patterns::PatternsSet &patterns,
    Trees::MetricsSet &metrics,
    Forecast &forecast)
{

}

//! Make time series forecast
void DynamicMultiPass::forecastTimeSeries(
    const TimeSeries &ts,
    Trees::Forest &forest,
    Patterns::PatternsSet &patterns,
    Trees::MetricsSet &metrics,
    Forecast &forecast)
{

}
