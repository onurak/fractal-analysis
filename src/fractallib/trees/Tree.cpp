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

#include "Tree.h"

using namespace FL::Trees;

Tree::Tree()
{
    m_virtualRoot = new Node(NULL, -1, -1, -1, -1);
}

Tree::~Tree()
{
    clear();
    delete m_virtualRoot;
}

void Tree::clear()
{
    // Delete root links
    Layer::Iterator node;
    forall(node, m_virtualRoot->children())
    {
        (*node)->m_parent = NULL;
        (*node)->m_internalParent = NULL;
        (*node)->m_children->clear();
    }
    m_virtualRoot->children().clear();

    // Delete all nodes
    forall(node, m_allNodes)
    {
        (*node)->m_parent = NULL;
        (*node)->m_internalParent = NULL;
        (*node)->m_children->clear();
        delete *node;
    }
    m_allNodes.clear();

    // Clear cache
    m_nodesByLevel.clear();
    m_possibleNodes.clear();
}

Tree* Tree::copy() const
{
    Tree *tree = new Tree();

    Layer::ConstIterator node;

    // Create nodes
    forall(node, m_allNodes)
    {
        Node *newNode = new Node(
                NULL, (*node)->id(),
                (*node)->begin(), (*node)->end(), (*node)->level(),
                (*node)->status());
        tree->m_allNodes.push_back(newNode);
        tree->m_nodesByLevel[newNode->level()].push_back(newNode);
        newNode->m_relativeNode = *node;
        (*node)->m_relativeNode = newNode;
    }

    // Create relations between nodes
    forall(node, tree->m_allNodes)
    {
        Node *originalNode = (*node)->m_relativeNode;
        if (originalNode->m_parent)
        {
            (*node)->m_parent = originalNode->m_parent->m_relativeNode;
            (*node)->m_parent->children().push_back(*node);
        }
    }

    // Remember virtual node's children
    forall(node, m_virtualRoot->children())
    {
        Node *originalNode = (*node)->m_relativeNode;
        tree->m_virtualRoot->children().push_back(originalNode);
        originalNode->m_internalParent = tree->m_virtualRoot;
    }

    // Remember "possible" nodes
    forall(node, m_possibleNodes)
    {
        Node *originalNode = (*node)->m_relativeNode;
        originalNode->setOrigPattern((*node)->origPattern());
        originalNode->setOrigSequence((*node)->origSequence());
        tree->m_possibleNodes.push_back(originalNode);
    }

    return tree;
}

void Tree::add(Node *node)
{
    if (node)
    {
        // Add to m_allNodes
        m_allNodes.push_back(node);

        // Add to roots if it is root
        if (node->parent() == NULL)
        {
            node->m_internalParent = m_virtualRoot;
            insertChild(m_virtualRoot, node);
        }
        else
        {
            node->m_internalParent = NULL;
        }

        // Add to possible cache
        if (node->status() == nsPossible)
            m_possibleNodes.push_back(node);

        // Add to leveled cache
        m_nodesByLevel[node->level()].push_back(node);
    }
}

void Tree::changeNodeStatus(Node *node, NodeStatus newStatus)
{
    if (node && node->status() != newStatus)
    {
        if (node->status() == nsPossible)
        {
            Layer::Iterator i;
            if (FL::search(m_possibleNodes, node, i))
                m_possibleNodes.erase(i);
        }

        node->setStatus(newStatus);

        if (node->status() == nsPossible)
        {
            if (!FL::exists(m_possibleNodes, node))
                m_possibleNodes.push_back(node);
        }
    }
}

void Tree::insertChild(Node *dst, Node *src)
{
    // Children of dst must be sorted by position in time series
    Layer::Iterator node;
    forall(node, dst->children())
    {
        if ((*node)->begin() >= src->end())
            break;
    }
    dst->children().insert(node, src);
}

void Tree::remove(Node *node)
{
    if (node)
    {
        Layer::Iterator i;

        // First remove it's parent and connections with children
        if (node->parent())
        {
            //node->parent()->children().remove(node);
            remove(node->parent());
        }

        // It could be among roots
        if (search(m_virtualRoot->children(), node, i))
        {
            m_virtualRoot->children().erase(i);

            forall(i, node->children())
            {
                (*i)->m_internalParent = m_virtualRoot;
                insertChild(m_virtualRoot, *i);
            }
        }

        while (node->children().size() > 0)
            node->children().front()->setParent(NULL);

        // Remove from m_allNodes and from caches
        if (search(m_allNodes, node, i))
        {
            m_allNodes.erase(i);

            if (search(m_nodesByLevel[node->level()], node, i))
                m_nodesByLevel[node->level()].erase(i);
        }

        // It could be possible node
        if (search(m_possibleNodes, node, i))
            m_possibleNodes.erase(i);

        node->m_internalParent = NULL;
    }
}

