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

#ifndef PREV_H
#define PREV_H

#include "../Predicates.h"

namespace FL { namespace Predicates {

/*! \class Prev
  * \brief Extract structure element of previously parsed pattern or pattern itself
  *
  * Usage exmaple:
  * \code
  *   p1 = a_1 a_2 @ true;
  *   p2 = a b a @ (min( prev(p1, a, 2) ) > 0)  and  ( value(prev()) > 10 );
  * \endcode
  * In case of 3 or 4 arguments it must be previouse pattern's name, its structure
  * element's name and element's no (and optionaly "nospace" if it must be no
  * space between patterns). In case of 0 or 1 argument
  * (optional argument is "nospace") function will return last parsed pattern.
  *
  * If previous pattern is different from specified in arguments or it don't
  * have specified structure element or there are space between patterns
  * (if "nospace" specified) than function will throw an error.
  * Arguments:
  *  - args[0] -- (optional) "nospace" if no spaces allowed between this two patterns
  *  or
  *  - args[0] -- previous pattern name
  *  - args[1] -- previouse pattern's structure element name
  *  - args[2] -- previouse pattern's structure element no
  *  - args[3] -- (optional) "nospace" if no spaces allowed between this two patterns
  */
class Prev : public Predicate
{
private:
    int m_no_nospace;
public:
    //! Default constructor
    Prev()
    {
        m_name = "Prev";
        m_no_nospace = UniqueNamer::id_of_name("nospace");
    }

    //! Call operator
    virtual const GVariant& operator()(Patterns::CheckContext& context, PredicateArgs& args)
            throw (EPredicate)
    {
        int argSize = args.size();
        if (argSize != 0 && argSize != 1 && argSize != 3 && argSize != 4)
            throw EPredicate(m_name, INVALID_ARGS);

        // If no patterns were recognized
        if (context.modification.size() == 0)
            throw EPredicate(m_name, INVALID_NODE);

        // Get previous recognized pattern
        ParseTreeNode *prev = context.modification.back();

        if (argSize > 1 && prev->nameId != (int)*args[0])
            throw EPredicate(m_name, INVALID_NODE);

        // nospace
        bool nospace = true;
        if (argSize == 1)
            nospace = m_no_nospace == (int)*args[0];
        else if (argSize == 4)
            nospace = m_no_nospace == (int)*args[3];

        // If "nospace" set than check that patterns cover adjacent segment
        if (nospace && prev->tsEnd != context.lastParsed.front()->tsBegin - 1)
            throw EPredicate(m_name, INVALID_NODE);

        if (argSize > 1)
            // Get first node that have specific parameters
            return m_result = context.getNode(*args[1], *args[2], prev->children);
        else
            return m_result = prev;
    }
};


} } // namespaces

#endif // PREV_H
