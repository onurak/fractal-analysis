#ifndef NEAR_H
#define NEAR_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

class Near : public Function
{
public:
    Near();

    //! Main function of class
    virtual const GVariant& operator()(Patterns::Context& context, FunctionArgs& args);
};

}}} // namespaces


#endif // NEAR_H
