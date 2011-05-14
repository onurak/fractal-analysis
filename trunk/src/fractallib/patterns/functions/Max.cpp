#include "Max.h"
#include "../../TimeSeries.h"
#include "../../trees/Tree.h"

using namespace FL::Patterns::Functions;

Max::Max()
{
    m_name = "Max";
}


const GVariant& Max::operator()(Patterns::Context& context, FunctionArgs& args)
{
    if (args.size() == 0)
        throw FL::Exceptions::EArguments(m_name, -1, 0);
    const FL::TimeSeries *ts = context.timeSeries();

    // Get most left and most right indices of time series
    int begin = ts->values().size(), end = -1;

    FunctionArgs::iterator arg;
    forall(arg, args)
    {
        checkValidNode(*arg);
        FL::Trees::Node *node = **arg;
        if (node->begin() < begin)
            begin = node->begin();
        if (node->end() > end)
            end = node->end();
    }

    // Get max on interval
    int max = ts->values()[begin];
    for (++begin; begin <= end; ++begin)
        if (ts->values()[begin] > max)
            max = ts->values()[begin];

    return m_result = max;
}
