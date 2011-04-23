#ifndef METRICS_H
#define METRICS_H

#include "Forest.h"

namespace FL { namespace Trees {

/*! \class Metric
  * \brief Abstract metric of tree
  */
class Metric
{
public:
    Metric();

    virtual double apply(const Tree &tree) = 0;
};

/*! \class MetricsSet
  * \brief Collection of tree metrics
  */
class MetricsSet : public std::vector<Metric>
{
public:
};

}} // namespaces

#endif // METRICS_H
