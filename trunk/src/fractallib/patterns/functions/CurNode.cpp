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

#include "CurNode.h"

using namespace FL::Patterns::Functions;

CurNode::CurNode()
{
    m_name = "Node";
}


const GVariant& CurNode::operator()(Patterns::Context& context, FunctionArgs& args)
{
    if (args.size() > 0)
        throw FL::Exceptions::EArguments(m_name, 0, args.size());
    return m_result = context.currentItNode();
}

