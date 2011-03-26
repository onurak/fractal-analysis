#include "Max.h"

using namespace FL::Patterns::Functions;

Max::Max()
{
    m_name = "Max";
}


const GVariant& Max::operator()(Patterns::Context& context, FunctionArgs& args)
{
    return m_result;
}
