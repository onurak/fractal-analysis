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

#include "PatternGuardRPN.h"
#include "Exceptions.h"

using namespace FL;
using namespace FL::Patterns::RPN;

/****** Execution of RPN ********/

char errorCodes[][50] =
{
    "No error",
    "Empty program",
    "Invalid program (execution stack error)",
    "Invalid operand reference",
    "Invalid predicate reference",
    "Invalid goto reference",
    "Invalid operation",
    "Invalid instruction",
    "Division by zero",
    "Invalid predicate arguments"
};

#define checkStack(sz) \
    if (st.size() < sz)  \
        return error(INVALID_PROGRAM);

GVariant GuardCheckerRPN::execute(CheckContext &context)
{
    if (m_structure->code.size() == 0)
        return error(EMPTY_PROGRAM);
    m_structure->code.begin();
    st.clear();
    while (!m_structure->code.last())
    {
        Instruction &i = m_structure->code.next();
        switch (i.type)
        {
            // VARIABLE
            case OPERAND:
              {
                if (i.concr < 0 || i.concr >= int(m_structure->vars.size()))
                    return error(INVALID_OPERAND_REFERENCE);
                st.push(&(m_structure->vars[i.concr]));
                break;
              }

            // OPERATION
            case OPERATION:
              {
                GVariant *v1, *v2;
                switch (i.concr)
                {
                    /*** Math operations ***/
                    case MATH_LESS:
                        checkStack(2);
                        v1 = st.pop(); v2 = st.pop();
                        st_push(*v2 < *v1);
                        logg << "\nResult of LESS is " << (char*)*st.top();
                        break;
                    case MATH_GRTR:
                        checkStack(2);
                        v1 = st.pop(); v2 = st.pop();
                        st_push(*v2 > *v1);
                        break;
                    case MATH_EQ:
                        checkStack(2);
                        v1 = st.pop(); v2 = st.pop();
                        st_push(*v2 == *v1);
                        break;
                    case MATH_NEQ:
                        checkStack(2);
                        v1 = st.pop(); v2 = st.pop();
                        st_push(*v2 != *v1);
                        break;
                    case MATH_LEQ:
                        checkStack(2);
                        v1 = st.pop(); v2 = st.pop();
                        st_push(*v2 <= *v1);
                        break;
                    case MATH_GEQ:
                        checkStack(2);
                        v1 = st.pop(); v2 = st.pop();
                        st_push(*v2 >= *v1);
                        break;
                    case MATH_ADD:
                        checkStack(2);
                        st_push(*st.pop() + *st.pop());
                        break;
                    case MATH_SUB:
                        checkStack(2);
                        v1 = st.pop(); v2 = st.pop();
                        st_push(*v2 - *v1);
                        break;
                    case MATH_MUL:
                        checkStack(2);
                        st_push(*st.pop() * *st.pop());
                        break;
                    case MATH_DIV:
                        checkStack(2);
                        v1 = st.pop(); v2 = st.pop();
                            if (double(*v1) == 0)
                        return error(DIVISION_BY_ZERO);
                        st_push(*v2 / *v1);
                        break;
                    case MATH_UNARY_PLUS:
                        // really do nothing
                        checkStack(1);
                        break;
                    case MATH_UNARY_MINUS:
                        checkStack(1);
                        st_push( -double(*st.pop()) );
                        break;
                    /*** Boolean operations ***/
                    case BOOL_AND:
                        checkStack(2);
                        // (st.pop() && st.pop()) is error when first pop returns false
                        v1 = st.pop(); v2 = st.pop();
                        st_push(bool(*v1) && bool(*v2));
                        logg << "\nResult of AND is " << (char*)*st.top();
                        break;
                    case BOOL_OR:
                        checkStack(2);
                        // (st.pop() || st.pop()) is error when first pop returns true
                        v1 = st.pop(); v2 = st.pop();
                        st_push(bool(*v1) || bool(*v2));
                        break;
                    case BOOL_NOT:
                        checkStack(1);
                        st_push(!(*st.pop()));
                        break;
                    default:
                        return error(INVALID_OPERATION);
                    }
                break;
              }

            // PREDICATE CALL
            case PREDICATE:
              {
                if (i.concr < 0 || i.concr >= int(m_structure->predicates.size()))
                    return error(INVALID_PREDICATE_REFERENCE);
                // collect args
                checkStack(1);
                int nArgs = int(*st.pop());
                checkStack(nArgs);
                FL::Predicates::PredicateArgs args;
                for ( ; nArgs > 0; nArgs--)
                    args.push_back(st.pop());
                if (args.size() > 1)
                    std::reverse(args.begin(), args.end());
                // call function
                FL::Predicates::Predicate &t = *m_structure->predicates[i.concr];
                try
                {
                    st_push( t(context, args) );
                    logg << "\nResult of " << t.name() << " is " << (char*)*st.top();
                }
                catch (Predicates::EPredicate &e)
                {
                    return error(
                            GuardCheckerRPN::Errors( PREDICATE_ERROR + e.errNo() ));
                }

                break;
              }
            // GOTO
            case GOTO:
              {
                if (!m_structure->code.gotoPos(i.concr))
                    return error(INVALID_GOTO_LABEL);
                break;
              }

            // GOTO_COND ("false jump")
            case GOTO_COND:
              {
                checkStack(1);
                if (! (*st.pop()) )
                    if (!m_structure->code.gotoPos(i.concr))
                        return error(INVALID_GOTO_LABEL);
                break;
              }

            // LOAD_NODES
            case LOAD_NODES:
              {
                checkStack(1);
                FL::Predicates::IndexedName *in = *st.pop();
                std::vector<ParseTreeNode*> &nodes =
                        context.getNodes(in->nameId, in->no);
                std::vector<ParseTreeNode*>::const_iterator node;
                for_each_(node, nodes)
                    st_push( *node );
               break;
             }

            // LOAD_NODE
            case LOAD_NODE:
              {
                checkStack(1);
                FL::Predicates::IndexedName *in = *st.pop();
                ParseTreeNode* node = context.getNode(in->nameId, in->no);
                st_push( node );
                break;
              }

            // NOP
            case NOP:
                // do nothing
                break;

            // UNKNOWN
            default:
                return error(INVALID_INSTRUCTION);
        }
    }
    checkStack(1);
    m_lastError = NO_ERROR;
    m_lastResult = *st.pop();
    return m_lastResult;
}

std::string GuardCheckerRPN::errorDescription(Errors errCode)
{
    if (errCode < PREDICATE_ERROR)
        return errorCodes[errCode];
    else
        return Predicates::errorDescription(
                (Predicates::Error) (errCode - PREDICATE_ERROR));
}
