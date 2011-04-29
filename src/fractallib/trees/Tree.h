#ifndef TREE_H
#define TREE_H

#include "../Common.h"
#include "Node.h"
#include "Layer.h"

namespace FL { namespace Trees {

/*! \struct TreeCompareResult
  * \brief Info on tree comparison procedure
  */
class TreeCompareResult
{
public:
    TreeCompareResult()
    {
        totalInFirst = totalInSecond = commonNodes = uniqueInFirst = uniqueInSecond = 0;
    }

    int totalInFirst;   //!< Total nodes in first tree
    int totalInSecond;  //!< Total nodes in second tree
    int commonNodes;    //!< Identical nodes count
    int uniqueInFirst;  //!< Count of unique nodes in first tree
    int uniqueInSecond; //!< Count of unique nodes in second tree

    //! Is the first tree is subtree of second
    bool isFirstSubtreeOfSecond() const
    {
        return (commonNodes != 0 || totalInFirst == 0) && uniqueInFirst == 0;
    }

    //! Is the second tree is subtree of first
    bool isSecondSubtreeOfFirst() const
    {
        return (commonNodes != 0 || totalInSecond == 0) && uniqueInSecond == 0;
    }

    //! Percent of equivalent nodes in trees (0-100%)
    int commonPercent() const
    {
        return int( (2*commonNodes) / double(totalInFirst + totalInSecond) * 100 );
    }
};


/*! \class Tree
  * \brief Represents Parse Tree - result of time series analysis.
  */
class Tree
{
private:
    //! Use copy() mothod to create deep tree copy
    Tree(const Tree &tree) { tree.roots(); }
public:
    //! Default constructor
    Tree();

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

    //! Change node status (internal cache would be updated in this case)
    void changeNodeStatus(Node *node, NodeStatus newStatus);

    //! Remove node from tree. It is the only valid way to do this
    void remove(Node *node);

    //! Make copy of tree
    Tree* copy() const;

    //! Delete all nodes, clear information about them
    void clear();

    //! Check if tree have identical node (same name, level and position)
    Node* haveNode(const Node *likeThis, bool ignoreZeroLevel = false) const;

    //! Remove all nodes except roots, set roots level to 0
    void markupWithRoots(int begin = 0, int end = -1);

    //! Compare this (first) tree to another (second) tree
    TreeCompareResult compare(const Tree &tree) const;

    //! Get all nodes with status nsPossible
    inline const Layer& possibleNodes() const
    {
        return m_possibleNodes;
    }

    //! Average memory usage
    int bytesUsed() const;

    //! Total nodes count
    int nodesCount() const
    {
        return (int)m_allNodes.size();
    }

    //! Get segment where tree is applied to time series
    void getSegment(int &min, int &max) const;

    //! Remove error from tree structure
    /*! It will sort all levels by time */
    void validateStructure();

    //! Make last nodes of tree to have status nsDynamic
    /*! It is usefull for dynamic analysis.
      * \param borderPos Only those nodes would be detached, whos end
      * position less or equal to borderPos.
      */
    void makeDynamicEnd(int borderPos);

    //! Get start position of ending nodes that have "floating" parameters (have status nsDynamic)
    int dynamicEndPos() const;

    //! Get nodes followed by specified node at the same level
    Layer getFollowingRoots(Node *node);

    //! Fixup tree structure (make all floating nodes to be fixed)
    void fixup();

    //! Destroy last nodes at last level.
    Tree* detachLevelLastNodes(int count) const;
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

    //! Cached possible nodes
    mutable Layer m_possibleNodes;
};

}} // namespaces

#endif // TREE_H
