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

#include "TimeSeries.h"

using namespace FL;

TimeSeries::TimeSeries()
{
    m_dimCount = 2;
}

TimeSeries::~TimeSeries()
{
    clear();
}

TimeSeries::Header& TimeSeries::header()
{
    while ((int)m_header.size() < m_dimCount)
        m_header.push_back("");
    return m_header;
}

void TimeSeries::setLinearTime()
{
    m_time.clear();
    for (int i = 0; i < size(); ++i)
        m_time.push_back(i*2);
    header()[1] = "";
}

void TimeSeries::getMinMaxTime(double *min, double *max) const
{
    if (size() == 0)
    {
        if (min) *min = 0;
        if (max) *max = 0;
    }
    else
    {
        if (min) *min = time(0);
        if (max) *max = time(-1);
    }
}

void TimeSeries::getMinMaxValue(double *min, double *max) const
{
    if (size() == 0)
    {
        if (min) *min = 0;
        if (max) *max = 0;
    }
    else
    {
        double dMin = value(0), dMax = value(0);

        Data::const_iterator v;
        forall(v, m_values)
        {
            if (dMin > *v) dMin = *v;
            if (dMax < *v) dMax = *v;
        }

        if (min) *min = dMin;
        if (max) *max = dMax;
    }
}
