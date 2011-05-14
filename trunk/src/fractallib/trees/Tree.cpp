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
        (*node)->setParent(NULL);
    m_virtualRoot->children().clear();

    // Delete all nodes
    forall(node, m_allNodes)
        delete *node;
    m_allNodes.clear();

    // Clear cache
    m_nodesByLevel.clear();
}

Tree* Tree::copy()
{
    Tree *tree = new Tree();

    Layer::ConstIterator node, parent;

    // Create nodes
    forall(node, m_allNodes)
    {
        Node *newNode = new Node(
                NULL, (*node)->id(), (*node)->begin(), (*node)->end(), (*node)->level());
        tree->m_allNodes.push_back(newNode);
        m_nodesByLevel[newNode->level()].push_back(newNode);
        newNode->m_relativeNode = *node;
        (*node)->m_relativeNode = newNode;
    }

    // Create relations between nodes
    forall(node, tree->m_allNodes)
    {
        Node *originalNode = (*node)->m_relativeNode;
        forall(parent, originalNode->children())
            (*node)->setParent((*parent)->m_relativeNode);
    }

    // Remember virtual node's children
    forall(node, m_virtualRoot->children())
    {
        Node *originalNode = (*node)->m_relativeNode;
        tree->m_virtualRoot->children().push_back(originalNode);
        originalNode->m_parent = tree->m_virtualRoot;
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

void Tree::markupWithRoots()
{
    Layer::Iterator node = m_allNodes.begin(), i;
    for (; node != m_allNodes.end(); )
    {
        if (!exists(m_virtualRoot->children(), *node))
        {
            if (search(m_nodesByLevel[(*node)->level()], *node, i))
                m_nodesByLevel[(*node)->level()].erase(i);
            delete *node;
            node = m_allNodes.erase(node);
        }
        else
        {
            (*node)->setLevel(0);
            ++node;
        }
    }
}

TreeCompareResult Tree::compare(const Tree &tree) const
{
    TreeCompareResult tcr;
    return tcr;
}
