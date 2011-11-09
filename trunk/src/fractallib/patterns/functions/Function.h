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

#ifndef FUNCTION_H
#define FUNCTION_H

#include <string>
#include <vector>
#include "../../../G/GVariant.h"
#include "../../Common.h"
#include "../../exceptions/EException.h"
#include "../Context.h"

namespace FL { namespace Patterns { namespace Functions {

/*! \struct IndexedName
  * \brief Name and it's index
  */
struct IndexedName
{
    IndexedName(int nameId = -1, int no = -2) : nameId(nameId), no(no) {}
    int nameId;
    int no;
};

/*! \enum Error
  * \brief Function's error
  */
enum Error
{
    OK = 0,
    INVALID_ARGS,
    INVALID_NODE
};

//! Get description of error code
const char* errorDescription(Error err);


//! Function's arguments
typedef std::vector<GVariant*> FunctionArgs;

/*! \class Function
  * \brief Function prototype.
  */
class Function
{
public:
    //! Main function of class. Children must override it
    virtual const GVariant& operator()(Patterns::Context&, FunctionArgs&) = 0;

    //! Another way to execute predicate (same as operator())
    inline const GVariant& call(Patterns::Context& context, FunctionArgs& args)
    {
        return this->operator ()(context, args);
    }

    //! Getter of predicate's name
    const std::string& name() const { return m_name; }
protected:
    //! Define optional argument of function.
    /*! \param argName Name of argument that makes it enabled. It automaically
      *                add argument with name "no<argName>" that makes it disabled
      * \param defaultValue default value of argument used if it is not specified
      */
    void defineArg(const std::string &argName, GVariant defaultValue);

    //! Parse optional arguments defined by defineArg method
    void parseArgs(FunctionArgs& args);

    //! Check that argument is the valid FL::Trees::Node*
    inline void checkValidNode(GVariant *arg)
    {
        if (arg == NULL || (FL::Trees::Node *)(*arg) == NULL)
            throw FL::Exceptions::EArguments(m_name, "Tree node", "NULL");
    }

protected:
    //! Predicate name. Every child must set it the in constructor
    std::string m_name;

    //! Internal storage for last result
    GVariant m_result;
};


}}} // namespaces

#endif // FUNCTION_H
