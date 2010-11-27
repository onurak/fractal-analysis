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

#ifndef TSMARKER_H
#define TSMARKER_H

#include "../Environment.h"
#include "../TimeSeries.h"
#include "../ParseTree.h"

namespace FL {

/*! \addtogroup FLAlgorithm
  */
/*@{*/


/*! \class TSMarker
  * \brief Class for transition from a time series to a sequence of symbols
  *
  * Children of this class are specifing concrete algorithm (ab, axiomatic etc.)
  */
class TSMarker
{
public:
    //! Markup time series with terminals. In case of error EAnalyze will be thrown
    /*! \param ts Time series to markup
      * \param trees Set of parse trees whereto marked tree will be added
      * \param tsBegin Begin of segment of time series to analyse
      * \param tsEnd End of segment of time series to analyse (-1 to analyse whole time series)
      * \throws EAnalyze
      */
    virtual void analyse(TimeSeries *ts, ParseTreeSet &trees, int tsBegin = 0, int tsEnd = -1) = 0;
};

/*@}*/

} //namespace

#endif // TSMARKER_H
