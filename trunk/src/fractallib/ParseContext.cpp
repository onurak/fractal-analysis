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

#include "ParseContext.h"

using namespace FL;

ParseContext::ParseContext(ParseContext &c)
{
    // copy common data pointers
    treeSet = c.treeSet;
    ts = c.ts;
    synonyms = c.synonyms;
    patterns = c.patterns;
    result = c.result;
    candidateNode = c.candidateNode;

    // make copy of tree
    tree = new ParseTree(treeSet);

    #ifdef FL_ENABLE_PARSE_TREE_OPTIMIZATION
        c.tree->copyTo(tree);
        roots = new ParseTree::ConstLayer(*c.roots);
    #else
        // make tree copy
        std::map<ParseTreeNode*, ParseTreeNode*> assoc;
        c.tree->copyTo(tree, &assoc);
        ParseTree::ConstLayer::const_iterator node;

        // make roots copy
        roots = new ParseTree::ConstLayer();
        ParseTree::Layer *tmpRoots = const_cast<ParseTree::Layer*>(roots);
        for_each_(node, *c.roots)
            tmpRoots->push_back(assoc[*node]);
        iRoot = std::find(roots->begin(), roots->end(), assoc[*c.iRoot]);

        // make modification copy
        for_each_(node, c.modification)
            modification.push_back(assoc[*node]);
    #endif

    m_ownRoots = true;


    // point iRoot iterator on the same element
    if (roots && roots->size())
    {
        ParseTree::Layer::const_iterator tmpRoot = c.roots->begin();
        iRoot = roots->begin();
        while (tmpRoot != c.iRoot)
            ++iRoot, ++tmpRoot;
    }

    // create check context
    cc = new FL::Patterns::CheckContext(
            tree, roots, iRoot, modification, candidateNode, lastParsed,
            synonyms, parameters, ts);
}

std::vector<ParseTreeNode*>& ParseContext::getNodes(const std::string &name, int no) const
{
    m_cashedNodes.clear();
    ParseTree::Layer::const_iterator i;
    if (name != "*")
    {
        for (i = lastParsed.begin(); i != lastParsed.end(); ++i)
            if ((*i)->no == no && (*i)->name == name)
                m_cashedNodes.push_back(*i);
    }
    else
    {
        for (i = lastParsed.begin(); i != lastParsed.end(); ++i)
            if ((*i)->no == no)
                m_cashedNodes.push_back(*i);
    }
    return m_cashedNodes;
}

ParseTreeNode* ParseContext::getNode(const std::string &name, int no) const
{
    ParseTree::Layer::const_iterator i;
    for (i = lastParsed.begin(); i != lastParsed.end(); ++i)
        if ((*i)->no == no && (*i)->name == name)
            return *i;
    return NULL;
}
