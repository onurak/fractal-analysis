#ifndef TREE_H
#define TREE_H

#include "../Common.h"
#include "Node.h"
#include "Layer.h"

namespace FL { namespace Trees {

/*! \struct TreeCompareResult
  * \brief Info on tree comparison procedure
  */
struct TreeCompareResult
{
    int totalNodesInFirst;
    int totalNodesInSecond;
    int totalCommonNodes;

    bool isFirstSubtreeOfSecond() const
    {
        return false;
    }

    bool isSecondSubtreeOfFirst() const
    {
        return false;
    }
};


/*! \class Tree
  * \brief Represents Parse Tree - result of time series analysis.
  */
class Tree
{
private:
    //! Use copy() mothod to create deep tree copy
    Tree(const Tree &tree) {}
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

    //! Remove node from tree. It is the only valid way to do this
    void remove(Node *node);

    //! Make copy of tree
    Tree* copy();

    //! Delete all nodes, clear information about them
    void clear();

    //! Remove all nodes except roots, set roots level to 0
    void markupWithRoots();

    //! Compare this (first) tree to another (second) tree
    TreeCompareResult compare(const Tree &tree) const;
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
};

}} // namespaces

#endif // TREE_H
