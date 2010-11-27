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
#ifndef PARSETREE_H
#define PARSETREE_H

#include <map>
#include <string>
#include <list>
#include <vector>
#include "ProjectSettings.h"
#include "Exceptions.h"

#ifdef FL_ENABLE_PARSE_TREE_OPTIMIZATION
    #include "HyperTree.h"
#endif


namespace FL {

/*! \class ParseTreeNode
  * \brief Node of parse tree
  */
struct ParseTreeNode
{

    //! Default constructor
    ParseTreeNode(const std::string &name = "", ParseTreeNode *parent = NULL, int level = -1,
                  int tsBegin = -1, int tsEnd = -1)
        : name(name), no(-1), parent(parent), tsBegin(tsBegin), tsEnd(tsEnd), level(level)
    {}

    /*
    //! Default constructor
    ParseTreeNode(const std::string &name = "", int level = -1, int tsBegin = -1, int tsEnd = -1)
        : name(name), no(-1), tsBegin(tsBegin), tsEnd(tsEnd), level(level)
    {}
    */

    //! Copy constructor
    /*! Coping all data except parent, wich set to NULL
      */
    ParseTreeNode(const ParseTreeNode &node)
    {
        name = node.name;
        no = node.no;
        tsBegin = node.tsBegin;
        tsEnd = node.tsEnd;
        level = node.level;
        parent = node.parent;
        children = node.children;
    }

    //! Convert to string
    std::string asString() const
    {
        if (no != -1)
        {
            char buf[50];
            i2a(no, buf);
            return name + "_" + std::string(buf);
        }
        return name;
    }

    std::string name; //!< Name of symbol (terminal or nonterminal) wich related with this node
    int no; //!< Symbol number
    ParseTreeNode *parent; //!< Parent of the node
    int tsBegin; //!< First index of segment of time series
    int tsEnd;   //!< Last index of segment of time series
    int level; //!< Level (depth) of node
    std::list<ParseTreeNode*> children; //!< Children of node
};

/*! \struct TreeCompareResult
  * \brief Result of compare of two trees
  */
struct TreeCompareResult
{
    TreeCompareResult()
    {
        commonNodes = uniqueInFirst = uniqueInSecond = totalInFirst = totalInSecond = 0;
    }

    int commonNodes; //!< number of same nodes
    int totalInFirst; //!< total nodes in first tree
    int totalInSecond; //!< total nodes in second tree
    int uniqueInFirst; //!< number of unique nodes in first tree
    int uniqueInSecond; //!< number of unique nodes in second tree

    //! Returns true if second tree contained in first (second is subtree)
    bool firstContainsSecond() const
    {
        return (commonNodes != 0 || totalInSecond == 0) && uniqueInSecond == 0;
    }

    //! Returns true if first tree contained in second (first is subtree)
    bool secondContainsFirst() const
    {
        return (commonNodes != 0 || totalInFirst == 0) && uniqueInFirst == 0;
    }

    //! Percent of equivalent nodes in trees (0-100%)
    int commonPercent() const
    {
        return int( (2*commonNodes) / double(totalInFirst + totalInSecond) * 100 );
    }
};

#ifdef FL_ENABLE_PARSE_TREE_OPTIMIZATION

class ParseTree;

/*! \class ParseTreeSet
  * \brief Set of parse tree
  */
class ParseTreeSet: public HyperTreeSet<ParseTreeNode>
{
public:
    typedef std::vector<ParseTree*>::iterator Iterator;
};

/*! \class ParseTree
  * \brief Parse tree of time series
  */
class ParseTree: public HyperTree<ParseTreeNode>
{
public:
    //! Default constructor
    ParseTree(ParseTreeSet *treeSet = NULL);

    //! Destructor
    ~ParseTree();

    //! Get coma of tree (all it's leafes ordered from left to right)
    ConstLayer& leafes() {
        return m_leafes;
    }

    //! Get roots of the tree (list of nodes nearest to possible root, left to right order)
    ConstLayer& roots();

    //! Returns layer of all nodes with selected level, or NULL
    ConstLayer* getLevel(int level) {
        return m_nodesByLevel[level];
    }

