#include "EndTime.h"
#include "../../TimeSeries.h"
#include "../../trees/Tree.h"

using namespace FL::Patterns::Functions;

EndTime::EndTime()
{
    m_name = "EndTime";
}


const GVariant& EndTime::operator()(Patterns::Context& context, FunctionArgs& args)
{
    if (args.size() != 1)
        throw FL::Exceptions::EArguments(m_name, 1, args.size());
    checkValidNode(args[0]);
    return m_result = ((FL::Trees::Node*)(*args[0]))->end();
}
