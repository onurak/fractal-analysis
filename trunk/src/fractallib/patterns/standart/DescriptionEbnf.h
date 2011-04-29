#ifndef DESCRIPTIONEBNF_H
#define DESCRIPTIONEBNF_H

#include "../Description.h"

namespace FL { namespace Patterns { namespace Standart {

class DescriptionEbnf : public Description
{
public:
    DescriptionEbnf();

    virtual FL::Exceptions::EParsing compile(Compilers::Input &input);
    virtual bool check(Context &c, CheckInfo &info, CheckOptions check = coNone);

    //! Get set of sequences in CI-form
    virtual CISet& sequences() { return m_ebnfSet; }
protected:
    //! Collection of all sequences possible for this description
    CISet m_ebnfSet;
};

}}} // namespaces

#endif // DESCRIPTIONEBNF_H