const Layer& Tree::roots() const
{
    // Virtual parent root stores all physical roots to faster access
    return m_virtualRoot->children();
}

const Layer& Tree::leafs() const
{
    return m_nodesByLevel[0];
}

const Layer& Tree::nodesByLevel(int level) const
{
    return m_nodesByLevel[level];
}

int Tree::levelCount() const
{
    std::map<int, Layer>::iterator i;
    for (i = m_nodesByLevel.begin(); i != m_nodesByLevel.end(); )
    {
        if (i->second.size() == 0)
            m_nodesByLevel.erase(i++);
        else
            ++i;
    }

            /*
    for (int lvl = 0; lvl < (int)m_nodesByLevel.size(); )
    {
        if (m_nodesByLevel.find(lvl) != m_nodesByLevel.end())
        {
            Layer &la = m_nodesByLevel[lvl];
            la.size();
        }

        if (m_nodesByLevel.find(lvl) != m_nodesByLevel.end() &&
            m_nodesByLevel[lvl].size() == 0)
        {
            m_nodesByLevel.erase(lvl);
        }
        else
            lvl++;
    }
            */

    return m_nodesByLevel.size();
}

void Tree::markupWithRoots(int begin, int end)
{
    Layer::Iterator node, i;

    // Delete possible nodes
    forall(node, m_possibleNodes)
    {
        if ( search(m_allNodes, *node, i) )
            m_allNodes.erase(i);
        if ( search( m_nodesByLevel[(*node)->level()], *node, i ) )
            m_nodesByLevel[(*node)->level()].erase(i);
        if ( search( m_virtualRoot->children(), *node, i ) )
            m_virtualRoot->children().erase(i);

        delete *node;
    }
    m_possibleNodes.clear();


    node = m_allNodes.begin();
    while ( node != m_allNodes.end() )
    {
        // Skip node if its out of range
        if ((*node)->begin() < begin || (end != -1 && (*node)->begin() > end))
        {
            ++node;
            continue;
        }

        // if node isn't root - erase it
        else if ( !exists(m_virtualRoot->children(), *node) )
        {
            if ( search( m_nodesByLevel[(*node)->level()], *node, i ) )
                m_nodesByLevel[(*node)->level()].erase(i);
            delete *node;
            node = m_allNodes.erase(node);
        }

        // else if node is a root - move it to 0 level (if necessary)
        else
        {
            if ((*node)->level() != 0)
            {
                if ( search( m_nodesByLevel[(*node)->level()], *node, i ) )
                    m_nodesByLevel[(*node)->level()].erase(i);
                (*node)->setLevel(0);
                (*node)->setStatus(nsFixed);
                m_nodesByLevel[0].push_back(*node);
            }
            ++node;
        }
    }
}

Node* Tree::haveNode(const Node *likeThis, bool ignoreZeroLevel) const
{
    FL::Trees::Layer::ConstIterator node;

    if (!ignoreZeroLevel)
    {
        forall(node, m_allNodes)
        {
            if ((*node)->m_id    == likeThis->m_id &&
                (*node)->m_begin == likeThis->m_begin &&
                (*node)->m_end   == likeThis->m_end &&
                (*node)->m_level == likeThis->m_level)
            {
                return *node;
            }
        }
    }
    else
    {
        for (int lvl = 1; lvl < (int)m_nodesByLevel.size(); ++lvl)
        {
            const Layer &level = m_nodesByLevel[lvl];

            forall(node, level)
            {
                if ((*node)->m_id    == likeThis->m_id &&
                    (*node)->m_begin == likeThis->m_begin &&
                    (*node)->m_end   == likeThis->m_end &&
                    (*node)->m_level == likeThis->m_level)
                {
                    return *node;
                }
            }
        }
    }

    return false;
}

