/* Author: Vladimir Belikov
 *
 * This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Fractal Libray.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PATTERN_H
#define PATTERN_H

#include <string>
#include "G/GCollection.h"
#include "G/GVariant.h"
#include "SynonymsTable.h"
#include "PatternDescription.h"
#include "PatternGuard.h"
#include "PatternParameter.h"

/*! \namespace FL
  * \brief Main Fractal Library namespace. Access to all its resources
  */

namespace FL {

/*! \addtogroup FLAlgorithm
  */
/*@{*/


/*! \class Pattern
  * \brief Fractal pattern
  *
  * %Pattern structure:
  * \image html pattern_structure.png
  *
  * It mean pattern consists of two main parts: Description (symbolic structure)
  * and Guard (constraint function).
  * \section Pattern Description
  * Description describe symbolic structure of pattern - it's some structure that we want
  * to find in time series structure. Different parsers can work with concreet
  * types of descriptions.
  * Physically it is FL::Patterns::Description class. It contains three elements:
  * - child of FL::Patterns::DescriptionStructure class, which holds real structure
  * - child of FL::Patterns::DescriptionChecker class, which must check if
  *   pattern is applicable to given parse tree layer
  * - child of FL::Patterns::DescriptionCompiler class, which must compile
  *   expressions in some human-readable form to FL::Patterns::DescriptionStructure
  *
  * \section Pattern Guard
  * TBD
  */
class Pattern
{
public:
    //! Result of structure check
    enum CheckResult
    {
        crOK,                 //!< Pattern applicable
        crInvalidDescription, //!< Pattern not applicable cause of FL::Patterns::Description
        crInvalidGuard        //!< Pattern not applicable cause of FL::Patterns::Guard
    };
private:

public:
    //! Constructor
    Pattern(SynonymsTable &synonyms, Patterns::ParameterSet &parameters);

    //! Destructor
    ~Pattern()
    {
        delete m_guard;
        delete m_description;
    }

    //! Getter for pattern's name
    std::string name() { return m_description->name(); }

    //! Set new name to pattern
    void setName(std::string value) { m_description->setName(value); }

    //! Convert to string
    operator std::string();

    //! Guard getter
    Patterns::Guard* guard() { return m_guard; }

    //! structure getter
    Patterns::Description* description() { return m_description; }

    //! Check if description and guard are applicable in context
    /*! \param context checking context of parsing
      * \return crOK if both description and guard are applicable,
      * crInvalidDescription in case of unapplicable description,
      * crInvalidGuard in case of unapplicable guard.
      */
    CheckResult check(Patterns::CheckContext &context);
private:
    //! Guard of pattern
    Patterns::Guard *m_guard;

    //! Description of pattern
    Patterns::Description *m_description;

    //! Synonyms table
    SynonymsTable &m_synonyms;

    //! Parameters set
    Patterns::ParameterSet &m_params;
};

/*! \class PatternCollection
  * \brief Collection of patterns
  */
class PatternCollection: public std::vector<Pattern*> {
public:
    typedef PatternCollection::iterator Iterator;
    typedef PatternCollection::const_iterator ConstIterator;
public:
    //! Synonyms table
    SynonymsTable synonyms;

    //! Parameters set
    Patterns::ParameterSet parameters;

};

/*@}*/

} // namespace

#endif // PATTERN_H
