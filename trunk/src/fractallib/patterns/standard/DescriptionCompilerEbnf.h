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

#ifndef DESCRIPTIONEBNF_H
#define DESCRIPTIONEBNF_H

#include "../DescriptionCompiler.h"

namespace FL { namespace Patterns { namespace Standard {

/*! \class DescriptionCompilerEbnf
  * \brief Compiler of descriptions that are writen in EBNF format
  *
  * Supported grammar:
  * \code
  * name         = alpha {alpha | digit};
  * indexed      = name "_" [uint];
  * ebnf         = name "=" expr;
  * expr         = term  {"|" term};
  * term         = factor {factor};
  * factor       = indexed | "(" expr ")" | "[" expr "]";
  * \endcode
  */
class DescriptionCompilerEbnf : public DescriptionCompiler
{
public:
    virtual FL::Exceptions::EParsing compile(
        Compilers::Input &input, Description *description);
};

}}} // namespaces

#endif // DESCRIPTIONEBNF_H
