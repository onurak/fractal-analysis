#ifndef ISNODE_H
#define ISNODE_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

/*! \class IsNode
  * \brief Check if current node have specified id and index
  *
  * It takes at least one argument of node type.
  * Return true or false.
  */


class IsNode : public Function
{
public:
    IsNode();

    //! Main function of class
    virtual const GVariant& operator()(Patterns::Context& context, FunctionArgs& args);
};

}}} // namespaces

#endif // ISNODE_H
