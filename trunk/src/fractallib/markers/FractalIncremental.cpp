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

#include "FractalIncremental.h"
#include "../parsers/SinglePass.h"

using namespace FL::Markers;
using namespace FL::Exceptions;
using namespace FL::Trees;
using namespace FL::Patterns;


FractalIncremental::FractalIncremental()
{
}

FractalIncremental::~FractalIncremental()
{
}

//FL::ParseResult FractalIncremental::analyze(
//    const TimeSeries &ts, Trees::Forest &forest, Patterns::PatternsSet &patterns)
//{
//    ParseResult result;

//    try
//    {
//        if (ts.values().size() < 2)
//            throw EAnalyze(E_INVALID_INPUT);
//        if (forest.size() == 0)
//            throw EAnalyze(E_INVALID_INPUT);

//        const int up   = IDGenerator::idOf("a");
//        const int down = IDGenerator::idOf("b");
//        int begin, end;

//        Forest::Iterator itTree;
//        forall(itTree, forest)
//        {
//            // Markup last non-marked up segment of time series with "a" and "b"
//            Tree *tree = *itTree;
//            tree->validateStructure();
//            tree->getSegment(begin, end);
//            begin = end;
//            end = ts.values().size()-1;

//            for (int i = begin; i < end; i += 1)
//            {
//                const int &id =  (ts.values()[i] <= ts.values()[i+1])  ?  up  :  down;
//                tree->add(new Node(NULL, id, i, i+1, 0, nsFloating));
//            }

//            // Apply patterns to floating time series segment
//            growTree(ts, *tree, patterns, tree->dynamicEndPos(), end);
//        }
//    }
//    catch (const EAnalyze &e)
//    {
//        m_lastError = e;
//    }

//    return result;
//}

//void FractalIncremental::growTree(const TimeSeries &ts, Trees::Tree &tree,
//              Patterns::PatternsSet &patterns, int begin, int end)
//{
//    while (growLayer(ts, tree, patterns, begin, end))
//        ;
//}

//bool FractalIncremental::growLayer(const TimeSeries &ts, Trees::Tree &tree,
//              Patterns::PatternsSet &patterns, int begin, int end)
//{
//    bool result = false;

//    // Initialize iterators
//    PatternsSet::Iterator pattern;

//    // Initialize analyse context
//    Context context;
//    context.setTimeSeries(&ts);
//    context.setParseTree(&tree);
//    context.setOutputTree(&tree);
//    context.setCandidateNode(new Node());
//    context.candidateNode()->setStatus(nsFloating);

//    // Change real roots of tree to nodes from level 0
//    context.setCustomRoots(tree.nodesByLevel(0));
//    context.advanceCurrentRootToPos(begin);

//    // Look for applicable patterns in each position of tree
//    while (!context.isAt(end))
//    {
//        bool found = false;
//        forall(pattern, patterns)
//        {
//            if (applyPattern(**pattern, context))
//            {
//                result = found = true;
//                break;
//            }
//        }

//        if (!found)
//            context.advanceCurrentRoot(1);
//    }
//    delete context.candidateNode();
//    return result;
//}

//bool FractalIncremental::applyPattern(Pattern &pattern, Context &context)
//{
//    CheckInfo info;
//    if (pattern.check(context, info) == crOK)
//    {
//        // Pattern sequence that was applied (use only one)
//        CISequence &seq = *(info.applicableSequences[0].sequence);

//        // Insert candidate node into output tree instead of it's children
//        context.buildLastParsed(seq);
//        context.candidateNode()->setLevel(0);
//        Node *parent = context.lastParsed().front()->parent();

//        Layer::Iterator child;
//        forall(child, context.lastParsed())
//        {
//            (*child)->setParent(NULL);
//            context.outputTree().remove(*child);
//            delete *child;

//        }
//        context.candidateNode()->setParent(parent);
//        context.outputTree().add(context.candidateNode());

//        // Update parent nodes of new node.
//        // If guards of parents are not executable anymore - rollback merging
//        while (parent != NULL)
//        {
//            parent->setEnd(context.candidateNode()->end());
//            parent = parent->parent();
//        }

//        // Update advance current roots position
//        context.advanceCurrentRoot(seq.size());
//        context.setCandidateNode(new Node());
//        context.candidateNode()->setStatus(nsFloating);

//        return true;
//    }

//    return false;
//}