    //! Compare two trees
    TreeCompareResult compare(ParseTree *tree);

    //! Check if there is node in tree equivalent to parameter
    /*! \param node Node to search
      * \param amongOwn Search this node among own nodes
      * \param amongShared Search this node among common shared nodes
      */
    ParseTreeNode* hasNode(const ParseTreeNode *node, bool amongOwn = true, bool amongShared = false) const;

    //! Return minimal level number in tree
    int minLevel()
    {
        return m_minLevel;
    }

    //! Return maximal level number in tree
    int maxLevel()
    {
        return m_maxLevel;
    }

    friend class ParseTreeModification;
protected:
    /*! \copydoc HyperTree::onAddNode(void *node)
      *
      * Also fill some buffers and make some statistics
      */
    virtual bool onAddNode(void *node);

    /*! \copydoc HyperTree::onCopied(void *original)
      *
      * Also fill some buffers and make some statistics
      */
    virtual void onCopied(void *original);
private:
    //! Check if all nodes of group has analogs in tree (needed by compare() function)
    /*! \return Count of group's nodes that have analogs
      */
    int hasGroup(const NodeGroup<ParseTreeNode> *group) const;

    //! Buffer for fast access to leafes of the tree
    Layer m_leafes;

    //! Buffer for fast access to roots of the tree
    Layer m_roots;

    //! Nodes sorted by level (fast access buffer)
    std::map<int, Layer*> m_nodesByLevel;

    //! Maximal level of node in tree
    int m_maxLevel;

    //! Minimal level of node in tree
    int m_minLevel;
};

#else // FL_ENABLE_PARSE_TREE_OPTIMIZATION

class ParseTreeSet;

/*! \class ParseTree
  * \brief Parse tree of time series (without optimizations)
  *
  * This tree do not based on any optimization algorithm so coping of tree is slow
  * and memory usage is high. But it's works. This tree will be used if macro
  * FL_ENABLE_PARSE_TREE_OPTIMIZATION is not defined.
  */
class ParseTree
{
public:
    /*! \typedef Layer
      * \brief Any sequence of nodes
      *
      * It was made as list (not vector) because insertion could be frequent
      */
    typedef std::list<ParseTreeNode*> Layer;

    /*! \typedef ConstLayer
      * \brief Any constant sequence of constant nodes
      */
    typedef const Layer ConstLayer;
public:
    //! Default constructor
    ParseTree(ParseTreeSet *treeSet = NULL);

    //! Destructor
    virtual ~ParseTree();

    //! Clear tree (all nodes will be deleted)
    void clear();

    //! Get coma of tree (all it's leafes ordered from left to right)
    ConstLayer& leafes()
    {
        return m_leafes;
    }

    //! Add node to tree
    /*! After you had set parent, children and other parameters of node you must
      * call this function because ParseTree using internal caching of its nodes.
      * Otherwise node will not be included in roots(), leafes(), minLevel(),
      * maxLevel() and other function's return values.
      * \return Node passed as parameter
      */
    ParseTreeNode* addNode(ParseTreeNode *node);

    //! Remove node form tree
    /*! If you suggest that node could be part of ParseTree you must call
      * excludeNode before deleting node. It will excelude this node from
      * internal ParseTree's cache. Otherwise memory errors are possible
      */
    void excludeNode(ParseTreeNode *node);

    //! Get parent of the node
    /*! Nodes can have various structure and there is no guarantee that
      * node->parent would exists. Use this method to make your code workable.
      */
    ParseTreeNode* nodeParent(ParseTreeNode *node)
    {
        return node->parent;
    }

    //! Set parent of the node
    /*! Nodes can have various structure and there is no guarantee that
      * node->parent would exists. Use this method to make your code workable.
      * \sa nodeParent
      */
    void setNodeParent(ParseTreeNode *node, ParseTreeNode *parent)
    {
        node->parent = parent;
    }

    //! Get roots of the tree (list of nodes nearest to possible root, left to right order)
    ConstLayer& roots();

