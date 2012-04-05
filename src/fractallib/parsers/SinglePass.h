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

#ifndef SINGLEPASS_H
#define SINGLEPASS_H

#include "AbstractParser.h"

namespace FL { namespace Parsers {

/*! \class SinglePass
  * \brief Single pass parser. Find only one parse tree.
  *
  * It always applies petterns in direct order and ignore metrics.
  */
class SinglePass : public AbstractParser
{
public:
    SinglePass();

    virtual ParseResult analyze(
        const TimeSeries &ts,
        Trees::Forest &forest,
        Patterns::PatternsSet &patterns,
        Patterns::Matcher &matcher,
        Trees::MetricsSet &metrics,
        int tsBegin = 0);
protected:
    void analyzeTree(
        const TimeSeries &ts,
        Trees::Tree &tree,
        Patterns::Matcher &matcher,
        int tsBegin);

    bool match(
        Patterns::Matcher &matcher,
        Patterns::Context &context);


    FL::ParseResult m_result;

    Trees::Forest m_forest, m_finishedForest;
    Trees::MetricsSet *m_metrics;
};

}} // namespaces

#endif // SINGLEPASS_H
