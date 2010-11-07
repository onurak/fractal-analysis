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

#include "Pattern.h"
#include "PatternDescriptionEbnf.h"
#include "PatternGuardRPN.h"
#include "GuardCompilerRPN.h"

using namespace FL;

/*************** PATTERNS IMPLEMENTATION ***************/

Pattern::Pattern(SynonymsTable &synonyms, Patterns::ParameterSet &parameters)
    : m_synonyms(synonyms), m_params(parameters)
{
    // create description
    FL::Patterns::DescriptionStructure * ds = new Patterns::EBNF::EbnfStructure();
    m_description = new Patterns::Description(
            ds,
            new Patterns::EBNF::EbnfChecker(ds),
            new Patterns::EBNF::EbnfCompiler());

    // create guard
    FL::Patterns::RPN::GuardStructureRPN *gs = new Patterns::RPN::GuardStructureRPN();
    m_guard = new Patterns::Guard(
            *m_description,
            gs,
            new Patterns::RPN::GuardCheckerRPN(gs),
            new Patterns::RPN::GuardCompilerRPN()
            );
}


Pattern::operator std::string()
{
    return m_description->sourceText() + " @ " + m_guard->sourceText() + ";";
}

Pattern::CheckResult Pattern::check(Patterns::CheckContext &context)
{
    // Check description
    logg.debug("    Pattern: ").log(name());
    logg.debug("    Checking description... ");
    if ( !m_description->check(context) )
    {
        logg << "failure";
        return crInvalidDescription;
    }
    logg << "successful";

    // Check guard
    logg.debug("    Checking guard... ");
    if ( !m_guard->check(context) )
    {
        logg << "failure";
        return crInvalidGuard;
    }
    logg << "successful";

    return crOK;
}
