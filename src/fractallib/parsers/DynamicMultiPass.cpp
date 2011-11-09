/** This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Fractal Library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "DynamicMultiPass.h"

using namespace FL::Parsers;

using namespace FL::Parsers;
using namespace FL::Patterns;
using namespace FL::Exceptions;
using namespace FL::Trees;


DynamicMultiPass::DynamicMultiPass()
{
}


//FL::ParseResult DynamicMultiPass::analyze(
//    const TimeSeries &ts,
//    Trees::Forest &forest,
//    Patterns::PatternsSet &patterns,
//    Trees::MetricsSet &metrics,
//    Forecast &forecast)
//{
//    m_result.reset();
//    m_interruption = false;

//    try
//    {
//        if (ts.values().size() < 3)
//            throw EAnalyze(E_EMPTY_TIME_SERIES);
//        if (forest.size() == 0)
//            throw EAnalyze(E_EMPTY_FOREST);
//        if (patterns.size() == 0)
//            throw EAnalyze(E_EMPTY_PATTERNS);

//        FL::Trees::Metric *metric;
//        metric = metrics.getByName("Subtree");
//        m_wantSubtree = metric && metric->isEnabled();

//        metric = metrics.getByName("Max trees");
//        m_maxTrees = metric && metric->isEnabled() ? metric->limit() : INT_MAX;

//        metric = metrics.getByName("Max levels count");
//        m_maxLevelCount = metric && metric->isEnabled() ? metric->limit() : INT_MAX;


//        m_ts = &ts;
//        m_patterns = &patterns;

//        // Update previous "possible" patterns
//        updatePossiblePatterns(forest);

//        // Find all possible patterns at the end of parse tree
//        forecastTreeStructure(forest);

//        // Make time series forecast
//        forecastTimeSeries(ts, forest, patterns, forecast);

//        // Fix forest structure
//        Forest::Iterator tree;
//        forall(tree, forest)
//            (*tree)->fixup();
//    }
//    catch (const EAnalyze &e)
//    {
//        m_lastError = e;
//        //m_lastError.setArg(descriptionOf(e.id()));
//    }

//    return m_result;
//}

//void DynamicMultiPass::updatePossiblePatterns(Trees::Forest &forest)
//{
//    Layer::ConstIterator itNode;
//    Forest::Iterator itTree;

//    std::list<Node*> processed, updated, removed;

//    //int indentSize = m_patterns->maxSequenceLength();

//    forall(itTree, forest)
//    {
//        Tree *tree = *itTree;
//        if (tree->possibleNodes().size() == 0)
//            continue;

//        // Create analysis context
//        tree->validateStructure();
//        Context *context = new Context();
//        context->setTimeSeries(m_ts);
//        context->setParseTree(tree);
//        context->setOutputTree(tree);

//        // Update each possible node status.
//        // Updating is performed level-by-level starting from zero.
//        Layer possibleNodes = tree->possibleNodes();
//        possibleNodes.sortByLevel();

//        forall(itNode, possibleNodes)
//        {
//            Node *node = *itNode;
//            if (FL::exists(processed, node))
//                continue;

//            if (node->status() != nsPossible)
//            {
//                updated.push_back(node);
//                continue;
//            }

//            // Do check that new node still valid
//            int oldEnd = node->end();
//            context->setCandidateNode(node);
//            CheckInfo info;
//            CheckResult cr = node->origPattern()->check(*context, info, coContinueChecking);

//            // Node still possible or even completed - modify tree to add it
//            if (cr == crOK)
//            {
//                node->setEnd(oldEnd);
//                // Set new children to possible node
//                Layer lastChildFollowers =
//                        context->outputTree().getFollowingRoots( node );
//                Layer::Iterator itChildNode = lastChildFollowers.begin();
//                int appendChildrenSize =
//                        info.applicableSequences[0].matchedCount - node->children().size();

//                for (int i = 0; i < appendChildrenSize; ++i, ++itChildNode)
//                    (*itChildNode)->setParent(node);

//                // Check if node is now completed
//                if (node->children().size() == node->origSequence()->size())
//                {
//                    node->setOrigSequence(NULL);
//                    node->setOrigPattern(NULL);
//                    updated.push_back(node);
//                }

//                // Also check parents of this node (they are possible nodes too)
//                Node *parent = node->parent();
//                while (parent != NULL)
//                {
//                    context->setCandidateNode(parent);

//                    // Check that guard of node is still valid.
//                    // BNF don't need to be checked because if this node is parent
//                    // of node that was checked before, then there are no
//                    // new nodes for this node.
//                    FL::CheckOptions options = static_cast<FL::CheckOptions>(
//                                FL::coNoDescriptionCheck | FL::coContinueChecking);
//                    CheckResult pcr =
//                        parent->origPattern()->check(*context, info, options);

//                    // This parent is invalid now - delete it with all its parents
//                    if (pcr != crOK)
//                    {
//                        processed.push_back(parent);
//                        removed.push_back(parent);
//                        parent = parent->parent();

//                        while (parent)
//                        {
//                            processed.push_back(parent);
//                            //removed.push_back(parent);
//                            parent = parent->parent();
//                        }
//                    }

//                    // It's ok - get to next parent
//                    else
//                    {
//                        processed.push_back(parent);
//                        parent = parent->parent();
//                    }
//                }
//            }

//            // Node isn't possible anymore - remove it and its parents from tree structure
//            else
//            {
//                Node *parent = node->parent();
//                while (parent)
//                {
//                    processed.push_back(parent);
//                    //removed.push_back(parent);
//                    parent = parent->parent();
//                }
//                removed.push_back(node);
//            }

//            processed.push_back(node);
//        }

//        delete context;

//        processed.clear();
//        std::list<Node*>::iterator node;
//        forall(node, updated)
//            tree->changeNodeStatus(*node, nsFloating);
//        updated.clear();

//        forall(node, removed)
//        {
//            tree->remove(*node);
//            delete *node;
//        }
//        removed.clear();
//    }
//}

////! Find all potential patterns
//void DynamicMultiPass::forecastTreeStructure(Trees::Forest &forest)
//{
//    int indentSize = m_patterns->maxSequenceLength() + 1;
//    m_oldForest = forest;
//    forest.clear();
//    m_newForest.clear();

//    int prevMaxLevel = m_oldForest.maxLevelCount(), newMaxLevel;

//    // Parse ALL levels
//    do
//    {
//        // Create context for each tree with position "at the end"
//        FL::Trees::Forest::Iterator itTree;
//        forall(itTree, m_oldForest)
//        {
//            Tree *tree = *itTree;
//            newAnalysisBranchForTree(*tree, indentSize);
//        }

//        // Do analysis all branches
//        while (m_branches.size() > 0 && !m_interruption)
//        {
//            runBranch(m_branches.back());
//            if (!onProgress.isNull() && !onProgress(m_result, &m_newForest))
//                break;
//        }
//        m_oldForest.cleanup();
//        if (m_interruption)
//            break;

//        newMaxLevel = m_newForest.maxLevelCount();
//        if (prevMaxLevel < newMaxLevel && newMaxLevel <= m_maxLevelCount)
//        {
//            m_oldForest = m_newForest;
//            m_newForest.clear();
//            prevMaxLevel = newMaxLevel;
//        }
//        else
//            break;
//    } while (true);

//    // Clear interrupted branches
//    while (m_branches.size() > 0)
//    {
//        Context *context = m_branches.front();
//        delete &context->outputTree();
//        delete context->candidateNode();
//        delete context;
//        m_branches.erase(m_branches.begin());
//    }

//    // Return result without subtrees
//    while (m_newForest.size() > 0)
//    {
//        if ( !isSubtreeOfSomeTree(m_newForest.front()) )
//            forest.push_back(m_newForest.front());
//        else
//            delete m_newForest.front();
//        m_newForest.erase(m_newForest.begin());
//    }
//    std::reverse(forest.begin(), forest.end());
//}

//void DynamicMultiPass::runBranch(Patterns::Context *context)
//{
//    // Initialize iterators
//    PatternsSet::Iterator pattern;

//    // Look for applicable patterns in each position of tree
//    while (!context->isAtEnd())
//    {
//        // If found new applicable pattern - add new analysis branch
//        forall(pattern, *m_patterns)
//            applyPattern(**pattern, *context);
//        context->advanceCurrentRoot(1);
//    }

//    if (!m_wantSubtree)
//    {
//        m_newForest.push_back(&context->outputTree());
//    }
//    else
//    {
//        if ( isSubtreeOfSomeTree(&context->outputTree()) )
//            delete &context->outputTree();
//        else
//            m_newForest.push_back(&context->outputTree());
//    }

//    // Forget context
//    std::vector<Patterns::Context*>::iterator itContext;
//    if (FL::search(m_branches, context, itContext))
//        m_branches.erase(itContext);

//    // Clear context
//    delete context->candidateNode();
//    delete context;
//}

//void DynamicMultiPass::applyPattern(Pattern &pattern, Context &context)
//{
//    CheckInfo info;
//    if (pattern.check(context, info, coAllowPartialMatch) == crOK)
//    {
//        std::vector<CISequenceInfo>::iterator itSequence;

//        forall(itSequence, info.applicableSequences)
//        {
//            // Pattern sequence that was applied
//            CISequence &seq = *itSequence->sequence;

//            // New context for new branch
//            Context* newContext = new Context(context);

//            // Insert candidate node into output tree
//            newContext->buildLastParsed(seq);
//            Layer::Iterator child;
//            forall(child, newContext->lastParsed())
//                (*child)->setParent(newContext->candidateNode());

//            // Update new node status
//            switch (itSequence->checkResult)
//            {
//                case crExactMatch:
//                    newContext->candidateNode()->setStatus(nsFloating);
//                    break;
//                case crPartialMatch:
//                    newContext->candidateNode()->setStatus(nsPossible);
//                    newContext->candidateNode()->setOrigSequence(itSequence->sequence);
//                    newContext->candidateNode()->setOrigPattern(&pattern);
//                    break;
//                default:
//                    break;
//            }

//            // Add to tree only after status changing!
//            newContext->outputTree().add(newContext->candidateNode());

//            // Remember modification
//            //context.modification().push_back(context.candidateNode());

//            // Update result, advance current roots position
//            newContext->advanceCurrentRoot(seq.size());
//            m_result.nodesAdded += 1;
//            newContext->setCandidateNode(new Node());

//            newAnalysisBranch(newContext);
//        }
//    }
//}

//void DynamicMultiPass::newAnalysisBranchForTree(FL::Trees::Tree &tree, int indentSize)
//{
//    tree.validateStructure();
//    Context *context = NULL;

//    //Tree *cTree = tree.copy();
//    Tree *cTree = &tree;
//    // Initialize analyse context
//    //do
//    {
//        context = new Context();
//        context->setTimeSeries(m_ts);
//        context->setParseTree(cTree);
//        context->setOutputTree(cTree->copy());
//        context->setCandidateNode(new Node());
//        context->advanceCurrentRoot(context->roots().size() - indentSize);

//        newAnalysisBranch(context);

//      //  cTree = cTree->detachLevelLastNodes(indentSize);
//    } //while (cTree->levelCount() > 1);

//    //delete cTree;
//}

//void DynamicMultiPass::newAnalysisBranch(Patterns::Context *context)
//{
//    if ((int)m_newForest.size() < m_maxTrees)
//        m_branches.push_back(context);
//    else
//    {
//        delete context->candidateNode();
//        delete &context->outputTree();
//        delete context;
//    }
//}

//bool DynamicMultiPass::isSubtreeOfSomeTree(Trees::Tree *tree)
//{
//    Forest::Iterator itTree;
//    bool isTreeUnique = true;

//    for (itTree = m_newForest.begin(); itTree != m_newForest.end(); )
//    {
//        if (*itTree == tree)
//        {
//            ++itTree;
//            continue;
//        }

//        TreeCompareResult tcr = (*itTree)->compare(*tree);

//        // Is this tree is subtree of another tree
//        if (tcr.isSecondSubtreeOfFirst())
//        {
//            isTreeUnique = false;
//            break;
//        }

//        // Is another tree is subtree of this
//        if (tcr.isFirstSubtreeOfSecond())
//        {
//            delete *itTree;
//            itTree = m_newForest.erase(itTree);
//            m_result.treesAdded--;
//        }
//        else
//            ++itTree;
//    }

//    if (isTreeUnique)
//    {
//        m_result.treesAdded++;
//        return false;
//    }
//    else
//        return true;
//}

////! Make time series forecast
//void DynamicMultiPass::forecastTimeSeries(
//    const TimeSeries &ts,
//    Trees::Forest &forest,
//    Patterns::PatternsSet &patterns,
//    FL::Forecast &forecast)
//{
//    forecast.clear();

//    /*
//    int up   = IDGenerator::idOf("up");
//    int down = IDGenerator::idOf("down");
//    int a    = IDGenerator::idOf("a");
//    int b    = IDGenerator::idOf("b");
//    */


