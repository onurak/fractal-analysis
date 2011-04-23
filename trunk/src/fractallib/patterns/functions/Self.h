#ifndef SELF_H
#define SELF_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

/*! \class Self
  * \brief Return pointer to currently analyzing node.
  *
  * It don't want any arguments.
  * Return pointer to FL::Trees::Node* that just was recognized and analyzing now.
  */
class Self : public Function
{
public:
    Self();

    //! Main function of class
    virtual const GVariant& operator()(Patterns::Context& context, FunctionArgs& args);
};

}}} // namespaces

#endif // THIS_H
