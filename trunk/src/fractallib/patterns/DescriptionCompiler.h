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

#ifndef DESCRIPTIONCOMPILER_H
#define DESCRIPTIONCOMPILER_H

#include "Description.h"


namespace FL { namespace Patterns {

/*! \class DescriptionCompiler
  * \brief Abstract interface for description compilers
  */
class DescriptionCompiler
{
public:
    //! Compile string to description
    FL::Exceptions::EParsing compile(const std::string &s, Description *description)
    {
        if (description == NULL)
            return FL::Exceptions::EParsing(E_UNKNOWN_ARG);

        std::istringstream stream(s);
        Compilers::Input::Iterator it(stream);
        Compilers::Input input(it);
        return compile(input, description);
    }

    //! Compile stream to description
    virtual FL::Exceptions::EParsing compile(
        Compilers::Input &input, Description *description) = 0;
};

}}


#endif // DESCRIPTIONCOMPILER_H
