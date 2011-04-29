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
    const TimeSeries &ts, Forest &forest, PatternsSet &patterns,
    Trees::MetricsSet &metrics,
    int begin, int end)
{
    m_result.reset();
    m_interruption = false;

    try
    {
        if (ts.values().size() < 2)
            throw EAnalyze(E_EMPTY_TIME_SERIES);
        if (forest.size() == 0)
            throw EAnalyze(E_EMPTY_FOREST);
        if (patterns.size() == 0)
            throw EAnalyze(E_EMPTY_PATTERNS);

        FL::Trees::Metric *metric;
        metric = metrics.getByName("Subtree");
        m_wantSubtree = metric && metric->isEnabled();

        metric = metrics.getByName("Max trees");
        m_maxTrees = metric && metric->isEnabled() ? metric->limit() : INT_MAX;

        metric = metrics.getByName("Max levels count");
        m_maxLevelCount = metric && metric->isEnabled() ? metric->limit() : INT_MAX;


        // Init
        m_ts = &ts;
        m_patterns = &patterns;
        m_oldForest = forest;
        forest.clear();
        m_newForest.clear();

        int prevMaxLevel = m_oldForest.maxLevelCount(), newMaxLevel;

        // Parse ALL levels
        do
        {
            // For each tree create analysis branch
            Forest::Iterator tree;
            forall(tree, m_oldForest)
                newAnalysisBranchForTree(**tree);

            // Do analysis all branches
            while (m_branches.size() > 0 && !m_interruption)
            {
                runBranch(m_branches.front());
                if (!onProgress.isNull() && !onProgress(m_result, &m_newForest))
                    break;
            }
            m_oldForest.cleanup();
            if (m_interruption)
                break;

            /*
            for (tree = m_newForest.begin(); tree != m_newForest.end(); )
            {
                if ( !metrics.check(**tree, m_newForest) )
                {
                    delete *tree;
                    tree = m_newForest.erase(tree);
                }
                else
                    ++tree;
            }
            */

            newMaxLevel = m_newForest.maxLevelCount();
            if (prevMaxLevel < newMaxLevel && newMaxLevel <= m_maxLevelCount)
            {
                m_oldForest = m_newForest;
                m_newForest.clear();
                prevMaxLevel = newMaxLevel;
            }
            else
                break;
        } while (true);

        // Clear interrupted branches
        while (m_branches.size() > 0)
        {
            Context *context = m_branches.front();
            delete &context->outputTree();
            delete context->candidateNode();
            delete context;
            m_branches.erase(m_branches.begin());
        }

        // Return result without subtrees
        while (m_newForest.size() > 0)
        {
            if ( !isSubtreeOfSomeTree(m_newForest.front()) )
                forest.push_back(m_newForest.front());
            else
                delete m_newForest.front();
            m_newForest.erase(m_newForest.begin());

            if (!onProgress.isNull())
                onProgress(m_result, &m_newForest);
        }
        std::reverse(forest.begin(), forest.end());
    }
    catch (const EAnalyze &e)
    {
        m_lastError = e;
        m_lastError.setArg(descriptionOf(e.id()));
    }

    return m_result;
}

void MultiPass::runBranch(Patterns::Context *context)
{
    // Initialize iterators
    PatternsSet::Iterator pattern;

    // Look for applicable patterns in each position of tree
    while (!context->isAtEnd())
    {
        // If found new applicable pattern - add new analysis branch
        forall(pattern, *m_patterns)
            applyPattern(**pattern, *context);
        context->advanceCurrentRoot(1);
    }

    if (!m_wantSubtree)
    {
        m_newForest.push_back(&context->outputTree());
    }
    else
    {
        if ( isSubtreeOfSomeTree(&context->outputTree()) )
            delete &context->outputTree();
        else
            m_newForest.push_back(&context->outputTree());
    }

    // Forget context
    std::vector<Patterns::Context*>::iterator itContext;
    if (FL::search(m_branches, context, itContext))
        m_branches.erase(itContext);

    // Clear context
    delete context->candidateNode();
    delete context;
}

void MultiPass::applyPattern(Pattern &pattern, Context &context)
{
    CheckInfo info;
    if (pattern.check(context, info, coAllowPartialMatch) == crOK)
    {
        std::vector<CISequenceInfo>::iterator itSequence;

        forall(itSequence, info.applicableSequences)
        {
            // Pattern sequence that was applied
            CISequence &seq = *itSequence->sequence;

            // New context for new branch
            Context* newContext = new Context(context);

            // Insert candidate node into output tree
            newContext->buildLastParsed(seq);
            Layer::Iterator child;
            forall(child, newContext->lastParsed())
                (*child)->setParent(newContext->candidateNode());

            if (itSequence->checkResult == crPartialMatch)
            {
                newContext->candidateNode()->setStatus(nsPossible);
                newContext->candidateNode()->setOrigPattern(&pattern);
                newContext->candidateNode()->setOrigSequence(&seq);
            }

            newContext->outputTree().add(newContext->candidateNode());

            // Remember modification
            //context.modification().push_back(context.candidateNode());

            // Update result, advance current roots position
            newContext->advanceCurrentRoot(seq.size());
            m_result.nodesAdded += 1;
            newContext->setCandidateNode(new Node());

            newAnalysisBranch(newContext);
        }
    }
}

void MultiPass::newAnalysisBranchForTree(FL::Trees::Tree &tree)
{
    tree.validateStructure();
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
    if ((int)m_newForest.size() >= m_maxTrees)
    {
        delete &context->outputTree();
        delete context->candidateNode();
        delete context;
    }
    else
        m_branches.push_back(context);
}

bool MultiPass::isSubtreeOfSomeTree(Trees::Tree *tree)
{
    Forest::Iterator itTree;
    bool isTreeUnique = true;

    for (itTree = m_newForest.begin(); itTree != m_newForest.end(); )
    {
        if (*itTree == tree)
        {
            ++itTree;
            continue;
        }

        TreeCompareResult tcr = (*itTree)->compare(*tree);

        // Is this tree is subtree of another tree
        if (tcr.isSecondSubtreeOfFirst())
        {
            isTreeUnique = false;
            break;
        }

        // Is another tree is subtree of this
        if (tcr.isFirstSubtreeOfSecond())
        {
            delete *itTree;
            itTree = m_newForest.erase(itTree);
            m_result.treesAdded--;
        }
        else
            ++itTree;
    }

    if (isTreeUnique)
    {
        m_result.treesAdded++;
        return false;
    }
    else
        return true;
}
