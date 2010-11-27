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

#ifndef TSDATASOURCE_H
#define TSDATASOURCE_H

#include "../TimeSeries.h"
#include "../G/GDelegate.h"
#include "../Exceptions.h"

namespace FL {

/*! \class TSDataSource
  * \brief Collect new values of time series if it dynamic
  *
  */
class TSDataSource
{
protected:
    //! Buffer for new values
    std::vector<double> m_collected;

    //! Datasource description
    std::string m_source;

    //! Is it opened
    bool m_isOpen;

    //! Add value to buffer
    void bufferIt(const double &value)
    {
        m_collected.push_back(value);
    }
public:
    //! Open datasource
    virtual bool open(const std::string &source) = 0;

    //! Close opened datasource
    virtual void close() = 0;

    //! Collect next values
    virtual bool next(double &value) = 0;
public:
    //! Default constructor
    TSDataSource()
    {
        m_isOpen = false;
    }       

    //! Destructor
    virtual ~TSDataSource()
    {
    }

    //! Reset
    void reset()
    {
        m_collected.clear();
    }

    //! Return true if there are no values in buffer ready to be proccessed
    bool isEmpty() const
    {
        return m_collected.size() > 0;
    }

    //! Return true if datasource is closed
    bool isClosed() const
    {
        return !m_isOpen;
    }
};

}

#endif // TSDATASOURCE_H