//    //double lastValue = ts.values()[ts.size()-1];
//    //double lastTime = ts.size()-1;

//    Forest::Iterator itTree;
//    forall(itTree, forest)
//    {
//        Tree *tree = *itTree;
//        const Layer &possibleNodes = tree->possibleNodes();

//        Layer::ConstIterator itNode;
//        forall(itNode, possibleNodes)
//        {
//            Node *node = *itNode;
//            if (node->status() != nsPossible) //BUG
//                continue;

//            FL::ForecastItem nodeForecast;

//            // Forecast based on max/min guards values
//            if (!node->origPattern()->calcGuardLimits(
//                        node, *tree, const_cast<TimeSeries&>(ts), nodeForecast))
//                continue;

//            // Get next symbol, if it's known - fill its characteristics
//            /*
//            int nextSymbol = (*node->origSequence())[ node->children().size() ].id;
//            if (nextSymbol == up || nextSymbol == a)
//            {
//                if (nodeForecast.maxValue < 0)
//                    continue;
//                nodeForecast.minValue = 0;
//            }
//            else if (nextSymbol == down || nextSymbol == b)
//            {
//                if (nodeForecast.minValue > 0)
//                    continue;
//                nodeForecast.maxValue = 0;
//            }
//            */

//            nodeForecast.pos = node->end();
//            nodeForecast.tree = tree;

//            forecast.push_back(nodeForecast);
//        }
//    }
//}
