#include "EbnfRpnPattern.h"

using namespace FL::Patterns;
using namespace FL::Patterns::Standart;

Guard* EbnfRpnPattern::createGuard(Description &d) const
{
    DescriptionEbnf *dEbnf = dynamic_cast<DescriptionEbnf*>(&d);
    if (dEbnf != NULL)
        return new GuardRpn(*dEbnf);
    else
        return NULL;
}

Description* EbnfRpnPattern::createDescription() const
{
    return new DescriptionEbnf();
}
