#ifndef NODE_H
#define NODE_H

namespace FL { namespace Trees {

class Layer;

class Node
{
private:
    /*! Node coping is disabled since it is too difficult to properly copy
      * internal cache info.
      */
    Node(const Node &node) {}
public:
    //! Default constructor
    Node(Node *parent = (Node*)0, int id = -1, int begin = -1, int end = -1, int level = -1);

    //! Destructor
    ~Node();

    //! Get parent of node
    Node* parent() const { return m_parent; }
    void setParent(Node* value);

    //! Get Id of name of node
    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    //! Get index of node
    int index() const { return m_index; }
    void setIndex(int index) { m_index = index; }

    //! Get begin of TimeSeries segment associated with node
    int begin() const { return m_begin; }
    void setBegin(int value) { m_begin = value; }

    //! Get end of TimeSeries segment associated with node
    int end() const { return m_end; }
    void setEnd(int value) { m_end = value; }

    //! Get level of node
    int level() const { return m_level; }
    void setLevel(int value) { m_level = value; }

    //! Get children of node
    Layer& children() { return *m_children; }

    //! Get cached relative node (warning)
    Node *relativeNode() const { return m_relativeNode; }

    friend class Tree;
private:
    //! Id of name of node
    int m_id;

    //! Index of node
    int m_index;

    //! Begin of TimeSeries segment associated with node
    int m_begin;

    //! End of TimeSeries segment associated with node
    int m_end;

    //! Level of node
    int m_level;

    //! Physical parent of node
    Node *m_parent;

    //! Children of node
    Layer *m_children;

    /*! Node that is on the same position in the tree,
      * that was duplicated to create current tree.
      * Used by internal algorithms of Tree.
      */
    Node *m_relativeNode;

    /*! Virtual parent node. Used by internal algorithms of Tree
      */
    Node *m_internalParent;
};

}}

#endif // NODE_H
