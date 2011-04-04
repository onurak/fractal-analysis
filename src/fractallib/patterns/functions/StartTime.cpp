#include "StartTime.h"

using namespace FL::Patterns::Functions;

StartTime::StartTime()
{
    m_name = "StartTime";
}


const GVariant& StartTime::operator()(Patterns::Context& context, FunctionArgs& args)
{
    return m_result;
}
