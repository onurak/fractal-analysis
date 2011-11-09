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
