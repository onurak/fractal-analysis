#ifndef MAXLEVELSCOUNT_H
#define MAXLEVELSCOUNT_H

#include "../Metrics.h"

namespace FL { namespace Trees { namespace Metrics {

class MaxLevelsCount : public Metric
{
public:
    MaxLevelsCount(double limit = 3) : Metric(limit)
    {
        m_name = "Max levels count";
    }

    virtual double apply(const Tree &tree, const Forest &forest)
    {
        return tree.levelCount();
    }

    virtual double setLimit(const double &value)
    {
        return m_limit = value >= 0 ? value : 0;
    }
};


}}} // namespaces

#endif // MAXLEVELSCOUNT_H
