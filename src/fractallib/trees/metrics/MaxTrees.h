#ifndef MAXTREES_H
#define MAXTREES_H

#include "../Metrics.h"

namespace FL { namespace Trees { namespace Metrics {

class MaxTrees: public Metric
{
public:
    MaxTrees(double limit = 5000) : Metric(limit)
    {
        m_name = "Max trees";
    }

    virtual double apply(const Tree &tree, const Forest &forest)
    {
        return forest.size();
    }

    virtual double setLimit(const double &value)
    {
        return m_limit = value >= 1 ? value : 1;
    }
};

}}} // namespaces

#endif // MAXTREES_H
