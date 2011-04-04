#include "Prev.h"

using namespace FL::Patterns::Functions;

Prev::Prev()
{
    m_name = "Prev";
}


const GVariant& Prev::operator()(Patterns::Context& context, FunctionArgs& args)
{
    return m_result;
}
