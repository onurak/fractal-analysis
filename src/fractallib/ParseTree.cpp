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

#include "ParseTree.h"

using namespace FL;

#ifdef FL_ENABLE_PARSE_TREE_OPTIMIZATION

ParseTree::ParseTree(ParseTreeSet *treeSet)
    : HyperTree<ParseTreeNode>::HyperTree(treeSet)
{
    m_minLevel = m_maxLevel = -1;
}

ParseTree::~ParseTree()
{
}

ParseTreeNode* ParseTree::hasNode(const ParseTreeNode *node, bool amongOwn, bool amongShared) const
{
    ConstLayer::const_iterator i;
    // itarate over own nodes
    for (i = m_ownNodes->cnodes().begin(); i != m_ownNodes->cnodes().end(); ++i)
    {
        if (//(*i)->no == node->no &&
            (*i)->level == node->level &&
            (*i)->tsBegin == node->tsBegin &&
            (*i)->tsEnd == node->tsEnd &&
            (*i)->name == node->name)
           return *i;
    }

    // iterate over shared nodes
    ParseTree::NodeGroupSet::const_iterator group;
    for (group = m_sharedNodes.begin(); group != m_sharedNodes.end(); ++group)
    {
        for (i = (*group)->cnodes().begin(); i != (*group)->cnodes().end(); ++i)
        {
            if (//(*i)->no == node->no &&
                (*i)->level == node->level &&
                (*i)->tsBegin == node->tsBegin &&
                (*i)->tsEnd == node->tsEnd &&
                (*i)->name == node->name)
               return *i;
        }
    }

    return NULL;
}

int ParseTree::hasGroup(const NodeGroup<ParseTreeNode> *group) const
{
    HyperTree<ParseTreeNode>::ConstLayer::const_iterator node;
    int common = 0;
    for_each_(node, group->cnodes())
        if (hasNode(*node, true, true))
            common++;
    return common;
}

ParseTree::ConstLayer& ParseTree::roots()
{
    m_roots.clear();

    if (m_leafes.size() != 0)
    {
        // Walk arround all leafes, for each looking for root of his subtree.
        // If this root not in result yet, than add it
        ConstLayer::iterator node;
        for (node = m_leafes.begin(); node != m_leafes.end(); ++node)
        {
            // looking for root
            ParseTreeNode *root = *node;
            if (root != NULL)
            {
                ParseTreeNode *parent;
                while ((parent = nodeParent(root)) != NULL)
                    root = parent;
            }
            // adding it (if it's not yet added)
            if (root != NULL && !find_(root, m_roots))
                m_roots.push_back(root);
        }
    }
    ConstLayer::iterator node;

    return m_roots;
}

bool ParseTree::onAddNode(void *node)
{
    ParseTreeNode *pnode = static_cast<ParseTreeNode*>(node);

    // buffer to leveled nodes
    Layer *layer = m_nodesByLevel[pnode->level];
    if (layer == NULL)
    {
        layer = new Layer();
        m_nodesByLevel[pnode->level] = layer;
    }
    if (!find_(pnode, *layer))
        layer->push_back(pnode);

    // buffer new node to leafes
    if (pnode->level == 0)
        m_leafes.push_back(pnode);

    // refresh max/min node levels
    if (pnode->level > m_maxLevel || m_maxLevel == -1)
        m_maxLevel = pnode->level;
    if (pnode->level < m_minLevel || m_minLevel == -1)
        m_minLevel = pnode->level;

    return true;
}

void ParseTree::onCopied(void *original)
{
     // Buffer all new nodes
    ParseTree::NodeGroupSet::iterator group;
    ParseTree::ConstLayer::const_iterator node;
    for_each_(group, m_sharedNodes)        
        for_each_ (node, (*group)->cnodes())
            onAddNode(*node);
}

// copy function now in HyperTree
/*
ParseTree* ParseTree::copy(std::map<ParseTreeNode*, ParseTreeNode*> *assocTable)
{
     Алгоритм копирования:
       1. для каждого узла создается копия, причем запоминается, какому узлу
          какая копия соответствует
       2. для скопированного каждого узла расставляются родители-потомки на основе
          запомненных ассоциаций

    ParseTree* result = new ParseTree();
    
    std::map< ParseTreeNode*, ParseTreeNode* >
            *assoc = assocTable != NULL ? assocTable : new std::map< ParseTreeNode*, ParseTreeNode* >;
    
    TreeLayer::iterator node;

    // 1.
    for (node = m_allNodes.begin(); node != m_allNodes.end(); ++node)
        (*assoc)[*node] = result->insertNode( new ParseTreeNode(**node) );

    // 2.
    for (node = m_allNodes.begin(); node != m_allNodes.end(); ++node)
        (*assoc)[*node]->parent = (*assoc)[(*node)->parent];

    if (assocTable == NULL)
        delete assoc;

    return result;
}
*/

