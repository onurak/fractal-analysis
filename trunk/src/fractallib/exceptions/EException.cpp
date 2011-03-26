#include "EException.h"

using namespace FL::Exceptions;

EException::EException(int id, const std::string& arg)
    : m_id(id), m_arg(arg)
{
}

int EException::id() const
{
    return m_id;
}

const std::string& EException::arg() const
{
    return m_arg;
}

EParsing::EParsing(int id, int line, int column, const std::string &arg)
    : EException(id, arg), m_line(line), m_column(column)
{
}

void EParsing::set(int id, int line, int column, const std::string &arg)
{
    m_id = id;
    m_line = line;
    m_column = column;
    m_arg = arg;
}

int EParsing::line() const
{
    return m_line;
}

int EParsing::column() const
{
    return m_column;
}

const EParsing& EParsing::operator=(const EParsing& e)
{
    m_line = e.line();
    m_column = e.column();
    m_id = e.id();
    m_arg = e.arg();
    return *this;
}
