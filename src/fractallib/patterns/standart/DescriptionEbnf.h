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

#include "../Description.h"

namespace FL { namespace Patterns { namespace Standart {

class DescriptionEbnf : public Description
{
public:
    DescriptionEbnf();

    virtual FL::Exceptions::EParsing compile(Compilers::Input &input);
    virtual bool check(Context &c, CheckInfo &info, CheckOptions check = coNone);

    //! Get set of sequences in CI-form
    virtual CISet& sequences() { return m_ebnfSet; }
protected:
    //! Collection of all sequences possible for this description
    CISet m_ebnfSet;
};

}}} // namespaces

#endif // DESCRIPTIONEBNF_H