TreeCompareResult ParseTree::compare(ParseTree *tree)
{
    TreeCompareResult result;
    ParseTree::NodeGroupSet checkedCommonGroups;
    std::vector<ParseTreeNode*> checkedCommonNodes;

    // Check shared nodes groups of first tree (first == this)
    ParseTree::NodeGroupSet::const_iterator group;
    for_each_(group, m_sharedNodes)
    {
        result.totalInFirst += (*group)->cnodes().size();
        // Look for identical node group in second tree
        if (find_(*group, tree->m_sharedNodes))
        {
            // Found? - remember common group
            checkedCommonGroups.push_back(*group);
            result.totalInSecond += (*group)->cnodes().size();
            result.commonNodes += (*group)->cnodes().size();
        }
        else
        {
            // Not found? - Look if nodes of group have analogs in seond tree
            int commonCount = tree->hasGroup(*group);
            result.uniqueInFirst += (*group)->cnodes().size() - commonCount;
            result.commonNodes += commonCount;
        }
    }

    // Now look for unique groups in second tree
    for_each_(group, tree->m_sharedNodes)
    {
        // If it wasn't checked than look for analogs for nodes of group
        if (!find_(*group, checkedCommonGroups))
        {
            result.totalInSecond += (*group)->cnodes().size();
            int commonCount = this->hasGroup(*group);
            result.uniqueInSecond += (*group)->cnodes().size() - commonCount;
            result.commonNodes += commonCount;
        }
    }
    checkedCommonGroups.clear();

    // Check common nodes in own groups...
    ParseTree::ConstLayer::const_iterator node;
    // ...first tree
    for_each_(node, m_ownNodes->cnodes())
    {
        ParseTreeNode* sameNode = tree->hasNode(*node, true, false);
        if (sameNode != NULL)
        {
            result.commonNodes += 1;
            checkedCommonNodes.push_back(sameNode);
        }
        else
            result.uniqueInFirst += 1;
    }
    // ...second tree
    for_each_(node, tree->m_ownNodes->cnodes())
    {
        if (!find_(*node, checkedCommonNodes))
            result.uniqueInSecond += 1;
    }

    // Don't forget about it
    result.totalInFirst += m_ownNodes->cnodes().size();
    result.totalInSecond += tree->m_ownNodes->cnodes().size();

    // It could happens, it's not an error
    if (result.commonNodes > result.totalInFirst + result.totalInSecond)
        result.commonNodes = result.totalInFirst + result.totalInSecond;

    return result;

    // OLD ALGORITHM
    /*
    Layer checked;
    ParseTreeNode *foundNode;
    Layer::iterator node;

    result.totalInFirst = m_allNodes.size();
    result.totalInSecond = tree->m_allNodes.size();

    for (node = m_allNodes.begin(); node != m_allNodes.end(); ++node) {
        if (options & tcoIgnoreZeroLevel && (*node)->level == 0) {
            result.totalInFirst--;
            continue;
        }
        foundNode = tree->hasNode(*node);
        if (foundNode) {
            result.commonNodes++;
            checked.push_back(foundNode);
        } else
            result.uniqueInFirst++;
    }

    for (node = tree->m_allNodes.begin(); node != tree->m_allNodes.end(); ++node) {
        if (options & tcoIgnoreZeroLevel && (*node)->level == 0) {
            result.totalInSecond--;
            continue;
        }
        if (find(checked.begin(), checked.end(), *node) == checked.end())
            result.uniqueInSecond++;
    }

    */
}

#else // FL_ENABLE_PARSE_TREE_OPTIMIZATION

ParseTree::ParseTree(ParseTreeSet *treeSet)
{
    m_minLevel = m_maxLevel = -1;
    m_treeSet = treeSet;
}

ParseTree::~ParseTree()
{
    clear();
}

ParseTree::ConstLayer& ParseTree::roots()
{
    m_roots.clear();

    if (m_leafes.size() != 0)
    {
        // For each leaf find root of its subtree.
        // If this root not in result yet, than add it
        ConstLayer::iterator node;
        for_each_(node,  m_leafes)
        {
            // find root
            ParseTreeNode *root = *node;
            if (root != NULL)
            {
                ParseTreeNode *parent;
                while ((parent = nodeParent(root)) != NULL)
                    root = parent;
            }

            // adding it (if it's not yet added)
            if (root != NULL && !find_(root, m_roots))
                m_roots.push_back(root);
        }
    }

    return m_roots;
}

ParseTree::ConstLayer& ParseTree::roots(int tsBegin, int tsEnd)
{
    static ParseTree::Layer rootsOnSegment;
    rootsOnSegment.clear();

    // For each leaf on segment: find root of its subtree.
    // If this root not in result yet, than add it
    if (m_leafes.size() != 0)
    {
        ConstLayer::iterator node;
        for_each_(node, m_leafes)
        {
            // find root
            ParseTreeNode *root = *node;
            if (root != NULL)
            {
                if ( ! isSegmentsOverlay(tsBegin, tsEnd, root->tsBegin, root->tsEnd) )
                    continue;
                ParseTreeNode *parent;
                while ((parent = nodeParent(root)) != NULL)
                    root = parent;
            }

            // adding it (if it's not yet added)
            if (root != NULL && !find_(root, m_roots))
                m_roots.push_back(root);
        }
    }

    return rootsOnSegment;
}

