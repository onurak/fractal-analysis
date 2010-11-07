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

    // make copy of tree
    tree = new ParseTree(treeSet);

    #ifdef FL_ENABLE_PARSE_TREE_OPTIMIZATION
        c.tree->copyTo(tree);
        roots = new ParseTree::ConstLayer(*c.roots);
    #else
        std::map<ParseTreeNode*, ParseTreeNode*> assoc;
        c.tree->copyTo(tree, &assoc);
        ParseTree::ConstLayer::const_iterator node;
        roots = new ParseTree::ConstLayer();
        ParseTree::Layer *tmpRoots = const_cast<ParseTree::Layer*>(roots);
        for_each_(node, *c.roots)
            tmpRoots->push_back(assoc[*node]);
        iRoot = std::find(roots->begin(), roots->end(), assoc[*c.iRoot]);
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
            roots, iRoot, modification, lastParsed, synonyms, parameters, ts);
}
