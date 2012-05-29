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

#ifndef FORECASTER_H
#define FORECASTER_H

#include "Forecast.h"
#include "../TimeSeries.h"
#include "../trees/Forest.h"

namespace FL { namespace Forecasting {

class Forecaster
{
public:
    Forecaster();

    void forecast(TimeSeries &ts, Trees::Forest &forest, Forecast &forecast);

protected:
    void forecastForTree(TimeSeries &ts, Trees::Tree &tree, Forecast &forecast);
    void forecastMaxLength(TimeSeries &ts, Trees::Tree &tree, Forecast &forecast);
    void forecastUnfinished(TimeSeries &ts, Trees::Tree &tree, Forecast &forecast);
    bool checkGuard(Patterns::Context &context, Trees::Node *node);
};

}} // namespaces

#endif // FORECASTER_H
