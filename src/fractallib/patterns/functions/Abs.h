#ifndef ABS_H
#define ABS_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

/*! \class Abs
  * \brief Return absolute value of argument
  *
  * It always has one argument of number type.
  */
class Abs : public Function
{
public:
    Abs();

    //! Main function of class
    virtual const GVariant& operator()(Patterns::Context& context, FunctionArgs& args);
};

}}} // namespaces


#endif // ABS_H
