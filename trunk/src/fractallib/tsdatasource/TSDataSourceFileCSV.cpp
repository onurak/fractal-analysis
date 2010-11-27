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

#include "TSDataSourceFileCSV.h"

using namespace FL;

TSDataSourceFileCSV::TSDataSourceFileCSV()
    : TSDataSource()
{
    m_posInFile = -1;
}

TSDataSourceFileCSV::~TSDataSourceFileCSV()
{
    m_file.clear();
}

bool TSDataSourceFileCSV::open(const std::string &source)
{
    m_isOpen = openFile(source);
    if (m_isOpen)
        m_source = source;
    return m_isOpen;
}

void TSDataSourceFileCSV::close()
{
    m_file.clear();
    m_colIdx = 0;
    m_posInFile = -1;
    m_collected.clear();
    m_isOpen = false;
    m_source = "";
}

bool TSDataSourceFileCSV::next(double &value)
{
    if (m_file.rowCount() == 0 || ++m_posInFile >= m_file.rowCount() || m_colIdx == -1)
        return false;
    //bufferIt(m_file[m_posInFile][m_colIdx]);
    value = m_file[m_posInFile][m_colIdx];
    return true;
}

void TSDataSourceFileCSV::setCurrentRow(int value)
{
    if (value >= 0 && value < m_file.rowCount())
        m_posInFile = value;
}

bool TSDataSourceFileCSV::openFile(const std::string &fileName,
                                   const std::string &column)
{
    m_file.clear();
    m_posInFile = -1;
    if (fileName.empty())
        return false;

    if (!m_file.loadFromFile(fileName))
        return false;

    if (m_file.rowCount() == 0)
    {
        return false;
    }

    m_colIdx = -1;
    if (column != "")
    {
        m_colIdx = columnIndex(column);
    }
    else
    {
        if (m_file.header().size() == 1)
            m_colIdx = 0;
        else
        {
            const std::string defaultColumns[] =
                { "value", "<value>", "last", "<last>", "close", "<close>" };
            for (int i = 0; i < 6; ++i)
            {
                m_colIdx = columnIndex(defaultColumns[i]);
                if (m_colIdx != -1)
                    break;
            }
        }
    }
    if (m_colIdx == -1)
        return false;

    return true;
}

int TSDataSourceFileCSV::columnIndex(const std::string &name)
{
    for (int i = 0; i < int(m_file.header().size()); ++i)
    {
        std::string s = m_file.header()[i];
        if (s == name)
            return i;

    }
    return -1;
}
