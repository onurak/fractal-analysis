#ifndef NODE_H
#define NODE_H

#include <vector>
#include "../patterns/CheckInfo.h"

namespace FL { namespace Patterns {
    class Context;
    class Pattern;
}}

namespace FL {
    //! Type of tree node
    enum NodeStatus
    {
        nsFixed,    //!< Node fixed in tree structure forever

        nsFloating, //!< Node have all of its children and guards are still executable,
                    //!< but some parameters of node could be changed ("almost fixed")

        nsPossible  //!< Not all children of node are exists, this node could be
                    //!< deleted from tree structure in future
    };
}

namespace FL { namespace Trees {

class Layer;

/*! \class Node
  * \brief Single node of tree
  */
class Node
{    
public:
    //! Default constructor
    Node(Node *parent = NULL, int id = -1, int begin = -1, int end = -1,
         int level = -1, NodeStatus ns = nsFixed);

    //! Copy constructor
    /*! Warning! Invalid usage of copy constructor may lead to tree cache
        destruction.
      */
    Node(const Node &node);

    //! Destructor
    ~Node();

    //! Get parent of node
    inline Node* parent() const { return m_parent; }
    void setParent(Node* value);

    //! Get Id of name of node
    inline int id() const { return m_id; }
    inline void setId(int id) { m_id = id; }

    //! Get index of node
    inline int index() const { return m_index; }
    inline void setIndex(int index) { m_index = index; }

    //! Get begin of TimeSeries segment associated with node
    inline int begin() const { return m_begin; }
    inline void setBegin(int value) { m_begin = value; }

    //! Get end of TimeSeries segment associated with node
    inline int end() const { return m_end; }
    inline void setEnd(int value) { m_end = value; }

    //! Get level of node
    inline int level() const { return m_level; }
    inline void setLevel(int value) { m_level = value; }

    //! Get status of node
    inline FL::NodeStatus status() const { return m_status; }
    inline void setStatus(FL::NodeStatus value) { m_status = value; }

    //! Get children of node
    inline Layer& children() { return *m_children; }

    //! Get cached relative node (warning)
    inline Node *relativeNode() const { return m_relativeNode; }

    //! Get original sequence for node that have status nsPossible
    inline Patterns::CISequence *origSequence() { return m_origSequence; }

    //! Set original sequence for node that have status nsPossible
    inline void setOrigSequence(Patterns::CISequence *sequence) { m_origSequence = sequence; }

    //! Get original pattern for node that have status nsPossible
    inline Patterns::Pattern *origPattern() { return m_origPattern; }

    //! Set original pattern for node that have status nsPossible
    inline void setOrigPattern(FL::Patterns::Pattern *pattern) { m_origPattern = pattern; }


    friend class Tree;
    friend class FL::Patterns::Context;
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

    //! Status of node
    FL::NodeStatus m_status;

    /*! Node that is on the same position in the tree,
      * that was duplicated to create current tree.
      * Used by internal algorithms of Tree.
      */
    Node *m_relativeNode;

    /*! Virtual parent node. Used by internal algorithms of Tree
      */
    Node *m_internalParent;

    /*! If node have status nsPossible, then this would
      * store original sequence that is possible for children of
      * this node
      */
    Patterns::CISequence *m_origSequence;

    Patterns::Pattern *m_origPattern;
};

}}

#endif // NODE_H
