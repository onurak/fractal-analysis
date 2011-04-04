#ifndef ENDTIME_H
#define ENDTIME_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

class EndTime : public Function
{
public:
    EndTime();

    //! Main function of class
    virtual const GVariant& operator()(Patterns::Context& context, FunctionArgs& args);
};

}}} // namespaces


#endif // ENDTIME_H
