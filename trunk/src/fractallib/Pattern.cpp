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
#include "UniqueNamer.h"

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

    // Fill 'Self' node
    createCandidateNode(context);

    // Check guard
    logg.debug("    Checking guard... ");
    if ( !m_guard->check(context) )
    {
        rollbackCandidateNode(context);
        logg << "failure";
        return crInvalidGuard;
    }
    logg << "successful";
    commitCandidateNode(context);

    return crOK;
}

void Pattern::createCandidateNode(Patterns::CheckContext &context)
{
    // create new node
    ParseTreeNode *newNode = new ParseTreeNode(
        name(),                           // name of node = name of applied pattern
        NULL,                             // no parent
        -1,                               // level will be set later
        (*context.iRoot)->tsBegin,        // begin of time series segment
        (*context.lastNode)->tsEnd);      // end of time series segment

    logg.debug("Pattern position: ") << (*(context.iRoot))->tsBegin << "-"
            << (*context.lastNode)->tsEnd;
    int maxLevel = -1;

    // set for every node in pattern parent to just created node,
    // looking for maximal level among them
    ParseTree::Layer::const_iterator node;
    for (node = context.iRoot; node != context.lastNode + 1; ++node)
    {
        newNode->children.push_back(*node);
        if ((*node)->level > maxLevel)
            maxLevel = (*node)->level;
    }
    newNode->level = maxLevel + 1; // set new node's level to max+1 (it will be new maximum)

    context.candidateNode = newNode;
}

void Pattern::commitCandidateNode(Patterns::CheckContext &context)
{
    ParseTree::Layer::const_iterator node;
    for (node = context.iRoot; node != context.lastNode+1 && node != context.roots->end(); ++node)
        context.tree->setNodeParent(*node, context.candidateNode);
    context.modification.push_back(context.candidateNode);
    context.tree->addNode(context.candidateNode);
}

void Pattern::rollbackCandidateNode(Patterns::CheckContext &context)
{
    if (context.candidateNode)
    {
        delete context.candidateNode;
        context.candidateNode = NULL;
    }
}

int PatternCollection::idOfPattern(const std::string & name) const
{
    return UniqueNamer::id_of_name(name);
}

const std::string & PatternCollection::patternNameOfID(int id) const
{
    return UniqueNamer::name_of_id(id);
}
