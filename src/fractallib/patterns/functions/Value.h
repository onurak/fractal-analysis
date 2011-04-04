#ifndef VALUE_H
#define VALUE_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

class Value : public Function
{
public:
    Value();

    //! Main function of class
    virtual const GVariant& operator()(Patterns::Context& context, FunctionArgs& args);
};

}}} // namespaces

#endif // VALUE_H
