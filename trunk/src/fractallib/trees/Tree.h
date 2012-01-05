#ifndef TREE_H
#define TREE_H

#include "../Common.h"
#include "Node.h"
#include "Layer.h"
#include "../TimeSeries.h"

namespace FL { namespace Trees {

/*! \struct TreeCompareResult
  * \brief Info on tree comparison procedure
  */
struct TreeCompareResult
{
    int totalNodesInFirst;
    int totalNodesInSecond;
    int totalCommonNodes;
    int uniqueNodesInFirst;
    int uniqueNodesInSecond;

    bool isFirstSubtreeOfSecond() const
    {
        return uniqueNodesInFirst == 0;
    }

    bool isSecondSubtreeOfFirst() const
    {
        return uniqueNodesInSecond == 0;
    }
};


/*! \class Tree
  * \brief Represents Parse Tree - result of time series analysis.
  */
class Tree
{
private:
    //! Use copy() mothod to create deep tree copy
    Tree(const Tree &tree) : m_timeSeries(tree.m_timeSeries) {}
public:
    //! Default constructor
    Tree(const FL::TimeSeries &timeSeries);

    //! Destructor
    virtual ~Tree();

    //! Get all root nodes of tree
    const Layer& roots() const;

    //! Get all leaf nodes (in fact it is all nodes at zero level)
    const Layer& leafs() const;

    //! Get all nodes at specified level
    const Layer& nodesByLevel(int level) const;

    //! Get level count in tree.
    //! There are always exists [0, ..., levelCount()-1] levels.
    int levelCount() const;

    //! Add node to tree. It is the only valid way to do this
    void add(Node *node);

    //! Remove node from tree. It is the only valid way to do this
    void remove(Node *node);

    //! Update level of node
    void updateLevel(Node *node, int newLevel);

    //! Make copy of tree
    Tree* copy() const;

    //! Delete all nodes, clear information about them
    void clear();

    //! Remove all nodes except roots, set roots level to 0
    void markupWithRoots();

    //! Compare this (first) tree to another (second) tree
    TreeCompareResult compare(const Tree &tree) const;

    //! Look for node with identical id, level, begin, end
    Node* findNode(Node *patternNode) const;

    //! Get begining of last unmarked segment
    int getLastUnmarkedSegment() const;

    //! Get time of first floating node at zero level
    int floatingBegin() const;
protected:
    //! Insert one node to children's sequence of another at right place
    //! according to position in time series
    void insertChild(Node *dst, Node *src);
private:
    //! All nodes of tree
    Layer m_allNodes;

    //! Virtual root for fast roots detection
    Node* m_virtualRoot;

    //! Cached nodes by layers
    mutable std::map<int, Layer> m_nodesByLevel;

    //! Actual levels count
    mutable int m_levelsCount;

    //! Assigned time series
    const FL::TimeSeries &m_timeSeries;
};

}} // namespaces

#endif // TREE_H
