#ifndef MIN_H
#define MIN_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

class Min : public Function
{
public:
    Min();

    virtual const GVariant& operator()(Patterns::Context &context, FunctionArgs &args);
};

}}} // namespaces

#endif // MIN_H
