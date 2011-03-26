#ifndef DESCRIPTION_H
#define DESCRIPTION_H

#include <string>
#include "SynonymsTable.h"
#include "../exceptions/EException.h"
#include "../compilers/AbstractCompiler.h"
#include "CheckInfo.h"

namespace FL { namespace Patterns {

class Context;

/*! \class Description
  * \brief Description (structure) part of Pattern
  */
class Description
{
public:
    Description();

    const std::string& name() const;
    void setName(const std::string& newName);
    int id() const;


    FL::Exceptions::EParsing compile(const std::string &s);
    virtual FL::Exceptions::EParsing compile(Compilers::Input &i) = 0;

    virtual bool check(Context &c, CheckInfo &info) = 0;
private:
    std::string m_name;
    int m_id;
};

}} // namespaces

#endif // DESCRIPTION_H
