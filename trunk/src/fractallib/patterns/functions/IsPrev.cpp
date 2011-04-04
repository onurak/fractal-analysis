#include "IsPrev.h"

using namespace FL::Patterns::Functions;

IsPrev::IsPrev()
{
    m_name = "IsPrev";
}

const GVariant& IsPrev::operator()(Patterns::Context &context, FunctionArgs &args)
{
    return m_result;
}
