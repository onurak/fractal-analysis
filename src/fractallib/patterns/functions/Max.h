#ifndef MAX_H
#define MAX_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

class Max : public Function
{
public:
    Max();

    //! Main function of class
    virtual const GVariant& operator()(Patterns::Context& context, FunctionArgs& args);
};

}}} // namespaces

#endif // MAX_H