    //! Get roots of tree on given segment (list of nodes nearest to possible root, left to right order)
    ConstLayer& roots(int tsBegin, int tsEnd);

    //! Returns layer of all nodes with selected level, or NULL
    ConstLayer* getLevel(int level)
    {
        return m_nodesByLevel[level];
    }

    //! Make copy of tree
    /*! Target tree will be cleared before coping. Also you can get mapping table between
      * this tree nodes and new tree nodes
      */
    void copyTo(ParseTree *tree, std::map<ParseTreeNode*, ParseTreeNode*> *assocTable = NULL);

    //! Compare two trees
    TreeCompareResult compare(ParseTree *tree);

    //! Check if there is node in tree equivalent to parameter
    ParseTreeNode* hasNode(const ParseTreeNode *node) const;

    //! Return minimal level number in tree
    int minLevel()
    {
        return m_minLevel;
    }

    //! Return maximal level number in tree
    int maxLevel()
    {
        return m_maxLevel;
    }

    //! Clear all nodes that overlays with specified segment
    void clearSegment(int tsBegin, int tsEnd);

    friend class ParseTreeSet;
private:
    //! Whole nodes of tree
    Layer m_allNodes;

    //! Buffer for fast access to leafes of the tree
    Layer m_leafes;

    //! Buffer for fast access to roots of the tree
    Layer m_roots;

    //! Nodes sorted by level (fast access buffer)
    std::map<int, Layer*> m_nodesByLevel;

    //! Maximal level of node in tree
    int m_maxLevel;

    //! Minimal level of node in tree
    int m_minLevel;

    //! Main ParseTreeSet that contain this tree
    ParseTreeSet *m_treeSet;
};

/*! \class ParseTreeSet
  * \brief Set of parse tree
  */
class ParseTreeSet
{
public:
    typedef std::vector<ParseTree*>::iterator Iterator;

    //! Add new tree. Shortcut for trees.push_back(tree)
    void add(ParseTree *tree)
    {
        trees.push_back(tree);
    }

    //! Shortcut to trees.size()
    int size()
    {
        return trees.size();
    }

    //! Clear collection of trees and delete every tree
    void clear()
    {
        Iterator tree;
        for_each_(tree, trees)
            delete *tree;
        trees.clear();
    }

    //! Access operator
    ParseTree* operator[](int index)
    {
        return trees[index];
    }

    //! All the trees
    std::vector<ParseTree*> trees;

    //! Compute average amount of memory used by registered groups
    /*! In fact it calculates only memory used by nodes, but it's quite exact
      */
    unsigned int bytesUsed()
    {
        unsigned int result = 0;
        Iterator tree;
        for_each_(tree, trees)
            result += (*tree)->m_allNodes.size() * sizeof(ParseTreeNode);
        return result;
    }
};

#endif // FL_ENABLE_PARSE_TREE_OPTIMIZATION

//! Shortcut operator
inline void operator+=(ParseTree::ConstLayer::const_iterator &i, int n)
{
    while (n > 0)
        ++i, --n;
}

//! Shortcut operator
inline ParseTree::ConstLayer::const_iterator
    operator+( const ParseTree::ConstLayer::const_iterator &i, int n )
{
    ParseTree::ConstLayer::const_iterator result = i;
    if (n >= 0)
        while (n > 0)
            ++result, --n;
    else
        while (n < 0)
            --result, ++n;
    return result;
}

//! Shortcut operator
inline ParseTree::ConstLayer::const_iterator
    operator-( const ParseTree::ConstLayer::const_iterator &i, int n )
{
    ParseTree::ConstLayer::const_iterator result = i;
    if (n >= 0)
        while (n > 0)
            --result, --n;
    else
        while (n < 0)
            ++result, ++n;
    return result;
}

inline bool isSegmentsOverlay(int seg1Begin, int seg1End, int seg2Begin, int seg2End)
{
    /* first check if there is no overlay */
    // (  )   [  ] or [  ]  (  )
    return ! ( (seg1End < seg2Begin) || (seg2End < seg1Begin) );
}

} // namespace

#endif // PARSETREE_H
