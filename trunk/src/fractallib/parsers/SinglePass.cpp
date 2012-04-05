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
    int tsBegin)
{
    m_result.reset();
    FL::ParseResult commonResult;

    m_interruption = false;

    try
    {
        if (ts.size() < 2)
            throw EAnalyze(E_EMPTY_TIME_SERIES);
        if (forest.size() == 0)
            throw EAnalyze(E_EMPTY_FOREST);
        if (patterns.size() == 0)
            throw EAnalyze(E_EMPTY_PATTERNS);

        m_metrics = &metrics;
        int newLevelsCount,  prevLevelsCount = forest.maxLevelCount();


        for (int iter = 0;; ++iter)
        {
            Forest::Iterator tree;
            forall(tree, forest)
                analyzeTree(ts, **tree, matcher, tsBegin);

            forest.cleanup();
            forest = m_forest;
            m_forest.clear();


            /*
            if (m_result.somethingAdded())
            {
                commonResult.add(m_result);
                m_result.reset();
            }
            else
                break;
            */
            // Was new level parsed from last operation?
            // If no then quit - everything that can be found is founded
            newLevelsCount = forest.maxLevelCount();
            if (newLevelsCount != prevLevelsCount && !m_interruption)
                prevLevelsCount = newLevelsCount;
            else
                break;
        }


        forest.insert(forest.end(),
                      m_finishedForest.begin(),
                      m_finishedForest.end());
        m_finishedForest.clear();
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
    Patterns::Matcher &matcher,
    int tsBegin)
{
    // Initialize analyse context
    Context context;
    context.setTimeSeries(&ts);
    context.setParseTree(&tree);
    //context.setOutputTree(&tree);
    context.setOutputTree(tree.copy());
    context.setCandidateNode(new Node());
    context.advanceCurrentRootToPos(tsBegin);

    // Look for applicable patterns in each position of tree
    while (!context.isAtEnd())
    {
        if (!match(matcher, context))
            context.advanceCurrentRoot(1);
    }


    size_t oldForestSize = m_forest.size() + m_finishedForest.size();
    if (m_metrics->filter(context.outputTree(), m_forest))
        m_forest.push_back(&context.outputTree());
    else
        m_finishedForest.push_back(&context.outputTree());
    // - Why not just ++m_result.treesAdded?
    // - Because metrics could delete any trees in forest
    m_result.treesAdded +=
            int(m_forest.size() + m_finishedForest.size() - oldForestSize);


    delete context.candidateNode();
}

bool SinglePass::match(Patterns::Matcher &matcher, Context &context)
{
    bool result;
    CheckInfo ci;

    if ((result = matcher.match(context, ci)) == true)
    {
        // Pattern sequence that was applied (use only one
        // with maximal length)
        size_t seqIdx;
        size_t maxSeqSize = 0;

        for(size_t i = 0; i < ci.applicableSequences.size(); ++i)
        {
            if (ci.applicableSequences[i].seq->size() > maxSeqSize)
            {
                seqIdx = i;
                maxSeqSize = ci.applicableSequences[i].seq->size();
            }
        }

        CheckInfo::ApplicableSeq& ciseq = ci.applicableSequences[seqIdx];

        // Fill candidate node's fields
        Node *candidate = context.candidateNode();
        candidate->setId(ciseq.pattern->id());
        candidate->origSequence() = *(ciseq.seq);

        // Insert candidate node into output tree
        context.buildLastParsed(*ciseq.seq);
        Layer::Iterator child;
        forall(child, context.lastParsed())
            (*child)->setParent(candidate);
        context.outputTree().add(candidate);

        // Update result, advance current roots position
        context.advanceCurrentRoot(ciseq.seq->size());
        context.setCandidateNode(new Node());
        m_result.nodesAdded += 1;
    }

    return result;
}
