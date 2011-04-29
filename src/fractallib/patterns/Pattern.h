#ifndef PATTERN_H
#define PATTERN_H

#include "Guard.h"
#include "Description.h"
#include "../Common.h"

namespace FL { namespace Patterns {

//! Pattern's data factory
class PatternConstructor
{
public:
    PatternConstructor() {}
    virtual Guard *createGuard(Description &d) const = 0;
    virtual Description *createDescription() const = 0;
};

class Context;

/*! \class Pattern
  * \brief Hold all pattern's data and methods for compilation/checking
  */
class Pattern
{
public:
    Pattern(const PatternConstructor &pc);
    Pattern(const std::string &name, const PatternConstructor &pc);
    Pattern(const PatternConstructor *pc, bool autoDeleteConstructor = true);
    Pattern(const std::string &name, const PatternConstructor *pc, bool autoDeleteConstructor = true);

    //! Get pattern name
    const std::string& name() const;

    //! Set patterns name. ID would be set automatically
    void setName(const std::string& name);

    //! Get id of name of pattern
    int id() const;

    //! Get maximal length of pattern's symbols sequence
    int maxSequenceLength() const;

    Guard* guard() const;
    Description* description() const;

    //! Compile pattern expression
    Exceptions::EParsing compile(const std::string &s);

    //! Compile pattern expression
    Exceptions::EParsing compile(Compilers::Input *input);

    //! Check if pattern is applicable in analysis context
    CheckResult check(Context &c, CheckInfo &info, CheckOptions check = coNone);
private:
    Pattern(const Pattern &p);
private:
    Guard *m_guard;
    Description *m_description;
};

/*! \class PatternsSet
  * \class Standart collection of patterns
  */
class PatternsSet : public std::vector<Pattern*>
{
public:
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
public:
    virtual ~PatternsSet() { cleanup(); }

    //! Clear and delete all patterns
    void cleanup() { FL::cleanup(*this); }

    //! Get max size of sequence of symbols in pattern set
    int maxSequenceLength() const;
};

} } // namepsace

#endif // PATTERN_H
