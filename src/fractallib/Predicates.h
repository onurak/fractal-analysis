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

#ifndef PREDICATES_H
#define PREDICATES_H

#include <math.h>
#include "PatternDescription.h"
#include "Exceptions.h"
#include "G/GVariant.h"

namespace FL {

/*! \namespace FL::Predicates
  * \brief Contains predicates available for patterns
  */
namespace Predicates {

/*! \addtogroup FLAlgorithm
  */
/*@{*/

/*! \struct IndexedName
  * \brief Name and it's index
  */
struct IndexedName
{
    IndexedName(int nameId = -1, int no = -2) : nameId(nameId), no(no) {}
    IndexedName(const std::string &name = "", int no = -2) : no(no)
    {
        nameId = UniqueNamer::id_of_name(name);
    }
    int nameId;
    int no;
};

/*! \enum Error
  * \brief Predicate errors
  */
enum Error
{
    OK = 0,
    INVALID_ARGS,
    INVALID_NODE
};

//! Get description of error code
const char* errorDescription(Error err);

/*! \class EPredicate
  * \brief Exception that can be thrown by Predicate
  */
class EPredicate: public GException
{
public:
    EPredicate(const std::string &predicateName, Predicates::Error errNo)
    {
        m_errNo = (int)errNo;
        const char *descr = errorDescription(errNo);
        if (descr)
            m_msg = predicateName + ": " + descr;
        else
            m_msg = predicateName + ": " + "Unknown error";
        report();
    }

    FL::Predicates::Error error() const { return (FL::Predicates::Error) m_errNo; }
};

//! Predicate's arguments
typedef std::vector<GVariant*> PredicateArgs;

class EPredicate;

/*! \class Predicate
  * \brief Predicate prototype.
  */
class Predicate
{
public:
    //! Main function of predicate. Children must override it
    virtual const GVariant& operator()(
            Patterns::CheckContext&, PredicateArgs&) throw (EPredicate) = 0;

    //! Another way to execute predicate (same as operator())
    inline const GVariant& call(Patterns::CheckContext& context, PredicateArgs& args)
        throw (EPredicate)
    {
        return this->operator ()(context, args);
    }

    //! Getter of predicate's name
    const std::string& name() const { return m_name; }
protected:
    //! Predicate name. Every child must set it in constructor
    std::string m_name;

    //! Internal result storage
    GVariant m_result;
};


/*@}*/

} // namespace
} // namespace

#endif // PREDICATES_H
