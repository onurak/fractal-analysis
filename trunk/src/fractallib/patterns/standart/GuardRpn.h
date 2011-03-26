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
    GuardRpn(const FL::Patterns::Standart::DescriptionEbnf &description);
    virtual ~GuardRpn();

    virtual FL::Exceptions::EParsing compile(Compilers::Input &i);
    virtual bool check(Context &c, CheckInfo &info);
private:
    Internal::Program *m_rpnProgram;
};

}}} // namespaces

#endif // GUARDRPN_H
