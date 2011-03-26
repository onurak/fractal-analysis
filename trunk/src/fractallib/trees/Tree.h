#ifndef TREE_H
#define TREE_H

#include "../Common.h"
#include "Node.h"
#include "Layer.h"

namespace FL { namespace Trees {

class Tree
{
public:
    Tree();
    virtual ~Tree();

    const Layer& roots() const;
    const Layer& leafs() const;
    const Layer& nodesByLevel(int level) const;
    int levelCount() const;


    void add(Node *node);
    void remove(Node *node);

    Tree* copy();
    void clear();
private:
    //! All nodes of tree
    Layer m_allNodes;

    //! Virtual root for fast roots detection
    Node* m_virtualRoot;

    //! Cached leafs
    Layer m_leafs;

    //! Cached nodes by layers
    mutable std::map<int, Layer> m_nodesByLevel;
};

}} // namespaces

#endif // TREE_H
