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
        forall(itTree, forest)
        {
            // Markup last non-marked up segment of time series with "a" and "b"
            Tree *tree = *itTree;

            begin = tree->getLastUnmarkedSegment();
            end   = ts.size()-1;

            for (int i = begin; i < end; i += 1)
            {
                const int &id =  ts.value(i) <= ts.value(i+1)  ?  up  :  down;
                tree->add(new Node(NULL, id, i, i+1, 0, nsFloating));
            }

            // Apply patterns to floating time series segment
            growTree(ts, *tree, matcher, tree->floatingBegin(), end);
            //growTree(ts, *tree, matcher, begin-3, end);
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
    Matcher &matcher, int begin, int end)
{
    while (growLayer(ts, tree, matcher, begin, end))
    {

    }
}

void ABIncremental::clamp(Trees::Tree &tree, Trees::Layer &nodes)
{
    FL::Trees::Layer::Iterator inode, ichild;
    forall(inode, nodes)
    {
        FL::Trees::Node *node = *inode;

        forall(ichild, node->children())
        {
            FL::Trees::Node *cnode = *ichild;
            tree.remove(cnode);
            delete cnode;
        }

        node->children().clear();
        tree.updateLevel(node, 0);
    }
}

bool ABIncremental::growLayer(
    const TimeSeries &ts, Trees::Tree &tree,
    Matcher &matcher, int begin, int end)
{
    bool result = false;

    // Initialize analyse context
    Context context;
    context.setTimeSeries(&ts);
    context.setParseTree(&tree);
    context.setOutputTree(&tree);
    context.setCandidateNode(new Node());
    context.candidateNode()->setStatus(nsFloating);

    // Change real roots of tree to nodes from level 0
    context.setRoots(tree.nodesByLevel(0));
    context.advanceCurrentRootToPos(begin);

    // Look for applicable patterns in each position of tree
    while (!context.isAtTime(end))
    {
        if (!match(matcher, context))
            context.advanceCurrentRoot(1);
        else
            result = true;
    }
    delete context.candidateNode();
    return result;
}

bool ABIncremental::match(Matcher &matcher, Context &context)
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
        {
            //(*child)->setParent(candidate);
            context.outputTree().remove(*child);
            (*child)->setParent(NULL);
            delete *child;
        }

        candidate->setLevel(0);
        context.outputTree().add(candidate);

        candidate->setId(ci.applicableSequences[0].pattern->id());

        // Remember modification
        //context.modification().push_back(context.candidateNode());

        // Update result, advance current roots position
        context.advanceCurrentRoot(seq.size());
        context.setCandidateNode(new Node());
        context.candidateNode()->setStatus(nsFloating);

        result = true;
    }

    return result;
}
