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

#ifndef TIMESERIES_H
#define TIMESERIES_H

#include <vector>
#include <string>
#include "AxiomLib.h"

namespace FL {

/*! \class TimeSeries
  * \brief Time series representation
  *
  * Why does it inherited from AxiomLib::MultiTS?
  *
  * - Hystorical reason at most, also for better compatability with AxiomLib
  */
class TimeSeries: public AxiomLib::MultiTS {
public:
    //! Time stamps of the ticks (x-coord)
    std::vector< std::vector<std::string> > time; // never used yet

    //! Size of the time series number @dimension, or -1 if it not exists
    int dimSize(int dimension) {
        if (dimension >= 0 && dimension < int(validParams.size()) && validParams[dimension])
            return data[dimension].size();
        return -1;
    }

    //! Adds value to time series number 0. If it's not exists it will be created
    void add(double value) {
        if (data.size() == 0) {
            validParams.clear();
            std::vector<double> v;
            v.push_back(value);
            data.push_back(v);
            validParams.push_back(true);
        } else
            data[0].push_back(value);
    }

    //! Removes from time series value number @index. Do nothing in case of error
    void remove(int index) {
        int dsz = dimSize(0);
        if (dsz != -1 && index >= 0 && index < dsz) {
            std::vector<double>::iterator p;
            for (p = data[0].begin(); index > 0 && p != data[0].end(); )
                index--, p++;
            data[0].erase(p);
        }

    }
};

} // namespace

#endif // TIMESERIES_H
