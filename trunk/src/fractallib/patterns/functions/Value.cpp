#include "Value.h"
#include "../../TimeSeries.h"
#include "../../trees/Tree.h"

using namespace FL::Patterns::Functions;

Value::Value()
{
    m_name = "Value";
}


const GVariant& Value::operator()(Patterns::Context& context, FunctionArgs& args)
{
    if (args.size() == 0)
        throw FL::Exceptions::EArguments(m_name, -1, 0);
    const FL::TimeSeries *ts = context.timeSeries();

    // Get min/max on segment
    checkValidNode(args[0]);
    FL::Trees::Node *node = *(args[0]);
    double vBeg = ts->values()[node->begin()];
    double vEnd = ts->values()[node->end()];
    double minimum = std::min(vBeg, vEnd);
    double maximum = std::max(vBeg, vEnd);

    FunctionArgs::iterator arg;
    for (arg = args.begin() + 1; arg != args.end(); ++arg)
    {
        checkValidNode(*arg);
        node = **arg;

        vBeg = ts->values()[node->begin()];
        vEnd = ts->values()[node->end()];
        if (vBeg > maximum)
            maximum = vBeg;
        if (vEnd > maximum)
            maximum = vEnd;

        if (vBeg < minimum)
            minimum = vBeg;
        if (vEnd < minimum)
            minimum = vEnd;
    }

    return m_result = (maximum - minimum);
}
