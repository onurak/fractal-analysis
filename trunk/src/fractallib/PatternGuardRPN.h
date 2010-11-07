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

#ifndef PATTERN_GUARD_RPN_H
#define PATTERN_GUARD_RPN_H

#include <vector>
#include <stack>
#include "G/GVariant.h"
#include "G/GLogger.h"
#include "Predicates.h"
#include "predicates/PredicateFactory.h"
#include "PatternDescription.h"
#include "PatternGuard.h"

namespace FL {

    namespace Patterns {
        /*! \namespace RPN Guard compiler and guard checker for programs in RPN
          */
        namespace RPN {

        // Instruction.type
        static const int NOP           = 0; //!< Dummy
        static const int OPERAND       = 2; //!< Push operand on stack' top
        static const int OPERATION     = 3; //!< Math/bool operation
        static const int PREDICATE     = 4; //!< Call predicate
        static const int GOTO          = 5; //!< Goto label of concr
        static const int GOTO_COND     = 6; //!< Goto label of concr if false is on the top of the stack
        static const int LOAD_NODES    = 7; //!< Load all parse tree nodes with given parameters
        static const int LOAD_NODE     = 8; //!< Load first parse tree node with given parameters

        // Instruction.concr when Instruction.type == OPERATION
        static const int MATH_LESS = 1;
        static const int MATH_GRTR = 2;
        static const int MATH_EQ   = 3;
        static const int MATH_NEQ  = 4;
        static const int MATH_LEQ  = 5;
        static const int MATH_GEQ  = 6;

        static const int MATH_ADD = 15;
        static const int MATH_SUB = 16;
        static const int MATH_MUL = 17;
        static const int MATH_DIV = 18;

        static const int MATH_UNARY_PLUS  = 25;
        static const int MATH_UNARY_MINUS = 26;

        static const int BOOL_AND = 30;
        static const int BOOL_OR  = 31;
        static const int BOOL_NOT = 32;

        /*! \class Instruction
          * \brief Single RPN Instruction
          */
        struct Instruction
        {
            //! Default constructor
            Instruction(int type = NOP, int concr = NOP): type(type), concr(concr) {}

            int type;  //!< Instruction type (load operand, operation or function call)
            int concr; //!< Instruction subtype (operand to load, kind of instruction or function to call)
        };

        /*! \class Code
          * \brief RPN compiled program code
          */
        struct Code: public std::vector<Instruction>
        {
            //! Default constructor
            Code(): m_pos(-1) {}

            //! Start execution from begining
            void begin() { m_pos = -1; }

            //! Get current instruction and move to next
            Instruction& next() { return (*this)[++m_pos]; }

            //! Is program finished (is last instruction reached)
            bool last() { return m_pos == int(size()) - 1; }

            //! Go to position (to get position's instruction you must call next()
            bool gotoPos(int pos)
            {
                if (pos < 0 || pos > (int)size())
                    return false;
                m_pos = pos-1;
                return true;
            }

        private:
            //! Number of current instruction
            int m_pos;
        };

        /*! \class Stack
          * \brief Stack base on std::stack, but have better suitable interface
          */
        template<class T>
        struct Stack
        {
            //! Push value to stack
            void push(T item)
            {
                m_st.push(item);
            }

            //! Pop value from stack
            T pop()
            {
                T result = m_st.top();
                m_st.pop();
                return result;
            }

            //! Get top value without poping
            T top()
            {
                return m_st.top();
            }

            //! Get stack size
            int size()
            {
                return m_st.size();
            }

            //! Clear stack
            void clear()
            {
                while (size()) m_st.pop();
            }

        private:
            //! Real stack structure
            std::stack<T> m_st;
        };

        /*! \class GuardStructureRPN
          * \brief Complete RPN program
          *
          * Contains variables (vars), constants (consts), functions (predicates), code (code)
          * and method for program execution
          */
        class GuardStructureRPN: public GuardStructure
        {
        public:
            //! Variables of program
            std::vector<GVariant> vars;

