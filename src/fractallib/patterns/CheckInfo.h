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

//! CINodes partial ordering operator (to enable them to be used as std::map elements)
inline bool operator<(const CINode &n1, const CINode &n2)
{
    return n1.id < n2.id && n1.index < n2.index;
}


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

class Pattern;

//! Info on pattern checking
struct CheckInfo
{
    //! Describes single sequence and its owner
    struct ApplicableSeq
    {
        ApplicableSeq(CISequence* seq, Pattern *pattern)
            : seq(seq), pattern(pattern) {}

        CISequence* seq;
        Pattern *pattern;
    };

    //! Sequences of symbol, applicable at the point of layer
    std::vector<ApplicableSeq> applicableSequences;
};

}} // namepsace

#endif // CHECKINFO_H
