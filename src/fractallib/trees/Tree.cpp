#include "Tree.h"

using namespace FL::Trees;

Tree::Tree(const FL::TimeSeries &timeSeries)
    : m_timeSeries(timeSeries)
{
    m_levelsCount = 0;
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
    m_virtualRoot->children().clear();

    // Delete all nodes
    Layer::Iterator node;
    forall(node, m_allNodes)
        delete *node;
    m_allNodes.clear();

    // Clear cache
    m_nodesByLevel.clear();
}

Tree* Tree::copy() const
{
    Tree *tree = new Tree(m_timeSeries);

    Layer::ConstIterator node, child;

    // Create nodes
    forall(node, m_allNodes)
    {
        Node *newNode = new Node(**node);
        tree->m_allNodes.push_back(newNode);
        tree->m_nodesByLevel[newNode->level()].push_back(newNode);
        newNode->m_relativeNode = *node;
        (*node)->m_relativeNode = newNode;
    }

    // Create relations between nodes
    forall(node, tree->m_allNodes)
    {
        Node *originalNode = (*node)->m_relativeNode;
        forall(child, originalNode->children())
        {
            (*child)->m_relativeNode->setParent(*node);
        }
    }

    // Remember virtual node's children
    forall(node, m_virtualRoot->children())
    {
        Node *originalNode = (*node)->m_relativeNode;
        tree->m_virtualRoot->children().push_back(originalNode);
        originalNode->m_internalParent = tree->m_virtualRoot;
    }

    tree->m_levelsCount = this->m_levelsCount;

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

        // Add to cache by level in proper place
        Layer &layer = m_nodesByLevel[node->level()];
        if (layer.size() > 0)
        {
            Layer::Iterator i;
            forall(i, layer)
            {
                if ((*i)->end() <= node->begin())
                {
                    layer.insert(i, node);
                    break;
                }
            }
        }
        else
            layer.push_back(node);

        if (m_levelsCount <= node->level())
            m_levelsCount = node->level()+1;
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
            removeNodeFromLeveledCache(node);
        }

        // It could be among roots
        if (search(m_virtualRoot->children(), node, i))
            m_virtualRoot->children().erase(i);
    }
}

void Tree::updateNodeLevel(Node *node, int newLevel)
{
    if (node && node->level() != newLevel)
    {
        removeNodeFromLeveledCache(node);

        node->setLevel(newLevel);
        m_nodesByLevel[newLevel].push_back(node);
    }
}

