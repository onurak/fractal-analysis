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

#ifndef CSVFILE_H
#define CSVFILE_H

#include <string>
#include <vector>
#include <iostream>
#include "../Exceptions.h"
#include "../G/GVariant.h"
#include "../TimeSeries.h"

namespace FL {

/*! \addtogroup FLFiles
  */
/*@{*/


/*! \class FileCSV
  * \brief Парсер Comma Separated Values файлов
  */
class FileCSV
{
public:
    //! Default constructor
    /*! \param fileName File name to load
      */
    FileCSV(const std::string & fileName = "");

    //! Loading from file
    /*! \param fileName File name to load
      * \throw EFile
      */
    void loadFromFile(const std::string & fileName);

    //! Save to file
    /*! \param fileName File name to save
      * \throw EFile
      */
    void saveToFile(const std::string & fileName);

    //! Save time series to file
    /*! \param fileName File name to save
      * \param ts Time series to save
      * \param dimension Number of dimension of time series wich must be saved
      * \throw EFile
      */
    void saveToFile(const std::string & fileName, TimeSeries *ts, int dimension = 0);

    //! Get file header strings vector
    /*! \return File header string vector
      */
    std::vector<std::string>& header() { return m_header; }

    //! Return selected row as vector of variants
    /*! \param rowNo Row no
      * \return Row as vector of variants
      * \throw EFile
      */
    std::vector<GVariant>& operator[](int rowNo)
    {
        if (rowNo >= 0 && rowNo < int(m_strings.size()))
            return m_strings[rowNo];
        else {
            throw 0;
        }
    }

    //! Trying guess column with needed data
    /*! \return Number of column with time series values or -1
      * \throw EFile
      */
    int guessTimeSeries();

    //! Load specified column to zero dimension of time series
    /*! \param ts Time series
      * \param columnIndex Index of column to load
      */
    void loadTimeSeries(FL::TimeSeries *ts, int columnIndex);
private:
    //! File header
    std::vector<std::string> m_header;

    //! File rows
    std::vector< std::vector<GVariant> > m_strings;

    //! Parse CSV file
    /*! \param file File to parse
      * \throws EFile
      */
    //void parseFile(ifstream &file);

    //! Save CSV file
    /*! \param file File to save
      */
    void saveFile(std::ostream &file);
};

/*@}*/

} // namespace

#endif // CSVFILE_H
