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

#ifndef TSDATASOURCEFILECSV_H
#define TSDATASOURCEFILECSV_H

#include "TSDataSource.h"
#include "../io/FileCSV.h"
#include <fstream>

namespace FL {

/*! \class TSDataSourceFileCSV
  * \brief Dynamic TS loader form CSV files
  */
class TSDataSourceFileCSV : public TSDataSource
{
private:
    FileCSV m_file;
    int m_posInFile;
    int m_colIdx;
    int columnIndex(const std::string &name);
public:
    //! Collect next values
    virtual bool next(double &value);

    //! Open datasource
    virtual bool open(const std::string &source);

    //! Close opened datasource
    virtual void close();
public:
    //! Default constructor
    TSDataSourceFileCSV();

    //! Set cuurent row index of value in file
    void setCurrentRow(int value);

    //! Destructor
    virtual ~TSDataSourceFileCSV();

private:
    //! Set file to get values from
    /*! Column in file could be optionaly specified. If column not exists than
      * method return false. If no column specified than default columns would
      * be searched (value, <value>, last, <last>, close, <close>). If they will
      * not be found and there are more than one column in file than method will
      * return false.
      * \param fileName File name to open
      * \param Column in CSV file that should be used
      */
    bool openFile(const std::string &fileName, const std::string &column = "");
};

}

#endif // TSDATASOURCEFILE_H