void Tree::removeNodeFromLeveledCache(Node *node)
{
    std::map<int, Layer>::iterator itnbLayer = m_nodesByLevel.find(node->level());
    if (itnbLayer != m_nodesByLevel.end())
    {
        Layer::Iterator i;
        Layer &layer = itnbLayer->second;
        if (search(layer, node, i))
        {
            layer.erase(i);
            if (layer.size() == 0)
                m_nodesByLevel.erase(itnbLayer);
        }
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

void Tree::markupWithRoots()
{
    Layer::Iterator node = m_allNodes.begin(), i;
    for (; node != m_allNodes.end(); )
    {
        int lvl = (*node)->level();

        if (!exists(m_virtualRoot->children(), *node))
        {
            if (search(m_nodesByLevel[lvl], *node, i))
                m_nodesByLevel[lvl].erase(i);
            delete *node;
            node = m_allNodes.erase(node);
        }
        else
        {
            if (lvl != 0)
            {
                if (search(m_nodesByLevel[lvl], *node, i))
                    m_nodesByLevel[lvl].erase(i);
                (*node)->setLevel(0);
                m_nodesByLevel[0].push_back(*node);
            }
            ++node;
        }
    }
}

Node* Tree::findNode(Node *patternNode) const
{
    if (patternNode == NULL)
        return NULL;

    if (patternNode->level() >= levelCount())
        return NULL;

    const Layer &level = m_nodesByLevel[patternNode->level()];

    Layer::ConstIterator itNode;
    forall(itNode, level)
    {
        Node *node = *itNode;
        if (node->begin() == patternNode->begin())
        {
            if (node->end() == patternNode->end() &&
                node->id() == patternNode->id())
            {
                return node;
            }
        }
    }

    return NULL;
}

int Tree::getLastUnmarkedSegment() const
{
    if (levelCount() == 0)
        return 0;
    //return std::min(leafs().back()->end(), m_timeSeries.size());

    int r = 0;
    const Layer &leafs = this->leafs();
    Layer::ConstIterator node;

    forall(node, leafs)
    {
        if ((*node)->end() > r)
            r = (*node)->end();
    }

    return r;
}

int Tree::floatingBegin() const
{
    const Layer &leafs = this->leafs();
    int r = m_timeSeries.size();

    if (levelCount() == 0)
        return r;

    Layer::ConstIterator node;

    forall(node, leafs)
    {
        if ((*node)->status() == nsFloating)
        {
            if ((*node)->begin() < r)
                r = (*node)->begin();
        }
        else
            return r;
    }

    return r;
}

TreeCompareResult Tree::compare(const Tree &tree) const
{
    TreeCompareResult tcr;

    tcr.totalNodesInFirst  = (int) this->m_allNodes.size();
    tcr.totalNodesInSecond = (int) tree.m_allNodes.size();

    // Zero level is always equals in trees from same analysis run.
    // We are not interested to compare trees from different analysis.
    if (this->nodesByLevel(0).size() != tree.nodesByLevel(0).size())
    {
        tcr.uniqueNodesInFirst  = tcr.totalNodesInFirst;
        tcr.uniqueNodesInSecond = tcr.totalNodesInSecond;
        tcr.totalCommonNodes = 0;
        return tcr;
    }

    tcr.uniqueNodesInFirst  = 0;
    tcr.uniqueNodesInSecond = 0;
    tcr.totalCommonNodes = (int) this->nodesByLevel(0).size();


    ///////////////////////////////////////////////
    // Perform deep analysis of non zero levels
    ///////////////////////////////////////////////

    // Nodes from usnique levels
    if (this->levelCount() > tree.levelCount())
    {
        int treeLevelCount = this->levelCount();
        for (int level = tree.levelCount(); level < treeLevelCount; ++level)
            tcr.uniqueNodesInFirst += this->nodesByLevel(level).size();
    } else if (this->levelCount() < tree.levelCount())
    {
        int treeLevelCount = tree.levelCount();
        for (int level = this->levelCount(); level < treeLevelCount; ++level)
            tcr.uniqueNodesInSecond += tree.nodesByLevel(level).size();
    }

    // Rest of nodes
    for (int level = 1; level < this->levelCount() && level < tree.levelCount(); ++level)
    {
        const Layer& frstLevel = this->nodesByLevel(level);
        const Layer& scndLevel = tree.nodesByLevel(level);
        Layer::ConstIterator node;

        std::map<Node*, bool> checkedNodes;

        // Look for each node in the second tree,
        // remember nodes that already been checked
        forall(node, frstLevel)
        {
            Node *secondNode = tree.findNode(*node);
            if (secondNode != NULL)
            {
                tcr.totalCommonNodes++;
                checkedNodes[secondNode] = true;
            }
            else
                tcr.uniqueNodesInFirst++;
        }

        // Look for nodes in second tree that wasn't been checked yet
        forall(node, scndLevel)
        {
            if (checkedNodes.find(*node) == checkedNodes.end())
               tcr.uniqueNodesInSecond++;
        }
    }

    return tcr;
}

void Tree::removeEmptyTopLevels() const
{
    while (m_nodesByLevel.size() != 0 &&
           m_nodesByLevel[m_nodesByLevel.size()-1].size() == 0)
        m_nodesByLevel.erase(m_nodesByLevel.size()-1);
}

int Tree::levelCount() const
{
    removeEmptyTopLevels();
    m_levelsCount = m_nodesByLevel.size();
    return m_levelsCount;
}

