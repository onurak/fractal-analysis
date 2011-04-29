#ifndef SUBTREE_H
#define SUBTREE_H

#include "../Metrics.h"

namespace FL { namespace Trees { namespace Metrics {

/*! \class Subtree
  * \brief Subtree metric checks if specified tree is subtree of another tree
  *
  * It return 1 if it's tree or 0 otherwise.
  * So if limit is 0 then subtrees not acceptable for metrics and 1 otherwise
  */
class Subtree : public Metric
{
public:
    Subtree(double limit = 0) : Metric(limit)
    {
        m_name = "Subtree";
    }

    virtual double apply(const Tree &tree, const Forest &forest)
    {
        Forest::ConstIterator itTree;

        for (itTree = forest.begin(); itTree != forest.end(); ++itTree)
        {
            if (*itTree == &tree)
                continue;
            TreeCompareResult tcr = (*itTree)->compare(tree);
            if (tcr.isSecondSubtreeOfFirst())
                return 1;
        }

        return 0;
    }

    virtual double setLimit(const double &value)
    {
        return m_limit = value == 0 ? 0 : 1;
    }
};


}}} // namespaces


#endif // SUBTREE_H
