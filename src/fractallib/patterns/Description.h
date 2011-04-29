#ifndef DESCRIPTION_H
#define DESCRIPTION_H

#include <string>
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
    //! Standart constructor
    Description();

    //! Get name of description (same as name of pattern)
    const std::string& name() const;

    //! Set name of description (same as name of pattern)
    void setName(const std::string& newName);

    //! Get id of name of description (same as id of pattern)
    int id() const;

    //! Compile string to description
    FL::Exceptions::EParsing compile(const std::string &s);

    //! Compile stream to description
    virtual FL::Exceptions::EParsing compile(Compilers::Input &i) = 0;

    //! Check if some sequence of description is applicable in context
    virtual bool check(Context &c, CheckInfo &info, CheckOptions check = coNone) = 0;

    //! Get set of sequences in CI-form
    virtual CISet& sequences() = 0;
private:
    int m_id;
};

}} // namespaces

#endif // DESCRIPTION_H
