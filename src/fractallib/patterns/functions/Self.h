#ifndef SELF_H
#define SELF_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

class Self : public Function
{
public:
    Self();

    //! Main function of class
    virtual const GVariant& operator()(Patterns::Context& context, FunctionArgs& args);
};

}}} // namespaces

#endif // THIS_H
