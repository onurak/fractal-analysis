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

#include "FileCSV.h"

using namespace FL::IO;

FileCSV::FileCSV()
{
    m_separator = ';';
}


FileCSV::~FileCSV()
{
    close();
}

bool FileCSV::open(const std::string &fileName, HeaderType ht)
{
    close();
    m_file.open(fileName.c_str());
    if (!m_file.is_open())
        return false;

    // Read header
    m_ht =  (ht != htAutodetect)  ? ht : detectHeader();
    readHeader();

    // Read data
    //while (readRow())

    return true;
}

void FileCSV::close()
{
    if (isOpen())
        m_file.close();
}

bool FileCSV::save(const std::string &fileName, TimeSeries &ts)
{
    std::ofstream file(fileName.c_str());
    if (!file.is_open())
        return false;

    // Write header
    for (unsigned int i = 0; i < ts.header().size(); ++i)
    {
        file << ts.header()[i];
        if (i < ts.header().size() - 1)
            file << m_separator;
    }

    // Write data
    for (int row = 0; row < (int)ts.time().size(); ++row)
    {
        file << std::endl << ts.time()[row] << m_separator << ts.values()[row];
    }
    file << std::endl;
    file.close();

    return true;
}

void FileCSV::readHeader()
{
    m_header.clear();
    m_file.seekg(0);
    m_lineNo = 0;

    if (m_ht == htWithHeader && isOpen())
    {
        Row row;
        if (readRow(row) == rrOK)
        {
            Row::const_iterator i;
            forall(i, row)
                m_header.push_back(std::string((char*)(*i)));
        }
    }
    else
    {
        Row row;
        if (readRow(row) == rrOK)
        {
            for (int i = 0; i < int(row.size()); ++i)
                m_header.push_back(intToString(i+1));
        }
        m_file.seekg(0);
        m_lineNo = 0;
    }
}

FileCSV::ReadResult FileCSV::readRow(Row &row)
{
    row.clear();
    if (!isOpen() || m_file.eof())
        return rrEmpty;

    ReadResult rr;
    while (true)
    {
        GVariant value;
        rr = readValue(value);
        if (rr == rrEmpty)
            break;
        row.push_back(value);
        if (rr == rrEOL)
            break;
    }

    if (rr != rrEmpty && m_lineNo++ > 0 && row.size() != m_header.size())
        return rrInvalidSize;
    return rr == rrEmpty ? rrEmpty : rrOK;
}

FileCSV::ReadResult FileCSV::skipRow()
{
    if (!isOpen() || m_file.eof())
        return rrEmpty;

    GVariant value;
    unsigned int readCount = 0;
    while (true)
    {
        ReadResult rr = readValue(value);
        readCount++;
        if (rr == rrEOL || rr == rrEmpty)
            break;
    }

    if (m_lineNo++ > 0 && readCount != m_header.size())
        return rrInvalidSize;
    return rrOK;
}

FileCSV::ReadResult FileCSV::readValue(GVariant &value)
{
    if (!isOpen() || m_file.eof())
        return rrEmpty;

    char c;
    std::string v;

    FileCSV::ReadResult result = rrOK;
    m_file.read(&c, 1);
    if (m_file.eof())
        return rrEmpty;

    while (c == ' ' || c == '\t')
        m_file.read(&c, 1);
    // Extract value
    if (c != '"')
    {
        do
        {
            if (c != m_separator && c != '\n' && !m_file.eof())
                v += c;
            m_file.read(&c, 1);
        } while (c != m_separator && c != '\n' && !m_file.eof());
        if (c == '\n')
            result = rrEOL;
    }
    else
    {
        do
        {
            if (c != '"' && c != '\n' && !m_file.eof())
                v += c;
            m_file.read(&c, 1);
        } while (c != '"' && c != '\n' && !m_file.eof());
        if (c == '\n')
            result = rrEOL;
        while ((c == '\t' || c == ' ') && !m_file.eof()) m_file.read(&c, 1);
    }

    // Try determine type
    value = v;
    if (value.canCastTo(G_VAR_INT))
        value = (int)value;
    else if (value.canCastTo(G_VAR_DOUBLE))
        value = (double)value;

    return result;
}

FileCSV::HeaderType FileCSV::detectHeader()
{
    m_file.seekg(0);
    m_lineNo = 0;

    Row row1, row2;
    HeaderType result = htWithoutHeader;
    try
    {
        if (readRow(row1) != rrOK)
            throw 0;
        ReadResult rr2 = readRow(row2);
        if (rr2 != rrOK && rr2 != rrInvalidSize)
            throw 0;
        Row::const_iterator i1, i2;
        for (i1 = row1.begin(), i2 = row2.begin();
             i1 != row1.end() && i2 != row2.end();
             ++i1, ++i2)
        {
            if (i1->canCastTo(G_VAR_DOUBLE) ^ i2->canCastTo(G_VAR_DOUBLE))
                throw 1;
        }
    }
    catch (int e) { if (e) result = htWithHeader; }

    m_file.seekg(0);
    m_lineNo = 0;

    return result;
}

int FileCSV::seekRow(int index)
{
    if (!isOpen())
        return -1;

    m_file.clear();
    m_file.seekg(0);
    m_lineNo = 0;

    if (m_ht == FileCSV::htWithHeader)
        if (skipRow() != rrOK) return -1;

    while (m_lineNo-1 < index)
        if (skipRow() != rrOK)
            break;
    return m_lineNo;
}

bool FileCSV::read(TimeSeries &ts, int column, TimeSeries::DataType dt)
{
    if (!isOpen() || column < 0 || column >= (int)m_header.size())
        return false;

    seekRow(0);

    Row row;
    ReadResult rr;

    if (dt == TimeSeries::dtValues)
    {
        ts.header()[1] = m_header[column];
        while ( (rr = readRow(row)) == rrOK )
            ts.values().push_back(row[column]);
    }
    else
    {
        ts.header()[0] = m_header[column];
        while ( (rr = readRow(row)) == rrOK )
            ts.time().push_back(row[column]);
    }

    return rr == rrEmpty;
}

bool FileCSV::read(TimeSeries &ts, const std::string &column, TimeSeries::DataType dt)
{
    return read(ts, this->columnIndex(column), dt);
}

int FileCSV::columnIndex(const std::string &name)
{
    FileCSV::Header::const_iterator i;
    forall(i, m_header)
        if (name == *i)
            return i - m_header.begin();
    return -1;
}
