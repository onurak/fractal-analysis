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

#include "ABIncremental.h"
#include "../parsers/SinglePass.h"

using namespace FL::Markers;
using namespace FL::Exceptions;
using namespace FL::Trees;
using namespace FL::Patterns;


ABIncremental::ABIncremental()
{
}

ABIncremental::~ABIncremental()
{
}

FL::ParseResult ABIncremental::analyze(
    const TimeSeries &ts,
    Trees::Forest &forest,
    Patterns::Matcher &matcher,
    Patterns::PatternsSet &patterns)
{
    ParseResult result;

    try
    {
        if (ts.size() < 2)
            throw EAnalyze(E_INVALID_INPUT);
        if (forest.size() == 0)
            throw EAnalyze(E_INVALID_INPUT);

        const int up   = IDGenerator::idOf("a");
        const int down = IDGenerator::idOf("b");
        int begin, end;

        Forest::Iterator itTree;
        for (itTree = forest.begin(); itTree != forest.end(); )
        {
            // Markup last non-marked up segment of time series with "a" and "b"
            Tree *tree = *itTree;

            // Remember which symbols was the last
            Node *oldLastSymbol = tree->nodesByLevel(0).back();

            begin = tree->getLastUnmarkedSegment();
            end   = ts.size()-1;

            for (int i = begin; i < end; i += 1)
            {
                const int &id =  ts.value(i) <= ts.value(i+1)  ?  up  :  down;
                tree->add(new Node(NULL, id, i, i+1, 0, nsFinished));
            }

            // Apply patterns to floating time series segment
            int offset = std::min(end-patterns.maxLength(), 0);
            growTree(ts, *tree, matcher, offset, end, patterns, *oldLastSymbol);

            // If there are no top-level nodes in tree then delete it
            if (tree->levelCount() < 2)
            {
                delete tree;
                itTree = forest.erase(itTree);
            }
            else
                ++itTree;

        }
    }
    catch (const EAnalyze &e)
    {
        m_lastError = e;
    }

    return result;
}

void ABIncremental::growTree(
    const TimeSeries &ts, Trees::Tree &tree,
    Matcher &matcher, int begin, int end,
    Patterns::PatternsSet &patterns,
    Trees::Node oldLastSymbol)
{
    // Grow and clamp tree at zero level
    while (growLayer(ts, tree, matcher, begin, end, patterns))
        ;

    // Check that new symbol is the next in EBNF
    const Layer& leafs = tree.nodesByLevel(0);

    Node *newLastSymbol = leafs.lastNode();

    if (oldLastSymbol.id() == newLastSymbol->id() &&
        oldLastSymbol.begin() == newLastSymbol->begin())
        return;

    // Update unfinished nodes of higher level.
    // Only one such node could exists.
    // If some node must be updated then all its parents are unfinished
    // nodes too, but they can't be updated until this node is unfinished.
    // So the algorithm:
    // 1. Find first unfinished node
    // 2. Try to finish it. If it finished ok then recheck all its parents guards.
    //    If not then delete it with all parents.

    Context context;
    context.setTimeSeries(&ts);
    context.setParseTree(&tree);
    context.setOutputTree(&tree);

    int levelCount = tree.levelCount();
    int level = 1;

    bool unfinished_found_and_successfuly_updated = false; // ouch!

    while (level < levelCount)
    {
        Node *unfinished = tree.getUnfinishedNode(level);

        // ok, there is unfinished node on this level.
        if (unfinished != NULL)
        {
            // Find the symbol that is expected to be the next in node's EBNF
            size_t ebnfLastPos = unfinished->children().size();
            CINode expectedNode = unfinished->origSequence().at(ebnfLastPos);

            // Compare it to parsed symbol
            if (expectedNode.id != newLastSymbol->id())
            {
                eraseNode(tree, unfinished);
                break;
            }

            // Add new symbol to EBNF and recheck guard
            newLastSymbol->setParent(unfinished);
            unfinished->setEnd(newLastSymbol->end());

            if (!recheckNodeGuard(context, unfinished))
            {
                eraseNode(tree, unfinished);
                break;
            }

            // Recheck parents
            Node *invalidParent = recheckNodeParents(context, unfinished);
            if (invalidParent != NULL)
                eraseNode(tree, invalidParent);

            // We found unfinished node on this level so there is can't be any
            // other unfinished on higher level - break
            unfinished_found_and_successfuly_updated = true;
            break;
        }
        level++;
    }
}


bool ABIncremental::growLayer(
    const TimeSeries &ts, Trees::Tree &tree,
    Matcher &matcher, int begin, int end,
    Patterns::PatternsSet &patterns)
{
    bool result = false;

    // Initialize analyse context
    Context context;
    context.setTimeSeries(&ts);
    context.setParseTree(&tree);
    context.setOutputTree(&tree);
    context.setCandidateNode(new Node());

    // Change real roots of tree to nodes from level 0
    context.setRoots(tree.nodesByLevel(0));
    context.advanceCurrentRootToPos(begin);

    // Look for applicable patterns in each position of tree
    while (!context.isAtTime(end))
    {
        if (!match(matcher, context, patterns))
            context.advanceCurrentRoot(1);
        else
            result = true;
    }
    delete context.candidateNode();
    return result;
}

