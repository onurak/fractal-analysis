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

#ifndef TIMESERIS_H
#define TIMESERIS_H

#include <vector>
#include <string>
#include "Common.h"

namespace FL {

class TimeSeries
{
public:
    typedef std::vector<double> Data;
    typedef std::vector<std::string> Header;

    enum DataType
    {
        dtValues,
        dtTime
    };

public:
    TimeSeries();
    ~TimeSeries();

    inline int size() const
    {
        return (int) m_values.size();
    }

    inline double& value(int index)
    {
        if (index < 0)
            return m_values[m_values.size()+index];
        else
            return m_values[index];
    }

    inline const double& value(int index) const
    {
        if (index < 0)
            return m_values[m_values.size()+index];
        else
            return m_values[index];
    }

    inline double& time(int index)
    {
        if (index < 0)
            return m_time[m_time.size()+index];
        else
            return m_time[index];
    }

    inline const double& time(int index) const
    {
        if (index < 0)
            return m_time[m_time.size()+index];
        else
            return m_time[index];
    }

    inline void clear()
    {
        m_values.clear();
        m_time.clear();
    }

    inline bool setData(const Data &values, const Data &time)
    {
        if (values.size() != time.size())
            return false;

        m_values.assign(values.begin(), values.end());
        m_time.assign(time.begin(), time.end());

        return true;
    }

    inline bool setValues(const Data &values)
    {
        m_values.assign(values.begin(), values.end());
        return true;
    }

    inline bool setTime(const Data &time)
    {
        m_time.assign(time.begin(), time.end());
        return true;
    }

    inline void append(double time, double value)
    {
        m_time.push_back(time);
        m_values.push_back(value);
    }

    inline void remove(int index, int count = 1)
    {
        if (index < 0 || count <= 0 || index + count > size())
            return;

        m_values.erase(m_values.begin() + index, m_values.begin() + index + count);
        m_time.erase(m_time.begin() + index, m_time.begin() + index + count);
    }

    void getMinMaxTime(double *min, double *max) const;
    void getMinMaxValue(double *min, double *max) const;


    void setLinearTime();

    Header& header();
    const Header& header() const { return m_header; }
protected:
    Data m_values;
    Data m_time;
    Header m_header;
    int m_dimCount;
};

} // namespace

#endif // TIMESERIS_H
