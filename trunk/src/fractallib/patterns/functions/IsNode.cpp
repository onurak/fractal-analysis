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

#include "IsNode.h"
#include "../../trees/Tree.h"


using namespace FL::Patterns::Functions;

IsNode::IsNode()
{
    m_name = "IsNode";
}


const GVariant& IsNode::operator()(Patterns::Context& context, FunctionArgs& args)
{
    if (args.size() < 1)
        throw FL::Exceptions::EArguments(m_name, 1, -1, args.size());

    /*
    checkValidNode(args[0]);
    FL::Trees::Node *node = *args[0];

    if (node->id() != FL::IDGenerator::WILDCARD &&
        node->id() != context.currentItNode()->id())
    {
        return m_result = false;
    }

    if (node->index() != FL::IDGenerator::WILDCARD &&
        node->index() != context.currentItNode()->index())
    {
        return m_result = false;
    }

    return m_result = true;
    */

    FunctionArgs::const_iterator arg;
    forall(arg, args)
    {
        FL::Trees::Node *node = **arg;
        if (node == context.currentItNode())
            return m_result = true;
    }

    return m_result = false;
}

