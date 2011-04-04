#include "AtOneLine.h"

using namespace FL::Patterns::Functions;

AtOneLine::AtOneLine()
{
    m_name = "AtOneLine";
}


const GVariant& AtOneLine::operator()(Patterns::Context& context, FunctionArgs& args)
{
    return m_result;
}
