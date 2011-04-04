#include "Value.h"

using namespace FL::Patterns::Functions;

Value::Value()
{
    m_name = "Value";
}


const GVariant& Value::operator()(Patterns::Context& context, FunctionArgs& args)
{
    return m_result;
}
