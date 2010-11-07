/* Author: Vladimir Belikov
 *
 * This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Fractal Libray.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TSPARSERIDEAL_H
#define TSPARSERIDEAL_H

#include "TSParser.h"

namespace FL {

class TSParserIdeal : public TSParser
{
public:
    /*! \copydoc FL::TSParser::parse()
      * Ideal parser - slow, but can find ALL posible parse trees
      */
    ParseResult parse(ParseTreeSet &trees,
                      PatternCollection &pattern,
                      FL::TimeSeries *ts,
                      FL::Patterns::GroupChecker &checker,
                      WorkMode mode = WorkNormal);

protected:    
    //! Recursive analysis function
    void ranalyse(ParseContext *context, int no);

    //! Run new analysis brunch
    void newBranch(ParseContext *context);

    //! Appling pattern at point
    /*! \param context parse context with current parsing state
      * \param p pattern to apply
      * \param patternSize size of applied pattern (if any)
      * \param newContext variable whereto new context will be writen
      * \return true if pattern applicable, false otherwise
      */
    bool applyPattern(ParseContext &context, Pattern *p, int &patternSize, ParseContext **newContext);

private:
    //! Number of ran brunches
    int m_branchNo;

    //! Number of completed brunches
    int m_completedBranches;

    //! Current work mode
    TSParser::WorkMode m_mode;
};

} // namespace

#endif // TSPARSERIDEAL_H
