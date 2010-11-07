#ifndef GUARDCOMPILERRPN_H
#define GUARDCOMPILERRPN_H

#include "PatternGuardRPN.h"

namespace FL { namespace Patterns { namespace RPN {

/*! \class GuardCompilerRPN
  * \brief RPN compiler
  *
  * Supported operations are:
  * - boolean: true, false, and, not, or. Priority or operations (top to bottom):
  *   not, and, or. Though true and false constants can be used in math expressions
  *   like 1 and 0 constants, result of expression calculation have type of left
  *   operand so (true*10 == true == 1 != 10). It is better do not mix boolean
  *   constant and numbers.
  * - math: +, -, *, /, numeric constants. All numeric constants are interpreted
  *   as double precision floating point values.
  * - relational: ==, !=, >, <, >=, <=. Result of relation is boolean.
  * - if expressions: if <cond> then <code1> [else <code2>]. Note that it's
  *   <i>expression</i>, not statement. It means that it returns value of <code1>
  *   or <code2> so it can be used in another expressions like this:
  *   \code
  *      (if true than 10 else 15) * 5
  *   \endcode
  *   (it will return 50.0).
  * - functions with arguments of type true, false or numeric constants,
  *   indexed names (like a_1 or b_*), functions and if expressions.
  * - parentheses
  *
  * Priority of operations (top to bottom): boolean, relational, math, if expression.
  *
  * Here is the example of valid complex expression (it doen't makes any sense):
  * \code
  *    if (min(a_2) >= min(a_1)) and (not (min(b_1) == 0)))
  *    then
  *       true
  *    else
  *       sum( min(a_2), max(a_2) )
  * \endcode
  *
  *
  * Complete grammar:
  * \code
  *         S            = b_expr;
  *         b_expr       = b_term ("or" b_term)*
  *         b_term       = b_not ("and" b_not)*
  *         b_not        = ["not"] b_factor
  *         b_factor     = relation
  *         relation     = m_expr [relop m_expr]
  *         m_expr       = m_term (("+" | "-") m_term)*
  *         m_term       = m_sig_factor (("*" | "/") m_factor)*
  *         m_sig_factor = ["+" | "-"] m_factor
  *         m_factor     = if_statement | function | b_const | m_const | "(" b_expr ")"
  *         if_statement   = "if" b_expr "then" b_expr ["else" b_expr]
  *         function     = name "(" arg ("," arg)* ")"
  *         b_const      = "true" | "false"
  *         m_const      = double_number;
  *         m_var        = indexed | name;
  *         relop       %= "<=" | ">=" | "<" | ">" | "==" | "!="
  *         arg          = if_statement | function | m_const | b_const | indexed | name
  *         name        %= alpha >> *alnum
  *         indexed     %= name '_' (digit+ | "*");
  * \endcode
  */
class GuardCompilerRPN: public GuardCompiler
{
public:

    //! Compile text to RPN program (GuardStructureRPN)
    virtual bool compile(const std::string &text, GuardStructure &guard);
};

}}} // namespaces

#endif // GUARDCOMPILERRPN_H
