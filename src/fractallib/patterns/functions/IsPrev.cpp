#include "IsPrev.h"
#include "../../trees/Tree.h"

using namespace FL::Patterns::Functions;

IsPrev::IsPrev()
{
    m_name = "IsPrev";
}

const GVariant& IsPrev::operator()(Patterns::Context &context, FunctionArgs &args)
{
    if (args.size() != 1)
        throw FL::Exceptions::EArguments(m_name, 1, args.size());

    FL::Trees::Node *prevNode =
        context.outputTree().roots().getPrevNearestTo(context.candidateNode()->begin());
    if (prevNode == NULL)
        return m_result = false;
    int prevId = FL::IDGenerator::idOf( *args[0] );

    if (prevNode->id() == prevId || FL::IDGenerator::isSynonyms(prevNode->id(), prevId))
        return m_result = true;
    else
        return m_result = false;
}
