#ifndef MAX_H
#define MAX_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

/*! \class Max
  * \brief Get maximum of time series in the set of parse tree nodes
  *
  * Arguments are any number of FL::Trees::Node*.
  * It finds maximum of time series among this nodes.
  * Throw EArguments exception if arguments are not valid.
  */
class Max : public Function
{
public:
    Max();

    //! Main function of class
    virtual const GVariant& operator()(Patterns::Context& context, FunctionArgs& args);
};

}}} // namespaces

#endif // MAX_H
