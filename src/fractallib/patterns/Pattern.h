/** This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Fractal Library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PATTERN_H
#define PATTERN_H

#include "Guard.h"
#include "Description.h"
#include "../Common.h"
#include "../Forecast.h"

namespace FL { namespace Patterns {

//enum CheckResult
//{
//    crOK,
//    crInvalidDescription,
//    crInvalidGuard
//};


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
    virtual ~Pattern();

    //! Get pattern name
    const std::string& name() const;

    //! Set patterns name. ID would be set automatically
    void setName(const std::string& name);

    //! Get id of name of pattern
    int id() const;

    Guard* guard() const;
    Description* description() const;

    //! Compile pattern expression
    Exceptions::EParsing compile(const std::string &s);

    //! Compile pattern expression
    Exceptions::EParsing compile(Compilers::Input *input);

    //! Check if pattern is applicable in analysis context
    //CheckResult check(Context &c, CheckInfo &info);

    //! Get time series values range where guard still executable for this node
    /*! \param node Node of interest
      * \param tree Tree where node is defined
      * \param ts Time series where tree is defined
      * \param forecast Returned forecast data
      * \return true if limits calculated, false if some error occures (invalid
      *  parameters or guard never can't be applied to this node)
      */
    bool calcGuardLimits(FL::Trees::Node *node,
                         FL::Trees::Tree &tree,
                         FL::TimeSeries &ts,
                         FL::ForecastItem &forecast);

    //! Get max length of description sequences
    int maxLength() const;
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

    //! Check for left recursion in patterns set
    /*! Currently it performs only situations like this:
      *    A = A ...
      * and can't find indirect recusion like this:
      *    A = B
      *    B = A
      */
    bool haveLeftRecursion() const;

    //! Get max length of patterns sequences
    int maxLength() const;
};

/*! @class PatternsRegistry
  * @brief Registry of patterns which can return any pattern registered in current
  * session by its id.
  */
class PatternsRegistry
{
public:
    typedef std::map< int, std::vector<Pattern*> > Storage;

public:
    //! Get registered pattern by its id.
    /** If more then one pattern with such id registered then
      * only one random pattern will be returned.
      */
    static inline Pattern* patternById(int id)
    {
        Storage::const_iterator i = m_registry.find(id);
        if (i == m_registry.end() || i->second.size() == 0)
            return NULL;
        return i->second[0];
    }

    //! Get pattern by its name
    static Pattern* patternByName(const std::string &name)
    {
        return patternById(IDGenerator::idOf(name));
    }

    //! Register pattern (must be called in Pattern constructor)
    static void registerPattern(int id, Pattern *pattern)
    {
        Storage::iterator i = m_registry.find(id);
        if (i == m_registry.end())
            m_registry[id].push_back(pattern);
        else
            i->second.push_back(pattern);
    }

    //! Unregister pattern (must be called in Pattern destructor)
    static void unregisterPattern(int id, Pattern *pattern)
    {
        Storage::iterator i = m_registry.find(id);
        if (i != m_registry.end())
        {
            std::vector<Pattern*>::iterator j;
            if (search(i->second, pattern, j))
                i->second.erase(j);
        }
    }

private:
    static Storage m_registry;
};

} } // namepsace

#endif // PATTERN_H
