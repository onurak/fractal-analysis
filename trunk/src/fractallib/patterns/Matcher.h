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
    Matcher()
    {
        m_allowUnfinished = false;
    }

    virtual ~Matcher() {}

    virtual bool match(Context &context, CheckInfo &ci) = 0;

    //! Is also look for incomplete matching patterns in the end of layer
    inline bool isAllowUnfinished() const { return m_allowUnfinished; }

    //! Set if also look for incomplete matching patterns in the end of layer
    inline void setAllowUnfinished(bool value) { m_allowUnfinished = value; }

private:
    bool m_allowUnfinished;
};

}} // namespaces

#endif // MATCHER_H
