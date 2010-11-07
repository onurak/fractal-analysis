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

#ifndef EXTRACTPREV_H
#define EXTRACTPREV_H

#include "../Predicates.h"

namespace FL { namespace Predicates {

/*! \class ExtractPrev
  * \brief Extract structure element previous parsed pattern
  *
  * Usage exmaple:
  * \code
  *   p1 = a_1 a_2 @ true;
  *   p2 = a b a @ min( extractprev(p1, a_2) ) > 0;
  * \endcode
  * If previous pattern is different from specified in arguments or it don't
  * have specified structure element than function return 0.
  * Arguments:
  *  - args[0] -- previous pattern name
  *  - args[1] -- previouse pattern's structure element name
  *  - args[2] -- previouse pattern's structure element no
  *  - args[3] -- (optional) "nospace" if no spaces allowed between this two patterns
  */
class ExtractPrev : public Predicate
{
public:
    //! Default constructor
    ExtractPrev()
    {
        m_name = "ExtractPrev";
    }

    //! Call operator
    virtual const GVariant& operator()(Patterns::CheckContext& context, PredicateArgs& args)
    {
        if (args.size() < 3 || args.size() > 4)
        {
            GError(GCritical, m_name, 0, EInvalidTermArgs);
            return m_result = NULL;
        }

        // If no patterns were recognized
        if (context.modification.size() == 0)
            return m_result = NULL;

        // Get previous recognized pattern
        ParseTreeNode *prev = context.modification.back();
        if (prev->name.compare((char*)(*args[0])) != 0)
            return m_result = NULL;

        // exact/nospace
        bool nospace = true;
        if (args.size() == 4)
            nospace = std::string("nospace").compare((char*)(*args[3])) == 0;

        // If "nospace" set than check that patterns cover adjacent segment
        if (nospace && prev->tsEnd != context.lastParsed.front()->tsBegin - 1)
            return m_result = NULL;

        // Get first node that have specific parameters
        return m_result = context.getNode(*args[1], *args[2], prev->children);
    }
};


} } // namespaces

#endif // EXTRACTPREV_H
