#ifndef EEXCEPTION_H
#define EEXCEPTION_H

#include "../Common.h"

namespace FL {

    static const int E_OK = 0;

} // namespaces

namespace FL { namespace Exceptions {

class EException
{
public:
    EException(int id, const std::string& arg);
    EException(const EException &e): m_id(e.m_id), m_arg(e.arg()) {}

    int id() const;
    const std::string& arg() const;
protected:
    int m_id;
    std::string m_arg;
};

class EParsing: public EException
{
public:
    EParsing(int id = E_OK, int line = -1, int column = -1, const std::string &arg = "");
    void set(int id, int line = -1, int column = -1, const std::string &arg = "");
    const EParsing& operator=(const EParsing& e);

    int line() const;
    int column() const;
private:
    int m_line;
    int m_column;
};

class EAnalyze : public EException
{
public:
    EAnalyze(int id = E_OK, const std::string& arg = "") : EException(id, arg) {}
};

}} // namespaces

#endif // EEXCEPTION_H
