#ifndef PREV_H
#define PREV_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

/*! \class Prev
  * \brief Return pointer to one of the previous root node.
  *
  * If no arguments specified, than it returns pointer to previous root node
  * or NULL if it is first root node.
  *
  * Optional string argument "samelevel" could be passed to indicate that root
  * must be at the same level. You also could specify "nosamelevel" to indicate
  * default behaviour.
  *
  * Return pointer to specified FL::Trees::Node* or NULL if there is no such node.
  */
class Prev : public Function
{
public:
    Prev();

    //! Main function of class
    virtual const GVariant& operator()(Patterns::Context& context, FunctionArgs& args);
};

}}} // namespaces
#endif // PREV_H
