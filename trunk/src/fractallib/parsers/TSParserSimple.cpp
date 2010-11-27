/* Author: Vladimir Belikov
 *
 * This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Fractal Libray.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TSParserSimple.h"

using namespace FL;

ParseResult TSParserSimple::parse(ParseTreeSet &trees,
                                  PatternCollection &patterns,
                                  FL::TimeSeries *ts,
                                  FL::Patterns::GroupChecker &checker,
                                  int tsBegin,
                                  int tsEnd,
                                  WorkMode mode)
{
    ParseResult result;
    if (patterns.size() == 0)
    {
        GError(GCritical, "TSParserSimple", 0, "No patterns loaded");
        return result;
    }
    if (trees.size() == 0)
    {
        GError(GCritical, "TSParserSimple", 0, "No parsed trees");
        return result;
    }
    if (ts == NULL)
    {
        GError(GCritical, "TSParserSimple", 0, "No TS loaded");
        return result;
    }
    if (trees.size() != 1)
    {
        GError(GCritical, "TSParserSimple", 0, "Can only parse single tree");
        return result;
    }

    if (tsEnd == -1)
        tsEnd = ts->dimSize(0)-1;

    if (tsBegin < 0 || tsBegin > tsEnd || tsEnd > ts->dimSize(0)-1)
    {
        GError(GCritical, "TSParserSimple", 0, "Invalid interval to parse");
        return result;
    }

    ParseTree *tree = (ParseTree*)(trees.trees.front());

    ParseTree::ConstLayer &roots = tree->roots(); // this layer of tree will be analyzed
    if (roots.size() == 0)
    {
        GError(GCritical, "TSParserSimple", 0, "Empty tree");
        return result;
    }

    ParseContext context(&trees, tree, ts, &patterns, &roots, &patterns.synonyms);

    logg.debug("TSParserSimple: begin...");

    // let's start...
    try
    {
        // trying to apply pattern in current point of time series
        while (context.iRoot != context.roots->end())
        {
            bool findOne = false; // true if some pattern was applied, false otherwise
            PatternCollection::iterator t;
            // looking for pattern
            for_each_(t, patterns)
            {
                int patternSize;
                ParseTreeNode *node = applyPattern(context, *t, patternSize);
                if (node)
                {
                    result.parsedCount++;
                    findOne = true;
                    context.iRoot += patternSize;
                    logg.debug("Pattern found! Advancing to ") << patternSize << " positions";
                    break;
                }
            }

            // no patterns were found - move to one position
            if (!findOne)
            {
                ++(context.iRoot);
                logg.debug("Patterns not found, moving to 1 position");
            }
        } // while

        logg.debug("TSParserSimple: parsed successfull");

    } catch (...) {
        GError(GCritical, "TSParserSimple", 0, "Some error occures");
    }

    return result;
}


ParseTreeNode* TSParserSimple::applyPattern(ParseContext &context, Pattern *p, int &patternSize)
{
    // check
    if (p->check(*context.cc) == Pattern::crOK)
    {
        patternSize = context.cc->patternSize;
        // create new node
        ParseTreeNode *newNode = new ParseTreeNode(
            p->name(),                        // name of node = name of applied pattern
            NULL,                             // no parent
            -1,                               // level will be set later
            (*(context.iRoot))->tsBegin,      // begin of time series segment
            (*context.cc->lastNode)->tsEnd);  // end of time series segment

        logg.debug("Pattern position: ") << (*(context.iRoot))->tsBegin << "-"
                << (*context.cc->lastNode)->tsEnd;
        int maxLevel = -1;

        // set for every node in pattern parent to just created node,
        // looking for maximal level among them
        ParseTree::Layer::const_iterator node;
        for (node = context.iRoot; node != context.cc->lastNode + 1; ++node)
        {
            context.tree->setNodeParent(*node, newNode);
            newNode->children.push_back(*node);
            if ((*node)->level > maxLevel)
                maxLevel = (*node)->level;
        }
        newNode->level = maxLevel + 1; // set new node's level to max+1 (it will be new maximum)

        context.modification.push_back(newNode);

        context.tree->addNode(newNode);
        return newNode;
    }
    return NULL;
}