bool ABIncremental::match(Matcher &matcher, Context &context,
                          Patterns::PatternsSet &patterns)
{
    bool result;
    CheckInfo ci;

    if ((result = matcher.match(context, ci)) == true)
    {
        insertNode(context, ci, patterns);

        // Update result, advance current roots position
        context.advanceCurrentRoot(ci.applicableSequences[0].seq->size());
        context.setCandidateNode(new Node());
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


void ABIncremental::insertNode(Patterns::Context &context, Patterns::CheckInfo &ci,
                               Patterns::PatternsSet &patterns)
{
    // Pattern sequence that was applied (use only first one)
    CISequence &seq = *ci.applicableSequences[0].seq;
    context.buildLastParsed(seq);
    Layer::Iterator child;

    Node *parent = NULL;

    // If nodes among last parsed have different parents then
    // new node cannot be inserted. Also get new parent for
    // newly inserted node

    // Find first non-NULL parent and compare it to other non-NULLs.
    // NULL parents don't really a problem.

    forall(child, context.lastParsed())
    {
        if ((*child)->parent() != NULL)
        {
            parent = (*child)->parent();
            break;
        }
    }

//    // Compare this non-NULL to rest of non-NULLs
//    Layer::Iterator child1 = child;
//    for (++child1; child1 != context.lastParsed().end(); ++child1)
//    {
//        if ((*child1)->parent() != NULL && (*child1)->parent() != parent)
//            return false;
//    }


    // Insert candidate node into output tree
    Node *candidate = context.candidateNode();
    candidate->setId(ci.applicableSequences[0].pattern->id());

    forall(child, context.lastParsed())
    {
        context.outputTree().remove(*child);
        (*child)->setParent(NULL);
        delete *child;
    }

    candidate->setLevel(0);
    candidate->setParent(parent);
    context.outputTree().add(candidate);


    // Now check that parent nodes are still valid.
    // If some parent is not valid - remove it with its own
    // parents too
    Node *invalidParent = recheckNodeParents(context, candidate);
    if (invalidParent != NULL)
        eraseNode(context.parseTree(), invalidParent);
}

bool ABIncremental::recheckNodeGuard(Patterns::Context &context, Trees::Node *node)
{
    // Find pattern with the same id
    Pattern* origPattern = PatternsRegistry::patternById(node->id());
    if (origPattern == NULL)
        throw EAnalyze(E_INVALID_PATTERNS);

    // Create fake context for parent analysis
    Context fakeContext;
    fakeContext.setTimeSeries(context.timeSeries());
    fakeContext.setParseTree(&context.parseTree());
    fakeContext.setOutputTree(&context.outputTree());
    fakeContext.setCandidateNode(node);

    // Set roots of fake context to
    // 1. First all nodes of the same level as current node that are BEFORE current node;
    // 2. Then children of current node.
    // This roots are just the same as they were when current node was created.
    Layer rootsLayer;

    // 1.
    const Layer& nodeLevelNodes =
            fakeContext.outputTree().nodesByLevel(node->level());

    Layer::ConstIterator iNode;
    forall(iNode, nodeLevelNodes)
    {
        if (*iNode == node) break;
        rootsLayer.push_back(*iNode);
    }

    // 2.
    forall(iNode, node->children())
        rootsLayer.push_back(*iNode);

    fakeContext.setRoots(rootsLayer);

    // Build last parsed sequence as children of current parent
    fakeContext.buildLastParsed(
                node->origSequence(),
                node->children().size(),
                &node->children());

    // Check the guard
    return origPattern->guard()->check(fakeContext);
}

Node* ABIncremental::recheckNodeParents(Patterns::Context &context, Trees::Node *node)
{
    Node *parent = node->parent();

    // Now check that parent nodes are still valid.
    // If some parent is not valid - remove it with its own
    // parents too
    while (parent != NULL)
    {
        // Expression candidate->setParent(parent) updates parameters of direct
        // parent of node only (for speed reasons), so we need to update
        // indirect parents manually
        parent->setEnd(node->end());

        // Check the guard. If it is invalid then erase
        // this node with all its parents
        if (!recheckNodeGuard(context, parent))
            return parent;

        parent = parent->parent();
    }

    return NULL;
}

void ABIncremental::eraseNode(Tree &tree, Node *node)
{
    Node *parent = node->parent();

    if (parent != NULL)
    {
        node->setParent(NULL);
        eraseNode(tree, parent);
    }

    tree.remove(node);
    delete node;
}
