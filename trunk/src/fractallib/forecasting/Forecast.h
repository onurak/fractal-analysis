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

#ifndef FORECAST_H
#define FORECAST_H

#include "../Common.h"
#include "../trees/Tree.h"

namespace FL { namespace Forecasting {

/*! \class ForecastItem
  * \brief Rectangle where time series future values could appears.
  *
  */
class ForecastItem
{
public:
    ForecastItem():
        pos(0), minDuration(0), maxDuration(0), minValue(0), maxValue(0), tree(NULL)
    {}

    ~ForecastItem() {}

    int pos;
    double minDuration, maxDuration;
    double minValue, maxValue;
    std::string patternName;
    FL::Trees::Tree *tree;
};

/*! \class Forecast
  * \brief Forecast of time series future values
  */
class Forecast : public std::vector<ForecastItem>
{
public:
    Forecast() {}

    void nullTrees()
    {
        iterator i;
        forall(i, *this)
            i->tree = NULL;
    }
};

}} // namespaces

#endif // FORECAST_H
