#ifndef ISPREV_H
#define ISPREV_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

class IsPrev : public Function
{
public:
    IsPrev();

    virtual const GVariant& operator()(Patterns::Context &context, FunctionArgs &args);
};

}}} // namespaces

#endif // ISPREV_H
