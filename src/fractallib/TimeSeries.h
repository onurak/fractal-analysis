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

    inline int size() const { return (int)m_values.size(); }
    inline Data& values() { return m_values; }
    inline const Data& values() const { return m_values; }
    inline double& values(int index)
    {
        if (index < 0)
            return m_values[m_values.size()+index];
        else
            return m_values[index];
    }
    inline Data& time() { return m_time; }
    inline const Data& time() const { return m_time; }
    inline double operator[](int index) { return m_values[index]; }
    inline void clear() { m_values.clear(); m_time.clear(); }

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
