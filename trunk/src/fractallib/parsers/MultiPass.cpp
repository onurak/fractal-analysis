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
        m_ts       = &ts;
        m_metrics  = &metrics;
        m_forest.clear();


        int newLevelsCount,  prevLevelsCount = maxLevel(forest);

        // Search for new levels while we can
        while (true)
        {
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
                deleteContext(m_branches.front());
                m_branches.erase(m_branches.begin());
            }

            // Delete old contexts that are unused now
            /*
            for (size_t i = 0; i < m_oldBranches.size(); ++i)
                deleteContext(m_oldBranches[i]);
            m_oldBranches.clear();
            */

            // Swap analysis results
            forest.cleanup();
            forest = m_forest;
            m_forest.clear();
            commonResult.add(m_result);

            // Was new level parsed from last operation?
            // If no then quit - everything that can be found is founded
            newLevelsCount = maxLevel(forest);
            if (newLevelsCount != prevLevelsCount && !m_interruption)
                prevLevelsCount = newLevelsCount;
            else
                break;

        }


        removeSubtrees(forest);
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
    /*
    if (isDuplicatingContext(context))
    {
        deleteContext(context);
        return;
    }
    */

    // Look for applicable patterns in each position of tree
    while (!context->isAtEnd())
    {
        match(matcher, *context);
        context->advanceCurrentRoot(1);
    }

    int oldForestSize = (int) m_forest.size();
    if (m_metrics->filter(context->outputTree(), m_forest))
        m_forest.push_back(&context->outputTree());
    else
        delete &context->outputTree();

    m_result.treesAdded += int(m_forest.size() - oldForestSize);


    delete context->candidateNode();
    delete context;
    //m_oldBranches.push_back(context);
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
        if ((*tree)->levelCount() > result)
            result = (*tree)->levelCount();
    }

    return result;
}

void MultiPass::removeSubtrees(FL::Trees::Forest &forest)
{
    bool isFirstTreeDeleted;

    for (size_t i = 0; i < forest.size(); )
    {
        Tree *firstTree = forest[i];
        isFirstTreeDeleted = false;

        for (size_t j = i+1; j < forest.size(); )
        {
            Tree *secondTree = forest[j];

            TreeCompareResult tcr = firstTree->compare(*secondTree);
            if (tcr.isSecondSubtreeOfFirst())
            {
                forest.erase(forest.begin() + j);
                m_result.treesAdded--;
            }
            else if (tcr.isFirstSubtreeOfSecond())
            {
                forest.erase(forest.begin() + i);
                isFirstTreeDeleted = true;
                m_result.treesAdded--;
                break;
            }
            else
                ++j;
        }

        if (!isFirstTreeDeleted)
            ++i;
    }

    /*
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
    */
}

/*
bool MultiPass::isDuplicatingContext(Patterns::Context *context)
{
    std::vector<Patterns::Context*>::const_iterator itci;

    forall(itci, m_oldBranches)
    {
        Context *ci = *itci;
        if (context->currentRootPos() == ci->currentRootPos() &&
            context->roots().size()   == ci->roots().size())
        {
            const std::list<Trees::Node*>& roots1 = context->roots();
            const std::list<Trees::Node*>& roots2 = ci->roots();
            Layer::ConstIterator r1, r2;

            forboth(r1, roots1, r2, roots2)
            {
                Node *n1 = *r1, *n2 = *r2;
                if (n1->begin() == n2->begin() &&
                    n1->end()   == n2->end()   &&
                    n1->level() == n2->level() &&
                    n1->id()    == n2->id())
                {
                    return true;
                }

            }
        }
    }

    return false;
}
*/

void MultiPass::deleteContext(Patterns::Context *context)
{
    delete context->candidateNode();
    delete &context->outputTree();
    delete context;
}






