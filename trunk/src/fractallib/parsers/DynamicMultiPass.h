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

#ifndef DYNAMICMULTIPASS_H
#define DYNAMICMULTIPASS_H

#include "AbstractDynamicParser.h"

namespace FL { namespace Parsers {

/*! \class DynamicMultiPass
  * \brief Dynamic multi pass (recursive) parser. Can find all parse trees.
  *
  */
class DynamicMultiPass : public AbstractDynamicParser
{
public:
    DynamicMultiPass();

    virtual ParseResult analyze(
        const TimeSeries &ts,
        Trees::Forest &forest,
        Patterns::PatternsSet &patterns,
        Trees::MetricsSet &metrics,
        Forecast &forecast);

protected:
    //! Update info on known possible patterns
    void updatePossiblePatterns(Trees::Forest &forest);

    //! Find all potential patterns
    void forecastTreeStructure(Trees::Forest &forest);

    //! Make time series forecast
    void forecastTimeSeries(const TimeSeries &ts,
                       Trees::Forest &forest,
                       Patterns::PatternsSet &patterns,
                       Forecast &forecast);

    bool isSubtreeOfSomeTree(Trees::Tree *tree);
protected:

    void newAnalysisBranchForTree(FL::Trees::Tree &tree, int indentSize);

    void newAnalysisBranch(Patterns::Context *context);

    void runBranch(Patterns::Context *context);

    void applyPattern(Patterns::Pattern &pattern, Patterns::Context &context);

    bool m_wantSubtree;
    int m_maxTrees;
    int m_maxLevelCount;

protected:
    bool m_interruption;
    FL::ParseResult m_result;

    std::vector<Patterns::Context*> m_branches;
    FL::Trees::Forest m_oldForest, m_newForest;

    FL::Patterns::PatternsSet *m_patterns;
    const FL::TimeSeries *m_ts;
};

}} // namespaces

#endif // DYNAMICMULTIPASS_H
