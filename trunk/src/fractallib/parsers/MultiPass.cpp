#include "MultiPass.h"

using namespace FL::Parsers;

using namespace FL::Parsers;
using namespace FL::Patterns;
using namespace FL::Exceptions;
using namespace FL::Trees;

MultiPass::MultiPass()
{
}

FL::ParseResult MultiPass::analyze(
    const TimeSeries &ts,
    Trees::Forest &forest,
    Patterns::PatternsSet &patterns,
    Patterns::Matcher &matcher,
    Trees::MetricsSet &metrics,
    int begin,
    int end)
{
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

        // Init
        m_ts = &ts;
        m_patterns = &patterns;
        m_forest.clear();


        int prevLevelsCount = maxLevel(forest);
        int newLevelsCount = prevLevelsCount;

        // Search for new levels while we can
        do
        {
            prevTreesCount = m_result.treesAdded;
            m_result.reset();

            // For each tree create analysis branch
            Forest::Iterator tree;
            forall(tree, forest)
                newAnalysisBranchForTree(**tree);

            // Analyse all branches
            while (m_branches.size() > 0 && !m_interruption)
            {
                runBranch(m_branches.front(), matcher);
                m_branches.erase(m_branches.begin());

                if (!onProgress.isNull())
                    m_interruption = onProgress(m_result, &m_forest);
            }

            // Clear interrupted branches
            while (m_branches.size() > 0)
            {
                Context *context = m_branches.front();
                delete &context->outputTree();
                delete context->candidateNode();
                delete context;
                m_branches.erase(m_branches.begin());
            }

            // Swap analysis results
            forest.cleanup();
            forest = m_forest;
            m_forest.clear();
            commonResult.add(m_result);
            newLevelsCount = maxLevel(forest);
        } while (prevLevelsCount != newLevelsCount);
    }
    catch (const EAnalyze &e)
    {
        m_lastError = e;
        m_lastError.setArg(descriptionOf(e.id()));
    }

    return commonResult;
}

void MultiPass::runBranch(Patterns::Context *context, Patterns::Matcher &matcher)
{
    // Look for applicable patterns in each position of tree
    while (!context->isAtEnd())
    {
        match(matcher, *context);
        context->advanceCurrentRoot(1);
    }

    // Check if analysed tree is unique
    Forest::Iterator tree;
    bool isTreeUnique = true;
    for (tree = m_forest.begin(); tree != m_forest.end(); )
    {
        TreeCompareResult tcr = (*tree)->compare(context->outputTree());
        if (tcr.isSecondSubtreeOfFirst())
        {
            isTreeUnique = false;
            break;
        }
        else if (tcr.isFirstSubtreeOfSecond())
        {
            tree = m_forest.erase(tree);
            m_result.treesAdded--;
        }
        else
            ++tree;

    }
    if (isTreeUnique)
    {
        m_forest.push_back(&context->outputTree());
        m_result.treesAdded++;
    }
    else
    {
        delete &context->outputTree();
    }


    // Clear context
    delete context->candidateNode();
    delete context;
}

bool MultiPass::match(Matcher &matcher, Context &context)
{
    bool result;
    CheckInfo info;
    if ((result = matcher.match(context, info)) == true)
    {
        std::vector<CheckInfo::ApplicableSeq>::iterator itSequence;

        forall(itSequence, info.applicableSequences)
        {
            // Pattern sequence that was applied
            CISequence &seq = *itSequence->seq;

            // New context for new branch
            Context* newContext = new Context(context);
            Node *candidate = newContext->candidateNode();

            // Insert candidate node into output tree
            newContext->buildLastParsed(seq);
            Layer::Iterator child;
            forall(child, newContext->lastParsed())
                (*child)->setParent(candidate);
            newContext->outputTree().add(candidate);

            candidate->setId(itSequence->pattern->id());

            // Remember modification
            //context.modification().push_back(context.candidateNode());

            // Update result, advance current roots position
            newContext->advanceCurrentRoot(seq.size());
            newContext->setCandidateNode(new Node());

            newAnalysisBranch(newContext);
            //m_result.nodesAdded++;
        }
    }

    return result;
}

void MultiPass::newAnalysisBranchForTree(FL::Trees::Tree &tree)
{
    // Initialize analyse context
    Context *context = new Context();
    context->setTimeSeries(m_ts);
    context->setParseTree(&tree);
    context->setOutputTree(tree.copy());
    context->setCandidateNode(new Node());
    newAnalysisBranch(context);
}

void MultiPass::newAnalysisBranch(Patterns::Context *context)
{
    m_branches.push_back(context);
}

int MultiPass::maxLevel(const Forest &forest)
{
    int result = 0;
    Forest::ConstIterator tree;
    forall(tree, forest)
    {
        if ((*tree)->levelsCount() > result)
            result = (*tree)->levelsCount();
    }

    return result;
}
