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

        // Init
        m_ts = &ts;
        m_patterns = &patterns;
        m_forest.clear();

        // For each tree create analysis branch
        Forest::Iterator tree;
        forall(tree, forest)
            newAnalysisBranchForTree(**tree);

        // Do analysis all branches
        while (m_branches.size() > 0 && !m_interruption)
        {
            runBranch(m_branches.back());
            if (!onProgress.isNull() && !onProgress(m_result))
                break;
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

        // Return analysis results
        std::reverse(m_forest.begin(), m_forest.end());
        forest = m_forest;
        m_forest.clear();
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

    // Check if analysed tree is unique
    Forest::Iterator tree;
    bool isTreeUnique = true;

    for (tree = m_forest.begin(); tree != m_forest.end(); )
    {
        TreeCompareResult tcr = (*tree)->compare(context->outputTree());

        // Is this tree is subtree of another tree
        if (tcr.isSecondSubtreeOfFirst())
        {
            isTreeUnique = false;
            break;
        }

        // Is another tree is subtree of this
        if (tcr.isFirstSubtreeOfSecond())
        {
            delete *tree;
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
        delete &context->outputTree();


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
    if (pattern.check(context, info) == crOK)
    {
        std::vector<CISequence*>::iterator itSequence;

        forall(itSequence, info.applicableSequences)
        {
            // Pattern sequence that was applied
            CISequence &seq = **itSequence;

            // New context for new branch
            Context* newContext = new Context(context);

            // Insert candidate node into output tree
            newContext->buildLastParsed(seq);
            Layer::Iterator child;
            forall(child, newContext->lastParsed())
                (*child)->setParent(newContext->candidateNode());
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
    m_branches.push_back(context);
}
