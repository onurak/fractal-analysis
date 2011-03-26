#ifndef FILECVS_H
#define FILECVS_H

#include <vector>
#include <string>
#include <fstream>
#include "../../G/GVariant.h"
#include "../TimeSeries.h"
#include "../Common.h"

namespace FL { namespace IO {

class FileCSV
{
public:
    typedef std::vector<std::string> Header;
    typedef std::vector<GVariant> Row;

    enum HeaderType
    {
        htWithHeader,
        htWithoutHeader,
        htAutodetect
    };

    enum ReadResult
    {
        rrOK,
        rrEmpty,
        rrInvalidSize,
        rrEOL
    };

public:
    FileCSV();
    ~FileCSV();

    bool open(const std::string &fileName, HeaderType ht = htAutodetect);
    void close();

    bool save(const std::string &fileName, TimeSeries &ts);

    const Row& row(int i)        const { return m_data[i]; }
    const Row& operator[](int i) const { return m_data[i]; }
    int rowCount() const               { return m_data.size(); }

    int columnIndex(const std::string &name);
    bool read(TimeSeries &ts, int column, TimeSeries::DataType dt);
    bool read(TimeSeries &ts, const std::string &column, TimeSeries::DataType dt);

    std::string fileName() const { return m_fileName; }

    const Header& header() const { return m_header; }
    HeaderType headerType() const { return m_ht; }
protected:
    bool isOpen() const { return m_file.is_open(); }
    HeaderType detectHeader();
    void readHeader();
    int seekRow(int index);
    ReadResult skipRow();
    ReadResult readRow(Row &row);
    ReadResult readValue(GVariant &value);
private:
    std::ifstream m_file;
    std::string m_fileName;
    Header m_header;
    HeaderType m_ht;
    std::vector<Row> m_data;
    int m_lineNo;
    char m_separator;
};

}} // namespaces

#endif // FILECVS_H
