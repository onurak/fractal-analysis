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
}

Tree* Tree::copy() const
{
    Tree *tree = new Tree();

    Layer::ConstIterator node, parent;

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

        // Add to leveled cache
        m_nodesByLevel[node->level()].push_back(node);
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

        // Remove from m_allNodes and from caches
        if (search(m_allNodes, node, i))
        {
            m_allNodes.erase(i);

            if (search(m_nodesByLevel[node->level()], node, i))
                m_nodesByLevel[node->level()].erase(i);
        }

        // It could be among roots
        if (search(m_virtualRoot->children(), node, i))
            m_virtualRoot->children().erase(i);
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
    return m_nodesByLevel.size();
}

void Tree::markupWithRoots(int begin, int end)
{
    Layer::Iterator node = m_allNodes.begin(), i;
    while ( node != m_allNodes.end() )
    {
        // Skip node if its out of range
        if ((*node)->begin() < begin || (end != -1 && (*node)->begin() > end))
        {
            ++node;
            continue;
        }

        // if node isn't root - erase it
        if ( !exists(m_virtualRoot->children(), *node) )
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
    m_allNodes.sort();
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
            if ((*node)->end() >= borderPos)
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
        if ((*node)->status() != nsFloating)
        {
            if (node == level0.rbegin())
                return (*node)->end();
            return (*(--node))->begin();
        }
    }
    return 0;
}
