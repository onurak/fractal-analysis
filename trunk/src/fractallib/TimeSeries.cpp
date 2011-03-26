#include "TimeSeries.h"

using namespace FL;

TimeSeries::TimeSeries()
{
    m_dimCount = 1;
}

TimeSeries::~TimeSeries()
{
    clear();
}

TimeSeries::Header& TimeSeries::header()
{
    while ((int)m_header.size() < m_dimCount + 1)
        m_header.push_back("");
    return m_header;
}
