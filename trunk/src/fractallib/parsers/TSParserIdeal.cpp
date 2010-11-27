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

#include "TSParserIdeal.h"

using namespace FL;

ParseResult TSParserIdeal::parse(ParseTreeSet &trees,
                                 PatternCollection &patterns,
                                 FL::TimeSeries *ts,
                                 FL::Patterns::GroupChecker &checker,
                                 int tsBegin,
                                 int tsEnd,
                                 WorkMode mode)
{
    ParseResult result;
    if (trees.size() == 0)
    {
        GError(GCritical, "TSParserIdeal", 0, "No parse trees");
        return result;
    }
    if (patterns.size() == 0)
    {
        GError(GCritical, "TSParserIdeal", 0, "No patterns loaded");
        return result;
    }
    if (ts == NULL)
    {
        GError(GCritical, "TSParserIdeal", 0, "No time series loaded");
        return result;
    }
    if (tsEnd == -1)
        tsEnd = ts->dimSize(0)-1;
    if (tsBegin < 0 || tsBegin > tsEnd || tsEnd > ts->dimSize(0)-1)
    {
        GError(GCritical, "TSParserIdeal", 0, "Invalid interval to parse");
        return result;
    }

    m_mode = mode;
    logg.debug("TSParserIdeal: begin analysis...");
    m_branchNo = 0;

    onProgress(0);

    // for each tree running new branch of analysis
    int baseCount = trees.size(); // number of trees at begining of analysis.
                                  // it's needed 'cause new trees could be added to "trees"
                                  // variable while analysis
    for (int i = 0; i < baseCount; ++i)
    {
        ParseTree *theTree = (ParseTree*)(trees[i]);
        logg.debug("Analysing tree ") << i+1 << " of " << baseCount << "( address " << theTree << ")";
        ParseContext *context = new ParseContext
        (
                &trees,
                theTree,
                ts,
                &patterns,
                &theTree->roots(),
                &patterns.synonyms,
                &patterns.parameters,
                &result
        );
        //DEBUG_NOTIFICATION(0, context);
        newBranch(context);
        logg.debug("End analysing tree number ") << i;
    }

    onProgress(100);

    // Remove old trees
    if (result.parsedCount > 0)
    {
        logg.debug("Deleting old trees... ");
        while (baseCount-- > 0)
        {
            delete trees.trees.front();
            trees.trees.erase(trees.trees.begin());
        }
        logg << "ok";
    }

    logg.debug("TSParserIdeal: end analisys seccussful");

    return result;
}

void TSParserIdeal::ranalyse(ParseContext *context, int no)
{
    PatternCollection::iterator t;
    int patternSize;
    ParseContext *newContext;

    // Do analysis
    logg.debug("TSParserIdeal: new analysis branch (") << no << ")";
    while (context->iRoot != context->roots->end())
    {
        for_each_(t, *(context->patterns))
        {
            if (applyPattern(*context, *t, patternSize, &newContext))
                newBranch(newContext);
            if (t != context->patterns->end()-1)
                logg.debug("Trying next pattern");
        }
        logg.debug("Move to one step");
        ++(context->iRoot);
    }

    // Check if new builded tree is subtree of some other tree
    logg.debug("Looking for identical trees... ");
    bool treeAlreadyInList = false;
    ParseTreeSet::Iterator  tree;
    for_each_(tree, context->treeSet->trees)
    {
        if (*tree != context->tree)
        {
            TreeCompareResult cmp = context->tree->compare((ParseTree*)(*tree));
            if (cmp.secondContainsFirst())
                break;
        } else {
            treeAlreadyInList = true;
            break;
        }
    }

    // If tree isn't subtree - add it
    if (tree == context->treeSet->trees.end())
    {
        logg << "not found! Tree will be added to resulting set";
        context->treeSet->add(context->tree);
        context->result->parsedCount++;
    }
    // Otherwise delete it (if tree should be deleted but it's in list than this tree is baseTree)
    else if (!treeAlreadyInList)
    {
        logg << "found! Tree will be deleted";
        delete context->tree;
        context->tree = NULL;
    }

    // context doesn't need anymore
    delete context;
    logg.debug("TSParserIdeal: analysis branch complited succesful (") << no << ")";
    m_completedBranches++;
}

bool TSParserIdeal::applyPattern(ParseContext &context, Pattern *p,
                                 int &patternSize, ParseContext **newContext)
{
    // check
    if (p->check(*context.cc) == Pattern::crOK)
    {
        logg.debug("Creating new node");
        // creating new node
        ParseTreeNode *newNode = new ParseTreeNode(
            p->name(),                      // node name = name of pattern
            NULL,                           // no parent
            -1,                             // level will be set later
            (*(context.iRoot))->tsBegin,    // begin of time series segment
            (*context.cc->lastNode)->tsEnd  // end of time series segment
        );
        logg.debug("Pattern position: ") << (*(context.iRoot))->tsBegin << "-"
                << (*context.cc->lastNode)->tsEnd;

        // Create copy of current context for new analysis branch
        *newContext = new ParseContext(context);

        // set parent for every node in pattern up to just created node,
        // looking for maximal level among them
        int maxLevel = -1;
        ParseTree::ConstLayer::const_iterator node = (*newContext)->iRoot,
                                              prevNode = context.iRoot;
        for (; prevNode != context.cc->lastNode + 1; ++node, ++prevNode)
        {
            (*newContext)->tree->setNodeParent(*node, newNode);
            newNode->children.push_back(*node);
            if ((*node)->level > maxLevel)
                maxLevel = (*node)->level;
        }
        (*newContext)->iRoot = node;
        newNode->level = maxLevel + 1;
        if ((*newContext)->iRoot == (*newContext)->roots->end() - 1)
            (*newContext)->iRoot++;

        // don't forget to add node to modification
        (*newContext)->modification.push_back(newNode);

        // add new node to tree
        (*newContext)->tree->addNode(newNode);
        return true;
    }
    return false;
}

void TSParserIdeal::newBranch(ParseContext *context)
{
    // Currently new branch running in the same thread recursevly.
    // Later multithreading support can be added
    logg.debug("Starting branch");
    ranalyse(context, m_branchNo++);
    logg.debug("Ending branch");
    onProgress(m_completedBranches != 0 ? (double(m_completedBranches) / m_branchNo)*100. : 0);
}