            //! Functions (Predicates) of program
            std::vector<FL::Predicates::Predicate*> predicates;

            //! Program's code
            Code code;
        };

        /*! \class GuardCheckerRPN
          * \brief RPN guard checker
          */
        class GuardCheckerRPN: public GuardChecker
        {
        public:
            //! \enum Errors Execution error codes
            enum Errors
            {
                NO_ERROR = 0, //!< Program exit normaly
                EMPTY_PROGRAM, //!< Program don't have instructions
                INVALID_PROGRAM, //! While execution stack contain invalid instructions or invalid number of them
                INVALID_OPERAND_REFERENCE, //!< Reference to nonexisting operand in OPERAND instruction
                INVALID_PREDICATE_REFERENCE, //!< Reference to nonexisting function in OPERAND instruction
                INVALID_GOTO_LABEL, //!< Goto references to unknown place
                INVALID_OPERATION, //!< Unknown operation code in OPERATION instruction
                INVALID_INSTRUCTION, //!< Unknown instruction type
                DIVISION_BY_ZERO   //! Division by zero in math op
            };
        public:
            //! Constructor
            /*! Accesible structure is GuardStructureRPN
              */
            GuardCheckerRPN(GuardStructure *structure)
                : GuardChecker(structure)
            {
                setStructure(structure);
                m_varPullIndex = 0;
                for (int i = 0; i < 20; ++i)
                    m_varPull.push_back(new GVariant());
            }

            virtual ~GuardCheckerRPN()
            {
                std::vector<GVariant*>::iterator v;
                for_each_(v, m_varPull)
                    delete *v;
            }

            //! Set structure to check
            /*! Structure must have type GuardStructureRPN
              */
            virtual void setStructure(GuardStructure *structure)
            {
                m_structure = dynamic_cast<GuardStructureRPN*>(structure);
                if (m_structure == NULL)
                    GError(GCritical, "RPN Checker", 0, "Invalid structure passed");
            }

            //! Get checking structure
            virtual GuardStructure* structure()
            {
                return m_structure;
            }


            //! Execute program code
            /*! \param context Checking context (it need for predicate calls)
              * \return Result of program execution or GVariant(G_VAR_NULL) if error occures
              */
            GVariant execute(FL::Patterns::CheckContext &context);

            //! GuardChecker's check interface
            virtual bool check(CheckContext &context)
            {
                return ((bool)execute(context) == true) && (m_lastError == NO_ERROR);
            }

            //! Last execution's error code
            /*! \sa Program::Errors
              */
            Errors lastError() { return m_lastError; }

            //! Last program execution result
            GVariant lastResult() { return m_lastResult; }

            //! Description for lastError
            std::string lastErrorDescription() { return errorDescription(m_lastError); }

            //! Description to program execution errors
            std::string errorDescription(Errors errCode);
        private:
            //! Execution stack
            Stack<GVariant*> st;

            //! Pull of temporary variables
            std::vector<GVariant*> m_varPull;

            //! Current variable in pull
            int m_varPullIndex;

            //! Get new temporary variable from pull, initialize it from parameter
            GVariant* var_pull(const GVariant& v)
            {
                if (++m_varPullIndex >= int(m_varPull.size()))
                    m_varPull.push_back(new GVariant());
                *m_varPull[m_varPullIndex] = v;
                return m_varPull[m_varPullIndex];
            }

            //! Shortcut for st.push( var_pull(v) )
            inline void st_push(const GVariant& v)
            {
                st.push( var_pull(v) );
            }

            //! Last error
            Errors m_lastError;

            //! Program error
            GVariant error(Errors errCode)
            {
                m_lastError = errCode;
                return GVariant(false);
            }

            //! Stored structure
            GuardStructureRPN *m_structure;

            //! Last execution result
            GVariant m_lastResult;
        };

        } // namespace RPN

    } // namespace Patterns

} // namespace

#endif // PATTERN_GUARD_RPN_H
