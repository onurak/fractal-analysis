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
    inline Data& values() { return m_values; }
    inline const Data& values() const { return m_values; }
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
