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

#ifndef DESCRIPTION_H
#define DESCRIPTION_H

#include <string>
#include "../exceptions/EException.h"
#include "../compilers/AbstractCompiler.h"
#include "CheckInfo.h"

namespace FL { namespace Patterns {

/*! \class Description
  * \brief Descriptional (structural) part of Pattern
  */
class Description
{
public:
    Description(const std::string &name = "")
    {
        setName(name);
    }

    //! Get name of description (same as name of pattern)
    inline const std::string& name() const
    {
        return IDGenerator::nameOf(m_id);
    }

    //! Set name of description (same as name of pattern)
    inline void setName(const std::string& newName)
    {
        if (!newName.empty())
            m_id = IDGenerator::idOf(newName);
        else
            m_id = IDGenerator::NONE;
    }

    //! Get id of name of description (same as id of pattern)
    inline int id() const
    {
        return m_id;
    }

    //! Sequences of symbols that satisfing description
    inline CISet& sequences()
    {
        return m_sequences;
    }
private:
    int m_id;

    CISet m_sequences;
};


//class Context;

///*! \class Description
//  * \brief Description (structure) part of Pattern
//  */
//class Description
//{
//public:
//    //! Standart constructor
//    Description();

//    //! Get name of description (same as name of pattern)
//    const std::string& name() const;

//    //! Set name of description (same as name of pattern)
//    void setName(const std::string& newName);

//    //! Get id of name of description (same as id of pattern)
//    int id() const;

//    //! Compile string to description
//    FL::Exceptions::EParsing compile(const std::string &s);

//    //! Compile stream to description
//    virtual FL::Exceptions::EParsing compile(Compilers::Input &i) = 0;

//    //! Check if some sequence of description is applicable in context
//    virtual bool check(Context &c, CheckInfo &info) = 0;

//    //! Get set of sequences in CI-form
//    virtual CISet& sequences() = 0;
//private:
//    int m_id;
//};

}} // namespaces

#endif // DESCRIPTION_H
