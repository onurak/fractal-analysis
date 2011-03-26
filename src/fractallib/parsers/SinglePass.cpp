#include "SinglePass.h"

using namespace FL::Parsers;
using namespace FL::Patterns;
using namespace FL::Exceptions;
using namespace FL::Trees;

SinglePass::SinglePass()
{
}

FL::ParseResult SinglePass::analyze(const TimeSeries &ts, Forest &forest, PatternsSet &patterns)
{
    m_result.reset();

    try
    {
        if (ts.values().size() < 2 || forest.size() == 0 || patterns.size() == 0)
            throw EAnalyze(E_INVALID_INPUT);

        Forest::Iterator tree;
        forall(tree, forest)
            analyzeTree(ts, **tree, patterns);
    }
    catch (const EAnalyze &e)
    {
        m_lastError = e;
    }

    return m_result;
}

void SinglePass::analyzeTree(const TimeSeries &ts, Tree &tree, PatternsSet &patterns)
{
    // Initialize iterators
    PatternsSet::Iterator pattern;

    // Initialize analyse context
    Context context;
    context.setParseTree(&tree);
    //context.setOutputTree(tree.copy());
    context.setOutputTree(&tree);
    context.setCandidateNode(new Node());

    // Look for applicable patterns in each position of tree
    while (!context.isAtEnd())
    {
        bool found = false;
        forall(pattern, patterns)
        {
            if (applyPattern(**pattern, context))
            {
                found = true;
                break;
            }
        }

        if (!found)
            context.anvanceCurrentRoot(1);
    }
    delete context.candidateNode();
}

bool SinglePass::applyPattern(Pattern &pattern, Context &context)
{
    CheckInfo info;
    if (pattern.check(context, info) == crOK)
    {
        // Pattern sequence that was applied (use only one)
        CISequence &seq = *info.applicableSequnces[0];

        // Insert candidate node into output tree
        context.buildLastParsed(seq);
        Layer::Iterator child;

//        forall(child, context.lastParsed())
//        {
//             Node *relativeToChild = (*child)->relativeNode();
//             relativeToChild->setParent(candidate);
//        }
        forall(child, context.lastParsed())
            (*child)->setParent(context.candidateNode());
        context.outputTree().add(context.candidateNode());

        // Update result, advance current roots position
        context.anvanceCurrentRoot(seq.size());
        m_result.nodesAdded += 1;
        context.setCandidateNode(new Node());

        return true;
    }

    return false;
}