TreeCompareResult Tree::compare(const Tree &tree) const
{
    TreeCompareResult tcr;
    Layer checked;
    FL::Trees::Layer::ConstIterator node;

    tcr.totalInFirst  = this->m_allNodes.size() - this->m_nodesByLevel[0].size();
    tcr.totalInSecond = tree. m_allNodes.size() - tree. m_nodesByLevel[0].size();

    // Compare first with second
    for (int lvl = 1; lvl < (int)m_nodesByLevel.size(); ++lvl)
    {
        const Layer &level = m_nodesByLevel[lvl];
        forall(node, level)
        {
            Node *identicalNode = tree.haveNode(*node, true);
            if (identicalNode)
            {
                tcr.commonNodes++;
                checked.push_back(identicalNode);
            }
            else
                tcr.uniqueInFirst++;
        }
    }

    // Compare second with first
    for (int lvl = 1; lvl < (int)tree.m_nodesByLevel.size(); ++lvl)
    {
        const Layer &level = tree.m_nodesByLevel[lvl];
        forall(node, level)
        {
            if (!exists(checked, *node))
                tcr.uniqueInSecond++;
        }
    }

    return tcr;
}

int Tree::bytesUsed() const
{
    int result = 0;
    result += m_allNodes.size() * sizeof(Node*) * sizeof(Node);

    Layer::ConstIterator node;
    forall(node, m_allNodes)
        result += (*node)->children().size() * sizeof(Node*);

    result += m_virtualRoot->children().size() * sizeof(Node*);
    result += sizeof(Node*);

    std::map<int, Layer>::const_iterator i;
    forall(i, m_nodesByLevel)
    {
        result += sizeof(Layer);
        result += i->second.size() * sizeof(Node*);
    }

    return result;
}

void Tree::getSegment(int &min, int &max) const
{
    min = max = -1;
    if (m_allNodes.size() > 0)
    {
        const Layer &l = roots();
        min = l.front()->begin();
        max = l.back()->end();
    }
}

void Tree::validateStructure()
{
    std::map<int, Layer>::iterator layer;
    forall(layer, m_nodesByLevel)
        layer->second.sort();
    m_allNodes.sortByTime();
}

void Tree::makeDynamicEnd(int borderPos)
{
    validateStructure();
    std::map<int, Layer>::iterator itLayer;
    forall(itLayer, m_nodesByLevel)
    {
        const Layer &layer = itLayer->second;

        Layer::const_reverse_iterator node;
        for (node = layer.rbegin(); node != layer.rend(); ++node)
        {
            if ((*node)->end() >= borderPos && (*node)->status() != nsPossible)
                (*node)->setStatus(nsFloating);
            else
                break;
        }
    }
}

int Tree::dynamicEndPos() const
{
    if (m_nodesByLevel.size() == 0)
        return 0;
    const Layer &level0 = m_nodesByLevel[0];
    Layer::const_reverse_iterator node;
    for (node = level0.rbegin(); node != level0.rend(); ++node)
    {
        if ((*node)->status() == nsFixed)
        {
            if (node == level0.rbegin())
                return (*node)->end();
            return (*(--node))->begin();
        }
    }
    return 0;
}

Layer Tree::getFollowingRoots(Node *node)
{
    Layer result;
    if (node)
    {
        Layer &roots = m_virtualRoot->children();
        Layer::iterator itFwd = roots.begin();
        while (itFwd != roots.end() && (*itFwd)->begin() < node->end())
            ++itFwd;
        if (itFwd != roots.end())
            while (itFwd != roots.end())
            {
                result.push_back(*itFwd);
                ++itFwd;
            }

        /*
        Layer::iterator         itFwd = roots.end();
        Layer::reverse_iterator itRev = roots.rbegin();

        --itFwd;
        while (*itRev != node && itRev != roots.rend())
            ++itRev, --itFwd;
        if (itRev != roots.rend())
            while (++itFwd != roots.end())
                result.push_back(*itFwd);
                */
    }
    return result;
}

void Tree::fixup()
{
    Layer::Iterator node;
    forall(node, m_allNodes)
    {
        if ((*node)->status() == nsFloating)
            (*node)->setStatus(nsFixed);
    }
}

Tree* Tree::detachLevelLastNodes(int count) const
{
    Tree *result = copy();
    Layer &lastLevel = result->m_nodesByLevel[levelCount()-1];

    while (count > 0 && lastLevel.size() > 0)
    {
        result->remove(lastLevel.back());
        count--;
    }
    return result;
}
