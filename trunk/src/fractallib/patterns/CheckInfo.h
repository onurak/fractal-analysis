#ifndef CHECKINFO_H
#define CHECKINFO_H

#include "../Common.h"

namespace FL { namespace Patterns
{

//! Node for FL::Patterns::CheckInfo
struct CINode
{
    int id;
    int index;
};

//! Sequence of CINode
typedef std::vector<CINode> CISequence;

//! Set of CISequences
typedef std::vector<CISequence> CISet;

//! Comparison operator for CISequences
inline bool operator==(const CISequence &s1, const CISequence &s2)
{
    if (s1.size() != s2.size())
        return false;
    FL::Patterns::CISequence::const_iterator i1, i2 = s2.begin();
    forall(i1, s1)
    {
        if (i1->id != i2->id || i1->index != i2->index)
            return false;
        ++i2;
    }
    return true;
}

//! Info on pattern checking
struct CheckInfo
{
    //! Set of applicable sequences
    std::vector<CISequence*> applicableSequences;
};

}} // namepsace

#endif // CHECKINFO_H
