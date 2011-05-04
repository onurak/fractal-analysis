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
    Metric(double limit = 0)
        : m_limit(limit), m_isEnabled(true)
    {
    }

    virtual ~Metric() {}

    //! Get metric value for concreet tree
    virtual double apply(const Tree &tree, const Forest &forest) = 0;

    //! Check if metric of tree less than limit value
    bool isLimited(const Tree &tree, const Forest &forest)
    {
        return apply(tree, forest) <= m_limit;
    }

    //! Get limit value for metric
    double limit() const { return m_limit; }

    //! Set limit value for metric
    /*! \return Actual value, assigned to limit
      */
    virtual double setLimit(const double &value) { return m_limit = value; }

    //! Get if metric is enabled
    bool isEnabled() const { return m_isEnabled; }

    //! Set if metric is enabled
    void setEnabled(bool value) { m_isEnabled = value; }

    //! Get metric name
    const std::string& name() const { return m_name; }
protected:
    double m_limit;
    std::string m_name;
    bool m_isEnabled;
};

/*! \class MetricsSet
  * \brief Collection of tree metrics
  */
class MetricsSet : public std::vector<Metric*>
{
public:
    MetricsSet()
    {
        createStdMetrics();
    }

    ~MetricsSet()
    {
        cleanup();
    }

    void cleanup()
    {
        FL::cleanup(*this);
    }

    //! Check if all metrics are performed in tree
    bool check(const Tree &tree, const Forest &forest)
    {
        MetricsSet::iterator metric;
        forall(metric, *this)
        {
            if (!(*metric)->isEnabled())
                continue;
            if (!(*metric)->isLimited(tree, forest))
                return false;
        }
        return true;
    }

    Metric* createMetric(const std::string &metricName);

    Metric* getByName(const std::string &metricName);
protected:
    void createStdMetrics();
};

}} // namespaces

#endif // METRICS_H
