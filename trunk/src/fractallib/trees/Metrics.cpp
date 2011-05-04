/** This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Fractal Library. If not, see <http://www.gnu.org/licenses/>.
 */

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
