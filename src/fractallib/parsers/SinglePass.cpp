#include "SinglePass.h"

using namespace FL::Parsers;
using namespace FL::Patterns;
using namespace FL::Exceptions;
using namespace FL::Trees;

SinglePass::SinglePass()
{
}

FL::ParseResult SinglePass::analyze(
    const TimeSeries &ts,
    Trees::Forest &forest,
    Patterns::PatternsSet &patterns,
    Patterns::Matcher &matcher,
    Trees::MetricsSet &metrics,
    int begin,
    int end)
{
    m_result.reset();
    FL::ParseResult commonResult;

    try
    {
        if (ts.size() < 2)
            throw EAnalyze(E_EMPTY_TIME_SERIES);
        if (forest.size() == 0)
            throw EAnalyze(E_EMPTY_FOREST);
        if (patterns.size() == 0)
            throw EAnalyze(E_EMPTY_PATTERNS);

        m_begin = begin;
        m_end   = end < 0 ? ts.size() : end;
        if (m_begin < 0 || m_begin >= m_end || m_end > ts.size())
            throw EAnalyze(E_INVALID_SEGMENT);

        while (true)
        {
            Forest::Iterator tree;
            forall(tree, forest)
            {
                analyzeTree(ts, **tree, matcher);
            }

            if (m_result.somethingAdded())
            {
                commonResult.add(m_result);
                m_result.reset();
            }
            else
                break;
        }
    }
    catch (const EAnalyze &e)
    {
        m_lastError = e;
        m_lastError.setArg(descriptionOf(e.id()));
    }

    return commonResult;
}

void SinglePass::analyzeTree(
    const TimeSeries &ts,
    Tree &tree,
    Patterns::Matcher &matcher)
{
    // Initialize analyse context
    Context context;
    context.setTimeSeries(&ts);
    context.setParseTree(&tree);
    context.setOutputTree(&tree);
    context.setCandidateNode(new Node());

    // Look for applicable patterns in each position of tree
    while (!context.isAtEnd())
    {
        if (!match(matcher, context))
            context.advanceCurrentRoot(1);
    }
    delete context.candidateNode();
}

bool SinglePass::match(Patterns::Matcher &matcher, Context &context)
{
    bool result;
    CheckInfo ci;

    if ((result = matcher.match(context, ci)) == true)
    {
        // Pattern sequence that was applied (use only one)
        CISequence &seq = *ci.applicableSequences[0].seq;

        Node *candidate = context.candidateNode();

        // Insert candidate node into output tree
        context.buildLastParsed(seq);
        Layer::Iterator child;
        forall(child, context.lastParsed())
            (*child)->setParent(candidate);
        context.outputTree().add(candidate);

        candidate->setId(ci.applicableSequences[0].pattern->id());

        // Remember modification
        //context.modification().push_back(context.candidateNode());

        // Update result, advance current roots position
        context.advanceCurrentRoot(seq.size());
        m_result.nodesAdded += 1;
        context.setCandidateNode(new Node());
    }

    return result;
}
