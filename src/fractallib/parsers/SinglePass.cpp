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

#include "SinglePass.h"

using namespace FL::Parsers;
using namespace FL::Patterns;
using namespace FL::Exceptions;
using namespace FL::Trees;

SinglePass::SinglePass()
{
}

FL::ParseResult SinglePass::analyze(
    const TimeSeries &ts, Forest &forest, PatternsSet &patterns,
    Trees::MetricsSet &metrics,
    int begin, int end)
{
    m_result.reset();
    int maxDepth = 10;

    try
    {
        if (ts.values().size() < 2)
            throw EAnalyze(E_EMPTY_TIME_SERIES);
        if (forest.size() == 0)
            throw EAnalyze(E_EMPTY_FOREST);
        if (patterns.size() == 0)
            throw EAnalyze(E_EMPTY_PATTERNS);
        if (begin >= (int)ts.values().size() ||
            end >= (int)ts.values().size() ||
            (end != -1 && begin >= end))
            throw EAnalyze(E_INVALID_SEGMENT);

        m_begin = begin;
        if (end > 0)
            m_end = end;
        else
            m_end = ts.values().size()-1;

        int prevMaxLevel = forest.maxLevelCount(), newMaxLevel;

        do
        {
            Forest::Iterator tree;
            forall(tree, forest)
            {
                (*tree)->validateStructure();
                analyzeTree(ts, **tree, patterns);
            }

            newMaxLevel = forest.maxLevelCount();
            if (prevMaxLevel < newMaxLevel && newMaxLevel <= maxDepth)
                prevMaxLevel = newMaxLevel;
            else
                break;
        } while (true);
    }
    catch (const EAnalyze &e)
    {
        m_lastError = e;
        m_lastError.setArg(descriptionOf(e.id()));
    }

    return m_result;
}

void SinglePass::analyzeTree(const TimeSeries &ts, Tree &tree, PatternsSet &patterns)
{
    // Initialize iterators
    PatternsSet::Iterator pattern;

    // Initialize analyse context
    Context context;
    context.setTimeSeries(&ts);
    context.setParseTree(&tree);
    context.setOutputTree(&tree);
    context.setCandidateNode(new Node());
    context.advanceCurrentRootToPos(m_begin);

    // Look for applicable patterns in each position of tree
    while (!context.isAt(m_end))
    {
        bool found = false;
        forall(pattern, patterns)
        {
            if (applyPattern(**pattern, context))
            {
                found = true;
                break;
            }
        }

        if (!found)
            context.advanceCurrentRoot(1);
    }
    delete context.candidateNode();
}

bool SinglePass::applyPattern(Pattern &pattern, Context &context)
{
    CheckInfo info;
    if (pattern.check(context, info) == crOK)
    {
        // Pattern sequence that was applied (use only one)
        CISequence &seq = *(info.applicableSequences[0].sequence);

        // Insert candidate node into output tree
        context.buildLastParsed(seq);
        Layer::Iterator child;
        forall(child, context.lastParsed())
            (*child)->setParent(context.candidateNode());
        context.outputTree().add(context.candidateNode());

        // Remember modification
//        context.modification().push_back(context.candidateNode());

        // Update result, advance current roots position
        context.advanceCurrentRoot(seq.size());
        m_result.nodesAdded += 1;
        context.setCandidateNode(new Node());

        return true;
    }

    return false;
}
