#ifndef CHECKINFO_H
#define CHECKINFO_H

#include "../Common.h"

namespace FL {

/*! \class ParseResult
  * \brief Result of layer/tree parsing
  */
class ParseResult
{
public:
    ParseResult()
    {
        reset();
    }

    inline void reset()
    {
        treesAdded = treesModified = nodesAdded = 0;
    }

    int treesAdded;
    int treesModified;
    int nodesAdded;

    void operator += (const ParseResult &r)
    {
        treesAdded += r.treesAdded;
        treesModified += r.treesModified;
        nodesAdded += r.nodesAdded;
    }

    bool somethingAdded() const
    {
        return treesAdded > 0 || treesModified > 0 || nodesAdded > 0;
    }
};


//! Options available for patterns checker
enum CheckOptions
{
    coNone               = 0x0,   //!< No options
    coAllowPartialMatch  = 0x1,   //!< Allow partial match
    coContinueChecking   = 0x2,   //!< Check if cnadidate possible node is still possible
    coNoDescriptionCheck = 0x4    //!< Do not check description, only guard
};

//! Result of pattern checking
enum CheckResult
{
    crOK,                  //!< Something matched (exact or partial)
    crExactMatch,          //!< Exact match
    crPartialMatch,        //!< Begining of sequence matched, end is unavailable
    crInvalidDescription,  //!< Invalid description part
    crInvalidGuard,        //!< Invalid guard part
};



} // namespace



namespace FL { namespace Patterns {

//! Node for FL::Patterns::CheckInfo
struct CINode
{
    int id;
    int index;

    std::string toStr() const
    {
        char s[100];
        if (index >= 0)
            sprintf(s, "%s_%d", IDGenerator::nameOf(id).c_str(), index);
        else
            sprintf(s, "%s_*", IDGenerator::nameOf(id).c_str());
        return s;
    }
};

struct CINodeLessCompare
{
    bool operator()(const CINode &node1, const CINode &node2)
    {
        if (node1.index < node2.index)
            return true;
        if (node1.id < node2.id)
            return true;
         return false;
    }
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

//! Info on single founded sequence
struct CISequenceInfo
{
    //! Sequence itselves (you must not delete it manually!)
    CISequence* sequence;

    //! Exact or partial match
    CheckResult checkResult;

    //! First matchedCound symbols of sequence are matched
    int matchedCount;
};

//! Info on pattern checking
struct CheckInfo
{
    //! Set of applicable sequences
    std::vector<CISequenceInfo> applicableSequences;
};

}} // namepsace

#endif // CHECKINFO_H
