#ifndef PATTERN_H
#define PATTERN_H

#include "Guard.h"
#include "Description.h"
#include "../Common.h"

namespace FL { namespace Patterns {

//! Result of pattern checking
enum CheckResult
{
    crOK,
    crInvalidDescription,
    crInvalidGuard
};

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

    const std::string& name() const;
    void setName(const std::string& name);

    int id() const;

    Guard* guard() const;
    Description* description() const;

    Exceptions::EParsing compile(const std::string &s);
    Exceptions::EParsing compile(Compilers::Input *input);
    CheckResult check(Context &c, CheckInfo &info);
private:
    Pattern(const Pattern &p);
private:
    Guard *m_guard;
    Description *m_description;
};

class PatternsSet : public std::vector<Pattern*>
{
public:
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
public:
    virtual ~PatternsSet() { cleanup(); }
    void cleanup() { FL::cleanup(*this); }
};

} } // namepsace

#endif // PATTERN_H
