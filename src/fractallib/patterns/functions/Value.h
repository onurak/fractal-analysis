#ifndef VALUE_H
#define VALUE_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

/*! \class Value
  * \brief Get value (max - min) on segment of time series.
  *
  * Arguments is any number of FL::Trees::Node*.
  * If count of arguments is 0 than it throw EAnalyze exception.
  */
class Value : public Function
{
public:
    Value();

    //! Main function of class
    virtual const GVariant& operator()(Patterns::Context& context, FunctionArgs& args);
};

}}} // namespaces

#endif // VALUE_H
