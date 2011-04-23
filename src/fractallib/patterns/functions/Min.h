#ifndef MIN_H
#define MIN_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

/*! \class Min
  * \brief Get minimum of time series in the set of parse tree nodes
  *
  * Arguments are any number of FL::Trees::Node*.
  * It finds minimum of time series among this nodes.
  * Throw EArguments exception if arguments are not valid.
  */
class Min : public Function
{
public:
    Min();

    virtual const GVariant& operator()(Patterns::Context &context, FunctionArgs &args);
};

}}} // namespaces

#endif // MIN_H
