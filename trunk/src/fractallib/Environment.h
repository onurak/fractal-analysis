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
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

/*! \file
  * \brief Содержит класс Environment, который позволяет указывать параметры алгоритма
  */

#define BOOST_DISABLE_ASSERTS

#include <string>
#include <map>
#include "Exceptions.h"
#include "G/GVariant.h"
#include "tools/spirit.h"

namespace FL {

/*! \addtogroup FLUtils
  */
/*@{*/

class Environment;

/*! \class EnvironmentSection
  * \brief Single FL::Environment section with data
  */
class EnvironmentSection: public std::map<std::string, GVariant>
{
private:
    std::string m_name;
public:
    //! Default constructor
    EnvironmentSection(std::string name = ""): std::map<std::string, GVariant>()
    {
        m_name = name;
    }

    //! Get name of section
    const std::string& name() const
    {
        return m_name;
    }

    //! Set name of section
    void setName(const std::string &newName)
    {
        m_name = newName;
    }

    //! Check if field exists
    bool exists(const std::string &name)
    {
        return this->find(name) == end();
    }

    //! Insure that params exists. If they are not than default value will be assigned to them
    void insureExists(const std::string &name, GVariant defaultValue)
    {
        if (this->find(name) == end())
            this->insert(std::make_pair(name, defaultValue));
    }

    //! Get value of field if it's exists, or defaultValue if it's not
    /*! \param name Field name
      * \param defaultValue Default value returned in case of field not exists
      * \return value of field if it's exists, or defaultValue if it's not
      */
    GVariant valueOf(const std::string &name, GVariant defaultValue) const
    {
        std::map<std::string, GVariant>::const_iterator p = find(name);
        return p != end() ? p->second : defaultValue;
    }

    //! Add records from section with same name of another environment
    /*! \param env Environment to load
      * \param reassingExisting If it's true (default) than all existing values will be rewriten form env
      */
    void appendFrom(Environment &env, bool reassingExisting = true);

    GVariant& operator[](const std::string& key)
    {
        insureExists(key, "0");
        GVariant& result =
                std::map<std::string, GVariant>::operator[](key);
        return result;
    }
};

/*! \class Environment
  * \brief Set of grouped settings FractalLib
  *
  * Contains data of any type supported by GVariant used by FractalLib.
  * Settings file grammar:
  * \code
  * file    = (section record*)*
  * section = '[' value ']'
  * record  = value '=' value eol
  * value   = symbol* | '"' (any_char - '"')* '"'
  * symbol  = 'a'..'z' | 'A'..'Z' | '0'..'9' | ' ' | '\t'
  * eol     = '\n'
  * \endcode
  */
class Environment: public std::map<std::string, EnvironmentSection>
{
public:
    //! Default constructor
    /*! \param fileName File name to load
      * \throws EEnvironment
      * \sa loadFromFile, addFromFile
      */
    Environment(const std::string &fileName = "")
    {
        if (!fileName.empty())
            loadFromFile(fileName);
    }

    //! Load values from file and clear all old values
    /*! \param fileName File name to load
      * \return true in case of success, false otherwise
      * \sa addFromFile
      */
    bool loadFromFile(const std::string &fileName)
    {
        clear();
        return addFromFile(fileName);
    }

    //! Load values from file but don't clear old values
    /*! \param fileName File name to load
      * \return true in case of success, false otherwise
      * \sa loadFromFile
      */
    bool addFromFile(const std::string &fileName);

    //! Add records from another environment
    /*! \param env Environment to load
      * \param reassingExisting If it's true (default) than all existing values will be rewriten form env
      */

    void appendFrom(Environment &env, bool reassingExisting = true)
    {
        std::map<std::string, EnvironmentSection>::const_iterator section_it;
        std::map<std::string, GVariant>::const_iterator v;
        for_each_(section_it, env)
        {
            EnvironmentSection &section = (*this)[section_it->first];
            for_each_(v, section)
            {
                std::string fieldName = v->first;
                if (!reassingExisting && section.find(fieldName) != section.end())
                    continue;
                section[fieldName] = v->second;
            }
        }
    }

    //! Check if section currently exists
    /*! \param name Name of section to check
      * \return true if section exists, false otherwise
      */
    bool sectionExists(const std::string &name) const
    {
        return this->find(name) != this->end();
    }

    EnvironmentSection& operator[](const std::string& key)
    {
        bool needRename = !sectionExists(key);
        EnvironmentSection& result =
                std::map<std::string, EnvironmentSection>::operator[](key);
        if (needRename)
            result.setName(key);
        return result;
    }
};

/*@}*/

} // namespace

#endif // ENVIRONMENT_H
