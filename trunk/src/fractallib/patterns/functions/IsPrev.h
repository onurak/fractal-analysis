#ifndef ISPREV_H
#define ISPREV_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

/*! \class IsPrev
  * \brief Check if previous to current node have specified name
  *
  * Argument is a string node name.
  * Result is true or false.
  */
class IsPrev : public Function
{
public:
    IsPrev();

    virtual const GVariant& operator()(Patterns::Context &context, FunctionArgs &args);
};

}}} // namespaces

#endif // ISPREV_H
