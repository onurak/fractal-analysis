#include "Self.h"

using namespace FL::Patterns::Functions;

Self::Self()
{
    m_name = "Self";
}


const GVariant& Self::operator()(Patterns::Context& context, FunctionArgs& args)
{
    return m_result;
}
