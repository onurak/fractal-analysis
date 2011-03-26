#ifndef EBNFRPNPATTERN_H
#define EBNFRPNPATTERN_H

#include "../Pattern.h"
#include "GuardRpn.h"
#include "DescriptionEbnf.h"

namespace FL { namespace Patterns { namespace Standart {

class EbnfRpnPattern : public PatternConstructor
{
public:
    EbnfRpnPattern() : PatternConstructor() {}
    virtual Guard* createGuard(Description &d) const;
    virtual Description* createDescription() const;
};

}}} // namespaces

#endif // EBNFRPNPATTERN_H
