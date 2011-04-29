#include "Metrics.h"
#include "metrics/MaxLevelsCount.h"
#include "metrics/Subtree.h"
#include "metrics/MaxTrees.h"

using namespace FL::Trees;
using namespace FL::Trees::Metrics;

Metric* MetricsSet::createMetric(const std::string &metricName)
{
    if (metricName == "Max levels count")
        return new Metrics::MaxLevelsCount();
    if (metricName == "Subtree")
        return new Metrics::Subtree();
    if (metricName == "Subtree")
        return new Metrics::Subtree();

    return NULL;
}

void MetricsSet::createStdMetrics()
{
    push_back(new Metrics::Subtree());
    push_back(new Metrics::MaxLevelsCount());
    push_back(new Metrics::MaxTrees());
}

Metric* MetricsSet::getByName(const std::string &metricName)
{
    iterator i;
    forall(i, *this)
       if ((*i)->name() == metricName)
            return *i;
    return NULL;
}
