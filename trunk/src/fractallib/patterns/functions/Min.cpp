#include "Min.h"

using namespace FL::Patterns::Functions;

Min::Min()
{
    m_name = "Min";
}

const GVariant& Min::operator()(Patterns::Context &context, FunctionArgs &args)
{
    return m_result;
}
