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

#ifndef TSPARSERSIMPLE_H
#define TSPARSERSIMPLE_H

#include "TSParser.h"
#include "../groupcheckers/GroupChecker.h"

namespace FL {

/*! \addtogroup FLAlgorithm
  */
/*@{*/

/*! \class TSParserSimple
  * \brief Fast single pass parser which can extract only one parse tree
  *
  */
class TSParserSimple: public TSParser
{
public:
    /*! \copydoc FL::TSParser::parse()
      * Algorithm: trying to apply all patterns in every point of time series. If
      * some patterns applied - markup segment with this pattern and move to its size.
      * If not pattern can be applied - move to next point. Can find only one parse tree
      */
    ParseResult parse(ParseTreeSet &trees,
                      PatternCollection &patterns,
                      FL::TimeSeries *ts,
                      FL::Patterns::GroupChecker &checker,
                      WorkMode mode = WorkNormal);
protected:
    //! Appling pattern at point
    /*! \param context parse context with current parsing state
      * \param p pattern to apply
      * \param patternSize size of applied pattern (if any)
      * \return NULL if no pattern can be applied, or filled ParseTreeNode if someone can
      */
    ParseTreeNode *applyPattern(ParseContext &context, Pattern *p, int &patternSize);
};

/*@}*/

} // namespace

#endif // TSPARSERSIMPLE_H
