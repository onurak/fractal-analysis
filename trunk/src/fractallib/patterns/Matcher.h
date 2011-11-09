#ifndef MATCHER_H
#define MATCHER_H

#include "Pattern.h"

namespace FL { namespace Patterns {

/*! \class Matcher
  * \brief Find matches with set of patterns in current analysis context
  *
  * It is interface, see actual implementations
  */
class Matcher
{
public:
    virtual ~Matcher() {}

    virtual bool match(Context &context, CheckInfo &ci) = 0;
};

}} // namespaces

#endif // MATCHER_H
