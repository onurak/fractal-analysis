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

#ifndef PREVIOUS_H
#define PREVIOUS_H

#include "../Predicates.h"

namespace FL { namespace Predicates {

/*! \class Previous
  * \brief Check if previous to current pattern is the passed in parameter
  *
  * Arguments:
  *  - args[0] -- previous element's name. Can be "NONE" - than result will be true only if
  *  there is no previous elements. Can be "*" - than result will be true only if there is any
  *  previous elements.
  *  - args[1] -- (optional) "exact" if don't look for synonyms
  *  - args[2] -- (optional) "nospace" if no spaces allowed between this two patterns
  */
class Previous : public Predicate
{
public:
    //! Default constructor
    Previous()
    {
        m_name = "Previous";
    }

    //! Call operator
    virtual const GVariant& operator()(Patterns::CheckContext& context, PredicateArgs& args)
    {
        if (args.size() != 1 && args.size() != 2 && args.size() != 3)
        {
            GError(GCritical, m_name, 0, EInvalidTermArgs);
            return m_result = false;
        }

        const std::string & wantPrevName = *args[0];
        // If no patterns were recognized
        if (context.modification.size() == 0)
        {
            if (wantPrevName == "*")
                return m_result = false;
            if (wantPrevName == "NONE")
                return m_result = true;
        }
        ParseTreeNode *prev = context.modification.back();

        // Parse arguments
        bool exact = false;
        bool nospace = true;
        if (args.size() > 1)
            exact = std::string("exact").compare((char*)(*args[1])) == 0;
        if (args.size() > 2)
            nospace = std::string("nospace").compare((char*)(*args[2])) == 0;

        // If "nospace" set than check that patterns cover adjacent segment
        if (nospace && prev->tsEnd != context.lastParsed.front()->tsBegin - 1)
            return m_result = false;

        // Is "exact" set than check exact name of previous pattern, otherwise it can be any synonym
        if (!exact)
            return m_result = context.synonyms->isSynonyms(prev->name, wantPrevName) ? true : false;
        else
            return m_result = prev->name == wantPrevName ? true : false;
    }
};

}} // namespaces

#endif // PREVIOUS_H
