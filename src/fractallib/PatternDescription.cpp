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
#include "PatternDescription.h"

using namespace FL;
using namespace FL::Patterns;

std::vector<ParseTreeNode*>& CheckContext::getNodes(
        int nameId, int no, ParseTree::Layer &nodes) const
{
    m_cashedNodes.clear();
    ParseTree::Layer::const_iterator i;
    if (nameId != UniqueNamer::WILDCARD)
    {
        for (i = nodes.begin(); i != nodes.end(); ++i)
            if ((*i)->no == no && (*i)->nameId == nameId)
                m_cashedNodes.push_back(*i);
    }
    else
    {
        for (i = nodes.begin(); i != nodes.end(); ++i)
            if ((*i)->no == no)
                m_cashedNodes.push_back(*i);
    }
    return m_cashedNodes;
}

std::vector<ParseTreeNode*>& CheckContext::getNodes(int nameId, int no) const
{
    return getNodes(nameId, no, lastParsed);
}

ParseTreeNode* CheckContext::getNode(
        int nameId, int no, ParseTree::Layer &nodes) const
{
    ParseTree::Layer::const_iterator i;
    for (i = nodes.begin(); i != nodes.end(); ++i)
        if ((*i)->no == no && (*i)->nameId == nameId)
            return *i;
    return NULL;
}

ParseTreeNode* CheckContext::getNode(int nameId, int no) const
{
    return getNode(nameId, no, lastParsed);
}
