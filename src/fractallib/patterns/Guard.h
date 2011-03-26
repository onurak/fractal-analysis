#ifndef GUARD_H
#define GUARD_H

#include "Description.h"
#include "functions/Function.h"

namespace FL { namespace Patterns {

class Context;

class Guard
{
public:
    Guard(const FL::Patterns::Description &description);
    virtual ~Guard() {}

    FL::Exceptions::EParsing compile(const std::string &s);
    virtual FL::Exceptions::EParsing compile(Compilers::Input &i) = 0;

    virtual bool check(Context &c, CheckInfo &info) = 0;
protected:
    const FL::Patterns::Description &m_description;
};

}} // namespaces

#endif // GUARD_H
