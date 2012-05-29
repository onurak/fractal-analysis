#ifndef TREEMATCHER_H
#define TREEMATCHER_H

#include "../Matcher.h"
#include <queue>

namespace FL { namespace Patterns { namespace Standard {

/*! \class TreeMatcher
  * \brief Patterns matcher based on variant of Aho–Corasick string matching algorithm
  *
  * Initialization performed in constructor may take some time so
  * better do it once BEFORE analysis.
  */
class TreeMatcher : public Matcher
{
public:
    struct TMNode;
    typedef std::map<int, TMNode*> TMChildren;

    struct TMNode
    {
        TMNode(int id): id(id) {}
        int id;
        CheckInfo::ASVec sequences;
        TMChildren children;
    };

public:
    TreeMatcher(PatternsSet &patterns);

    virtual ~TreeMatcher();

    virtual bool match(Context &context, CheckInfo &ci);

protected:
    bool match(Context &context,
               CheckInfo &ci,
               TMNode *treeNode,
               std::list<FL::Trees::Node*>::const_iterator itLayerNode);

    void buildTree(PatternsSet &patterns);
    void clearTree(TMNode *root);
    void addTreePath(Pattern &pattern, CISequence &seq);
    void addTreePath(Pattern &pattern, CISequence &seq, TMNode *node, int depth);

    //! Look for patterns that can be finished from specified tree node
    void getUnfinishedPatterns(Context &context, TMNode *node, CheckInfo::ASVec &result);
private:
    TMNode *m_root;
    int m_treeDepth;
};

}}} // namespaces

#endif // TREEMATCHER_H
