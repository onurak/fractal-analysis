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

EArguments::EArguments(const std::string &name, int expectedCount, int gotCount)
{
    m_id = E_INVALID_COUNT;
    m_arg = "In function " + name + ": invalid arguments count (expected ";
    if (expectedCount >= 0)
        m_arg += intToString(expectedCount);
    else
        m_arg += "at least " + intToString(-expectedCount);
    m_arg += ", got " + intToString(gotCount) + ")";
}

EArguments::EArguments(const std::string &name, int expectedCountFrom,
                       int expectedCountTo, int gotCount)
{
    m_id = E_INVALID_COUNT;
    m_arg = "In function " + name + ": invalid arguments count (expected " +
            intToString(expectedCountFrom) + "-" + intToString(expectedCountTo) +
            " arguments, got " + intToString(gotCount) + ")";
}

EArguments::EArguments(
    const std::string &name, const std::string &expectedType,
    const std::string &gotType, int argIndex)
{
    m_id = E_INVALID_COUNT;
    m_arg = "In function " + name + ": invalid type of ";
    if (argIndex >= 0)
        m_arg += intToString(argIndex);
    m_arg += " argument (expected " + expectedType;
    if (!gotType.empty())
        m_arg += ", got " + gotType;
    m_arg += ")";
}

EArguments::EArguments(const std::string &name, const std::string &arg, int argIndex)
{
    m_id = E_UNKNOWN_ARG;
    m_arg = "In function " + name + ": unknown type of ";
    if (argIndex >= 0)
        m_arg += intToString(argIndex);
    m_arg += " argument (" + arg + ")";
}
