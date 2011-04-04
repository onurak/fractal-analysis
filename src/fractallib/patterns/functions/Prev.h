#ifndef PREV_H
#define PREV_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

class Prev : public Function
{
public:
    Prev();

    //! Main function of class
    virtual const GVariant& operator()(Patterns::Context& context, FunctionArgs& args);
};

}}} // namespaces
#endif // PREV_H
