#ifndef TREEMATCHER_H
#define TREEMATCHER_H

#include "../Matcher.h"

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
    struct TMNode
    {
        TMNode(int id): id(id) {}
        int id;
        //std::vector<Pattern*> patterns;
        std::vector<CheckInfo::ApplicableSeq> sequences;
        std::vector< std::pair<int, TMNode*> > children;
    };

public:
    TreeMatcher(PatternsSet &patterns);

    virtual ~TreeMatcher();

    virtual bool match(Context &context, CheckInfo &ci);

protected:
    void buildTree(PatternsSet &patterns);
    void clearTree(TMNode *root);
    void addTreePath(Pattern &pattern, CISequence &seq);
    void addTreePath(Pattern &pattern, CISequence &seq, TMNode *node, int depth);
private:
    TMNode *m_root;
    int m_treeDepth;
};

}}} // namespaces

#endif // TREEMATCHER_H
