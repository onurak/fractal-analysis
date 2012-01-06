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

#ifndef GUARDRPN_H
#define GUARDRPN_H

#include "../Guard.h"

namespace FL { namespace Patterns { namespace Standard {

namespace Internal
{
    class Program;
}

class GuardRpn : public Guard
{
public:
    typedef std::map<CINode, Internal::Program*> GuardSet;
public:
    GuardRpn(const FL::Patterns::Description &description);
    virtual ~GuardRpn();

    virtual FL::Exceptions::EParsing compile(Compilers::Input &i);
    virtual bool check(Context &c);
private:
    //! Find guards suitable for node.
    /*! List of guard programs will be returned into
      * m_nodeGuards variable.
      *
      * Search order:
      * \li Exact match (match both id and index).
      * \li Match by id with index wildcard.
      * \li Match by index with id wildcard.
      * \li Match by both id and index wildcard.
      *
      * \returns true if some guards are found, false otherwise.
      */
    bool getGuardsForNode(FL::Trees::Node *node);

    std::vector<Internal::Program*> m_nodeGuards;

    GuardSet m_rpnPrograms;
};

}}} // namespaces

#endif // GUARDRPN_H
