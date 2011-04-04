#ifndef ATONELINE_H
#define ATONELINE_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

class AtOneLine : public Function
{
public:
    AtOneLine();

    //! Main function of class
    virtual const GVariant& operator()(Patterns::Context& context, FunctionArgs& args);
};

}}} // namespaces


#endif // ATONELINE_H