void ParseTree::clear()
{
    Layer::iterator node;
    for_each_(node, m_allNodes)
        delete *node;
    m_allNodes.clear();
    m_roots.clear();
    m_leafes.clear();
    m_nodesByLevel.clear();
    m_minLevel = m_maxLevel = -1;
}

TreeCompareResult ParseTree::compare(ParseTree *tree)
{
    TreeCompareResult result;
    bool ignoreZeroLevel = true;
    Layer checked;
    ParseTreeNode *foundNode;
    Layer::iterator node;

    result.totalInFirst = m_allNodes.size();
    result.totalInSecond = tree->m_allNodes.size();

    for (node = m_allNodes.begin(); node != m_allNodes.end(); ++node)
    {
        if (ignoreZeroLevel && (*node)->level == 0)
        {
            result.totalInFirst--;
            continue;
        }
        foundNode = tree->hasNode(*node);
        if (foundNode)
        {
            result.commonNodes++;
            checked.push_back(foundNode);
        } else
            result.uniqueInFirst++;
    }

    for (node = tree->m_allNodes.begin(); node != tree->m_allNodes.end(); ++node)
    {
        if (ignoreZeroLevel && (*node)->level == 0)
        {
            result.totalInSecond--;
            continue;
        }
        if (find(checked.begin(), checked.end(), *node) == checked.end())
            result.uniqueInSecond++;
    }

    return result;
}

void ParseTree::copyTo(ParseTree *target, std::map<ParseTreeNode*, ParseTreeNode*> *assocTable)
{
    /*
       Algorithm:
       1. For each node make a copy and remember mapping between nodes
       2. Based on mapping make child-parent associations
    */

    // Associations table
    std::map< ParseTreeNode*, ParseTreeNode* >
            *assoc = assocTable != NULL ? assocTable : new std::map< ParseTreeNode*, ParseTreeNode* >;

    Layer::iterator node;

    // 1.
    for (node = m_allNodes.begin(); node != m_allNodes.end(); ++node)
        (*assoc)[*node] = target->addNode( new ParseTreeNode(**node) );

    // 2.
    for (node = m_allNodes.begin(); node != m_allNodes.end(); ++node)
        (*assoc)[*node]->parent = (*assoc)[(*node)->parent];

    if (assocTable == NULL)
        delete assoc;
}

ParseTreeNode* ParseTree::addNode(ParseTreeNode *node)
{
    // Buffer to leveled nodes
    Layer *layer = m_nodesByLevel[node->level];
    if (layer == NULL)
    {
        layer = new Layer();
        m_nodesByLevel[node->level] = layer;
    }
    if (!find_(node, *layer))
        layer->push_back(node);

    // Buffer new node to leafes
    if (node->level == 0)
        m_leafes.push_back(node);

    // Refresh max/min node levels
    if (node->level > m_maxLevel || m_maxLevel == -1)
        m_maxLevel = node->level;
    if (node->level < m_minLevel || m_minLevel == -1)
        m_minLevel = node->level;

    m_allNodes.push_back(node);
    return node;
}

void ParseTree::excludeNode(ParseTreeNode *node)
{
    if (node != NULL)
    {
        ParseTree::Layer::iterator it;
        it = std::find( m_allNodes.begin(), m_allNodes.end(), node );
        if (it != m_allNodes.end())
        {
            // remove from cache
            m_allNodes.erase(it);

            it = std::find( m_leafes.begin(), m_leafes.end(), node );
            if (it != m_leafes.end())
                m_leafes.erase(it);

            it = std::find( m_roots.begin(), m_roots.end(), node );
            if (it != m_roots.end())
                m_roots.erase(it);

            // recalc levels
            if (m_maxLevel == node->level || m_minLevel == node->level)
            {
                int max = m_minLevel, min = m_maxLevel;
                for_each_(it, m_allNodes)
                {
                    if ((*it)->level > max)
                        max = (*it)->level;
                    if ((*it)->level < min)
                        min = (*it)->level;
                }
                m_minLevel = min;
                m_maxLevel = max;
            }
        }
    }
}

ParseTreeNode* ParseTree::hasNode(const ParseTreeNode *node) const
{
    ConstLayer::const_iterator it;
    for_each_(it, m_allNodes)
    {
        if (node->level == (*it)->level &&
            node->tsBegin == (*it)->tsBegin && node->tsEnd == (*it)->tsEnd &&  node->name == (*it)->name )
            return *it;
    }
    return NULL;
}

void ParseTree::clearSegment(int tsBegin, int tsEnd)
{
    ParseTree::Layer::iterator node;
    for_each_(node, m_allNodes)
    {
        if ( isSegmentsOverlay(tsBegin, tsEnd, (*node)->tsBegin, (*node)->tsEnd) )
        {
            excludeNode(*node);
            delete *node;
        }
    }
}

#endif
