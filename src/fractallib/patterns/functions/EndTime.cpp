#include "EndTime.h"

using namespace FL::Patterns::Functions;

EndTime::EndTime()
{
    m_name = "EndTime";
}


const GVariant& EndTime::operator()(Patterns::Context& context, FunctionArgs& args)
{
    return m_result;
}
