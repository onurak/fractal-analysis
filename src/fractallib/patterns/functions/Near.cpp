#include "Near.h"

using namespace FL::Patterns::Functions;

Near::Near()
{
    m_name = "Near";
}


const GVariant& Near::operator()(Patterns::Context& context, FunctionArgs& args)
{
    return m_result;
}
