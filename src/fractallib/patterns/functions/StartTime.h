#ifndef STARTTIME_H
#define STARTTIME_H

#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

/*! \class StartTime
  * \brief Return start time of node of tree
  *
  * It always has one argument of type FL::Trees::Node*.
  * It returns its starting time.
  */
class StartTime : public Function
{
public:
    StartTime();

    //! Main function of class
    virtual const GVariant& operator()(Patterns::Context& context, FunctionArgs& args);
};

}}} // namespaces


#endif // STARTTIME_H
