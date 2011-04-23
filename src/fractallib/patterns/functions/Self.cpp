#include "Self.h"

using namespace FL::Patterns::Functions;

Self::Self()
{
    m_name = "Self";
}


const GVariant& Self::operator()(Patterns::Context& context, FunctionArgs& args)
{
    if (args.size() > 0)
        throw FL::Exceptions::EArguments(m_name, 0, args.size());
    return m_result = context.candidateNode();
}
