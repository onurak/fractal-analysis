#include "Prev.h"
#include "../../trees/Tree.h"

using namespace FL::Patterns::Functions;

Prev::Prev()
{
    m_name = "Prev";
    defineArg("samelevel", false);
}

const GVariant& Prev::operator()(Patterns::Context& context, FunctionArgs& args)
{
    if (args.size() > 1)
        throw FL::Exceptions::EArguments(m_name, 0, 1, args.size());
    parseArgs(args);

    FL::Trees::Node *prevNode =
            context.outputTree().roots().getPrevNearestTo(context.candidateNode()->begin());
    if (prevNode == NULL ||
        (m_options["samelevel"] && prevNode->level() == context.candidateNode()->level()))
        return m_result = NULL;
    return m_result = prevNode;
}
