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

#ifndef PATTERN_GUARD_H
#define PATTERN_GUARD_H

#include "PatternDescription.h"

namespace FL {
    namespace Patterns {

/*! \addtogroup FLAlgorithm
  */
/*@{*/

/*! \class GuardStructure
  * \brief Pattern structure abstract guard
  *
  * Real structure's data stored in children dependent on applicable algorithm.
  */
class GuardStructure
{
public:
    //! Destructor
    /*! Destroy nothing. Just to be polymorphic.
      */
    virtual ~GuardStructure() {}
};

/*! \class GuardCompiler
  * \brief Abstract compiler for guards
  */
class GuardCompiler
{
public:

    virtual bool compile(const std::string &text, GuardStructure &guard) = 0;

    int errorPos;
};

/*! \class GuardChecker
  * \brief Abstract checker for guard
  */
class GuardChecker
{
public:
    //! Constructor
    /*! Throught it good idea to pass structure to check in check(CheckContext &context)
      * method its passed to constructor only once to not make dynamic cast every time
      * check() is called.
      * So children HAVE to call setStructure() in constructor.
      * \param structure Structure that checker will check
      */
    GuardChecker(GuardStructure *structure)
    {
    }

    //! Set the structure to check
    /*! Inherited class have to check type of structure, cast it and remember it.
      */
    virtual void setStructure(GuardStructure * structure) = 0;

    //! Get the structure that is chacking in checker
    /*! Inherited class have to return cashed value.
      */
    virtual GuardStructure* structure() = 0;

    //! Virtual destructor
    virtual ~GuardChecker() {}

    //! Check if structure is staisfies guard's constraints
    /*! \param context current parsing context
      * \return true if structure can be extracted, false otherwise.
      */
    virtual bool check(CheckContext &context) = 0;
};

/*! \class Guard
  * \brief Describe guard constraints for given pattern's FL::Patterns::Description
  */
class Guard
{
private:
    GuardStructure *m_structure;
    GuardChecker *m_checker;
    GuardCompiler *m_compiler;
    Description &m_description;
    std::string m_sourceText;
public:
    //! Default constructor
    /*! \param description Description to guard
      */
    //! Default constructor
    /*! \param structure Structure of guard, child of FL::Patterns::GuardStructure class
      * \param checker Checker of guard, child of FL::Patterns::GuardChecker class
      * \param compiler Compiler of guard, child of FL::Patterns::GuardCompiler class
      */
    Guard(Description &description,
          GuardStructure *structure = NULL,
          GuardChecker *checker = NULL,
          GuardCompiler *compiler = NULL)
        : m_description(description)
    {
        m_structure = structure;
        m_checker = checker;
        m_compiler = compiler;
    }

    //! Destructor
    virtual ~Guard()
    {
        if (m_structure)
            delete m_structure;
        if (m_checker)
            delete m_checker;
        if (m_compiler)
            delete m_compiler;
    }

    //! Get guard's structure
    GuardStructure* structure() { return m_structure; }

    //! Get checker for this guard class
    GuardChecker* checker() { return m_checker; }

    //! Get compiler for this guard class
    GuardCompiler* compiler() { return m_compiler; }

    //! Get guarded FL::Patterns::Description
    Description& description() { return m_description; }

    //! Shortcut to checker()->check()
    /*! If checker not set, FL::EAnalyze will be thrown
      */
    bool check(CheckContext &context)
    {
        if (m_checker != NULL)
            return m_checker->check(context);
        else {
            GError(GCritical, "PatterGuard", 0, "Guard checker not set");
            return false;
        }
    }

    //! Get human-readable text of structure
    std::string sourceText() { return m_sourceText; }
    
    //! Set human-readable text of structure
    /*! \param value New source text
      * \param compile If true than new text will be compiled to this description (only if compiler set)
      * \return True if (compile is false) or (compile is true and compilation compilted successful)
      */
    bool setSourceText(const std::string &value, bool compile = false)
    {
        m_sourceText = value;
        if (compile && m_compiler != NULL)
            return m_compiler->compile(value, *m_structure);
        return true;
    }
};



/*@}*/

}} // namespace

#endif // PATTERN_GUARD_H
