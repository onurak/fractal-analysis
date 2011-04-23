#ifndef GUARDRPN_H
#define GUARDRPN_H

#include "../Guard.h"
#include "DescriptionEbnf.h"

namespace FL { namespace Patterns { namespace Standart {

namespace Internal
{
    class Program;
}

class GuardRpn : public Guard
{
public:
    typedef std::map<CINode, Internal::Program*, CINodeLessCompare> GuardSet;
public:
    GuardRpn(const FL::Patterns::Standart::DescriptionEbnf &description);
    virtual ~GuardRpn();

    virtual FL::Exceptions::EParsing compile(Compilers::Input &i);
    virtual bool check(Context &c, CheckInfo &info);
private:
    GuardSet m_rpnPrograms;
};

}}} // namespaces

#endif // GUARDRPN_H
