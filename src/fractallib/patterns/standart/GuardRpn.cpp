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

#include "GuardRpn.h"
#include "../../compilers/AbstractCompiler.h"
#include "../../trees/Tree.h"
#include "../functions/FunctionFactory.h"
#include <vector>
#include <stack>


using namespace FL::Patterns::Standart;
using namespace FL::Patterns::Functions;
using namespace FL::Exceptions;
using namespace FL::Trees;

namespace FL { namespace Patterns { namespace Standart { namespace Internal
{
    using namespace FL::Compilers;

    ///////////////////////////////////////////////////////
    // SECTION: RPN CONSTANTS
    ///////////////////////////////////////////////////////

    // Instruction.type
    const int NOP           = 0; //!< Dummy
    const int OPERAND       = 2; //!< Push operand on stack' top
    const int OPERATION     = 3; //!< Math/bool operation
    const int FUNCTION      = 4; //!< Call function
    const int GOTO          = 5; //!< Goto label of concr
    const int GOTO_COND     = 6; //!< Goto label of concr if false is on the top of the stack
    const int LOAD_NODES    = 7; //!< Load all parse tree nodes with given parameters
    const int LOAD_NODE     = 8; //!< Load first parse tree node with given parameters

    // Instruction.concr when Instruction.type == OPERATION
    const int MATH_LESS = 1;
    const int MATH_GRTR = 2;
    const int MATH_EQ   = 3;
    const int MATH_NEQ  = 4;
    const int MATH_LEQ  = 5;
    const int MATH_GEQ  = 6;

    const int MATH_ADD = 15;
    const int MATH_SUB = 16;
    const int MATH_MUL = 17;
    const int MATH_DIV = 18;

    const int MATH_UNARY_PLUS  = 25;
    const int MATH_UNARY_MINUS = 26;

    const int BOOL_AND = 30;
    const int BOOL_OR  = 31;
    const int BOOL_NOT = 32;

    ///////////////////////////////////////////////////////
    // SECTION: RPN EXECUTABLE CODE
    ///////////////////////////////////////////////////////

    /*! \class Instruction
      * \brief Single RPN Instruction
      */
    struct Instruction
    {
    public:
        //! Default constructor
        Instruction(int type = NOP, int concr = NOP): type(type), concr(concr) {}

        int type;  //!< Instruction type (load operand, operation or function call)
        int concr; //!< Instruction subtype (operand to load, kind of instruction or function to call)
    };

    /*! \class Code
      * \brief RPN compiled program code
      */
    class Code: public std::vector<Instruction>
    {
    public:
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
      * \brief Stack based on std::stack, but have interface suitable for RPN Code
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

    ///////////////////////////////////////////////////////
    // SECTION: RPN PROGRAM
    ///////////////////////////////////////////////////////

    /*! \class ProgramData
      * \brief Complete RPN program data
      *
      * Contains variables (vars), constants (consts), functions (funcs) and
      * code (code)
      */
    class ProgramData
    {
    public:
        //! Variables of program
        std::vector<GVariant> vars;

        //! Functions of program
        std::vector<Function*> funcs;

        //! Program's code
        Code code;
    };

    char errorCodes[][50] =
    {
        "No error",
        "Empty program",
        "Invalid program (execution stack error)",
        "Invalid operand reference",
        "Invalid function reference",
        "Invalid goto reference",
        "Invalid operation",
        "Invalid instruction",
        "Division by zero",
        "Invalid function's arguments"
    };

    /*! \class Program
      * \brief RPN prorgam
      */
    class Program
    {
    public:
        //! \enum Errors Execution error codes
        enum Errors
        {
            NO_ERROR = 0,               //!< Program exit normaly
            EMPTY_PROGRAM,              //!< Program don't have instructions
            INVALID_PROGRAM,            //!< While execution stack contain invalid instructions or invalid number of them
            INVALID_OPERAND_REFERENCE,  //!< Reference to nonexisting operand in OPERAND instruction
            INVALID_FUNCTION_REFERENCE, //!< Reference to nonexisting function in OPERAND instruction
            INVALID_GOTO_LABEL,         //!< Goto references to unknown place
            INVALID_OPERATION,          //!< Unknown operation code in OPERATION instruction
            INVALID_INSTRUCTION,        //!< Unknown instruction type
            DIVISION_BY_ZERO,           //!< Division by zero in math op
            FUNCTION_ERROR              //!< If error code >= FUNCTION_ERROR than its one of FL::Functions::Error code
        };
    public:
        //! Constructor
        Program()
        {
            m_varPullIndex = 0;
            for (int i = 0; i < 20; ++i)
                m_varPull.push_back(new GVariant());
        }

        // Destructor
        virtual ~Program()
        {
            std::vector<GVariant*>::iterator v;
            forall(v, m_varPull)
                delete *v;
        }

        //! Execute program code
        /*! \param context Checking context (it need for predicate calls)
          * \return Result of program execution or GVariant(G_VAR_NULL) if error occures
          */
        GVariant execute(FL::Patterns::Context &context)
        {
            #define checkStack(sz) \
            if (st.size() < sz)  \
                return error(INVALID_PROGRAM);


            if (m_pd.code.size() == 0)
                return error(EMPTY_PROGRAM);
            m_pd.code.begin();
            st.clear();
            while (!m_pd.code.last())
            {
                Instruction &i = m_pd.code.next();
                switch (i.type)
                {
                    // VARIABLE
                    case OPERAND:
                    {
                        if (i.concr < 0 || i.concr >= int(m_pd.vars.size()))
                            return error(INVALID_OPERAND_REFERENCE);
                        st.push(&(m_pd.vars[i.concr]));
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

                    // FUNCTION CALL
                    case FUNCTION:
                    {
                        if (i.concr < 0 || i.concr >= int(m_pd.funcs.size()))
                            return error(INVALID_FUNCTION_REFERENCE);
                        // collect args
                        checkStack(1);
                        int nArgs = int(*st.pop());
                        checkStack(nArgs);
                        FunctionArgs args;
                        for ( ; nArgs > 0; nArgs--)
                            args.push_back(st.pop());
                        if (args.size() > 1)
                            std::reverse(args.begin(), args.end());
                        // call function
                        Function &f = *m_pd.funcs[i.concr];
                        try
                        {
                            st_push( f(context, args) );
                        }
                        catch (const FL::Exceptions::EArguments &e)
                        {
                            return error(
                                    Program::Errors( FUNCTION_ERROR + e.id() ));
                        }

                        break;
                    }
                    // GOTO
                case GOTO:
                    {
                        if (!m_pd.code.gotoPos(i.concr))
                            return error(INVALID_GOTO_LABEL);
                        break;
                    }

                    // GOTO_COND ("false jump")
                case GOTO_COND:
                    {
                        checkStack(1);
                        if (! (*st.pop()) )
                            if (!m_pd.code.gotoPos(i.concr))
                                return error(INVALID_GOTO_LABEL);
                        break;
                    }

                    // LOAD_NODES
                case LOAD_NODES:
                    {
                        checkStack(1);
                        IndexedName *in = *st.pop();
                        const Layer &nodes = context.getNodes(in->nameId, in->no);
                        Layer::ConstIterator node;
                        forall(node, nodes)
                            st_push( *node );
                        break;
                    }

                    // LOAD_NODE
                case LOAD_NODE:
                    {
                        checkStack(1);
                        IndexedName *in = *st.pop();
                        Node* node = context.getNode(in->nameId, in->no);
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

        //! Get program data
        ProgramData& pd() { return m_pd; }
    private:
        //! Program data
        ProgramData m_pd;

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

        //! Program error
        /*
        GVariant error(Predicates::EPredicate &e)
        {
            m_lastError = (GuardCheckerRPN::Errors) (PREDICATE_ERROR + e.errNo());
            return GVariant(false);
        }
        */

        //! Last execution result
        GVariant m_lastResult;
    };

    ///////////////////////////////////////////////////////
    // RPN COMPILER
    ///////////////////////////////////////////////////////
    class RpnCompiler : public AbstractCompiler
    {
    public:
        GuardRpn::GuardSet *m_programs;
        Program *m_program;
        CINode m_currentNode;
        int m_isFuncArg;
    public:
        RpnCompiler(GuardRpn::GuardSet *programs)
        {
            m_programs = programs;
            FunctionFactory::registerAll();
            m_oneLineCommentBegin = '#';
            // fill reserved word list
            m_reservedWords["true"]    = LEX_TRUE;
            m_reservedWords["false"]   = LEX_FALSE;
            m_reservedWords["if"]      = LEX_IF;
            m_reservedWords["then"]    = LEX_THEN;
            m_reservedWords["else"]    = LEX_ELSE;
            m_reservedWords["not"]     = LEX_NOT;
            m_reservedWords["or"]      = LEX_OR;
            m_reservedWords["and"]     = LEX_AND;

            // enable standart lexems
            filterLexeme(LEX_PLUS,      true);
            filterLexeme(LEX_MINUS,     true);
            filterLexeme(LEX_LPAREN,    true);
            filterLexeme(LEX_RPAREN,    true);
            filterLexeme(LEX_INTEGER,   true);
            filterLexeme(LEX_FLOAT,     true);
            filterLexeme(LEX_EQ,        true);
            filterLexeme(LEX_NEQ,       true);
            filterLexeme(LEX_GRTR,      true);
            filterLexeme(LEX_GEQ,       true);
            filterLexeme(LEX_LESS,      true);
            filterLexeme(LEX_LEQ,       true);
            filterLexeme(LEX_DIV,       true);
            filterLexeme(LEX_COMMA,     true);
            filterLexeme(LEX_SEMICOLON, true);
            filterLexeme(LEX_COLON,     true);
        }



    protected:

        std::string m_name;
        std::string m_funcName;

        virtual void vgl()
        {
            m_name = "";

            while (m_l == LEX_UNKNOWN)
            {
                // eof
                if (isEoi())
                {
                    m_l = LEX_EOI;
                }
                // spaces
                else if (isspace(c()) && m_lexprev.id != LEX_INDEXED_NAME)
                {
                     gc();
                }
                // names
                else if (isalpha(c()))
                {
                    m_name += c();
                    while ( isalnum(gc()) )
                        m_name += c();
                    // reserved word
                    std::map<std::string, LexemeId>::const_iterator
                            it_rw = m_reservedWords.find(m_name);
                    if (it_rw != m_reservedWords.end())
                        m_l = it_rw->second;
                    // indexed name
                    else if (c() == '_')
                    {
                        m_l = LEX_INDEXED_NAME;
                        gc();
                    }
                    // name
                    else
                        m_l = LEX_NAME;
                }
                else if (c() == '?')
                {
                    gc();
                    if (c() != '_')
                        m_l = LEX_NAME;
                    else
                    {
                        m_name = "?";
                        m_l = LEX_INDEXED_NAME;
                        gc();
                    }
                }
                // math * or wildcard indexed name
                else if (c() == '*')
                {
                    gc();
                    if (c() != '_')
                        m_l = LEX_MULT;
                    else
                    {
                        m_name = "*";
                        m_l = LEX_INDEXED_NAME;
                        gc();
                    }
                }
                else if (c() == '-')
                {
                    gc();
                    if (c() == '>')
                    {
                        m_l = LEX_COLON;
                        gc();
                    }
                    else
                        m_l = LEX_MINUS;
                }
                else
                    AbstractCompiler::vgl();
            }
        }

    protected:
        //**** LEXEMES *****
        static const LexemeId LEX_TRUE = 1;
        static const LexemeId LEX_FALSE = 2;
        static const LexemeId LEX_IF = 20;
        static const LexemeId LEX_THEN = 21;
        static const LexemeId LEX_ELSE = 22;
        static const LexemeId LEX_INDEXED_NAME = 23;

        int LEX2OP(LexemeId lex)
        {
            switch (lex)
            {
                case LEX_LESS:  return MATH_LESS;
                case LEX_LEQ:   return MATH_LEQ;
                case LEX_GRTR:  return MATH_GRTR;
                case LEX_GEQ:   return MATH_GEQ;
                case LEX_EQ:    return MATH_EQ;
                case LEX_NEQ:   return MATH_NEQ;
                case LEX_PLUS:  return MATH_ADD;
                case LEX_MINUS: return MATH_SUB;
                case LEX_MULT:  return MATH_MUL;
                case LEX_DIV:   return MATH_DIV;
                default:
                    return 0;
            }
        }


        virtual void S()
        {
            GuardRpn::GuardSet::iterator itProg;
            forall(itProg, *m_programs)
                delete itProg->second;
            m_programs->clear();

            m_isFuncArg = 0;
            while (m_l != LEX_SEMICOLON && m_l != LEX_EOI)
            {
                symbol_guard();
                if (m_l != LEX_COMMA && m_l != LEX_SEMICOLON)
                    error(E_EXPECTED_TOKEN, ";");
                if (m_l == LEX_COMMA)
                    gl();
            }

            if (m_l != LEX_SEMICOLON)
                error(E_EXPECTED_TOKEN, ";");
            gl();
        }

        void symbol_guard()
        {
            if (m_l != LEX_INDEXED_NAME && m_l != LEX_NAME)
                error(E_EXPECTED_TOKEN, "name, indexed or wildcard");

            // Read node parameters (name and optional index)
            if (m_name != "?")
                m_currentNode.id = IDGenerator::idOf(m_name);
            else
                m_currentNode.id = IDGenerator::WILDCARD;
            if (m_l == LEX_NAME)
            {
                m_currentNode.index = -1;
                gl();
                if (m_l == LEX_MULT)
                    gl();
            }
            else if (m_l == LEX_INDEXED_NAME)
            {
                gl();
                m_currentNode.index = m_symbolsTable[m_lex.index];
                gl();
            }

            if (m_l != LEX_COLON)
                error(E_EXPECTED_TOKEN, "->");
            gl();

            // Set new program for this node
            (*m_programs)[m_currentNode] = m_program = new Program();

            // Read program itself
            b_expr();
        }

        void b_expr()
        {
            b_term();
            while (m_l == LEX_OR)
            {
                gl();
                b_term();
                addOperation(BOOL_OR);
            }
        }

        void b_term()
        {
            b_not();
            while (m_l == LEX_AND)
            {
                gl();
                b_not();
                addOperation(BOOL_AND);
            }
        }

        void b_not()
        {
            if (m_l == LEX_NOT)
            {
                gl();
                b_factor();
                addOperation(BOOL_NOT);
            }
            else
                b_factor();
        }

        void b_factor()
        {
            /*
                    if (m_l == LEX_TRUE)
                    {
                        gl();
                        addOperand(true);
                    }
                    else if (m_l == LEX_FALSE)
                    {
                        gl();
                        addOperand(false);
                    }
                    else
                    */
            relation();
        }

        void relation()
        {
            m_expr();
            if (m_l == LEX_EQ   || m_l == LEX_NEQ || m_l == LEX_LESS ||
                m_l == LEX_GRTR || m_l == LEX_LEQ || m_l == LEX_GEQ)
            {
                LexemeId prevLex = m_l;
                gl();
                m_expr();
                addOperation(LEX2OP(prevLex));
            }
        }

        void m_expr()
        {
            m_term();
            while (m_l == LEX_PLUS || m_l == LEX_MINUS)
            {
                LexemeId prevLex = m_l;
                gl();
                m_term();
                addOperation(LEX2OP(prevLex));
            }
        }

        void m_term()
        {
            m_sig_factor();
            while (m_l == LEX_MULT || m_l == LEX_DIV)
            {
                LexemeId prevLex = m_l;
                gl();
                m_factor();
                addOperation(LEX2OP(prevLex));
            }
        }

        void m_sig_factor()
        {
            if (m_l == LEX_PLUS)
            {
                gl();
                //addOperation(MATH_UNARY_PLUS); // better don't
            }
            else if (m_l == LEX_MINUS)
            {
                gl();
                addOperation(MATH_UNARY_MINUS);
            }
            m_factor();
        }

        void m_factor()
        {
            if (m_l == LEX_IF)
            {
                gl();
                if_statement();
            }
            else if (m_l == LEX_TRUE)
            {
                gl();
                addOperand(true);
            }
            else if (m_l == LEX_FALSE)
            {
                gl();
                addOperand(false);
            }
            else if (m_l == LEX_FLOAT || m_l == LEX_INTEGER)
            {
                addOperand((double)(m_symbolsTable[m_lex.index]));
                gl();
            }
            else if (m_l == LEX_NAME)
            {
                m_funcName = m_name;
                gl();
                if (m_l == LEX_LPAREN)
                {
                    function();
                }
                else if (m_isFuncArg)
                    addOperand(m_funcName);
                else
                    error(E_SYNTAX_ERROR, "Pattern name outside function");
            }
            else if (m_l == LEX_INDEXED_NAME)
            {
                if (!m_isFuncArg)
                    error(E_SYNTAX_ERROR, "Pattern name outside function");
                std::string indexedName = m_name;
                int indexedNo = 0;
                gl();
                if (m_l == LEX_INTEGER)
                {
                    indexedNo = m_symbolsTable[m_lex.index];
                    gl();
                }
                else if (m_l == LEX_MULT)
                {
                    if (IDGenerator::idOf(indexedName) == IDGenerator::WILDCARD)
                        error(E_SYNTAX_ERROR, "* both on name and index");
                    indexedNo = -1;
                    gl();
                }
                else
                    error(E_SYNTAX_ERROR, "Index or * expected");

                // Check that this name is previously defined
                CINode cinode;
                cinode.id = (indexedName != "*" && indexedName != "?") ?
                      IDGenerator::idOf(indexedName) : IDGenerator::WILDCARD;
                cinode.index = indexedNo;
                if (cinode.id != -1  &&  m_programs->find(cinode) == m_programs->end())
                {
                    /*
                    error(E_SYNTAX_ERROR,
                          std::string("Symbol ") + cinode.toStr() +
                          " is not defined in context of " + m_currentNode.toStr());
                    */
                }

                // push indexed parameters
                int opIndex = addOperand( GVariant(IndexedName(cinode.id, cinode.index)) );

                // push LOAD_NODE or LOAD_NODES instruction
                if (indexedNo == -1)
                    addInstruction(LOAD_NODES, opIndex);
                else
                    addInstruction(LOAD_NODE, opIndex);
            }
            else if (m_l == LEX_LPAREN)
            {
                gl();
                b_expr();
                if (m_l != LEX_RPAREN)
                    error(E_EXPECTED_TOKEN, ")");
                gl();
            }
        }

        // How it works:
        // IF:
        //     <condition code>
        //     jump_if_false ELSE
        // THEN:
        //     <then code>
        //     jump_to END
        // ELSE:
        //     <else code (optional)>
        // END:
        void if_statement()
        {
            b_expr();
            if (m_l != LEX_THEN)
                error(E_EXPECTED_TOKEN, "then");
            // add "jump_if_false ELSE" dummy
            m_program->pd().code.push_back(Instruction(GOTO_COND, -1));
            int dummyPos = m_program->pd().code.size()-1;
            gl();
            b_expr();
            // add "jump_to END" dummy, fill previous dummy
            m_program->pd().code.push_back(Instruction(GOTO, -1));
            m_program->pd().code[dummyPos].concr = m_program->pd().code.size();
            dummyPos = m_program->pd().code.size()-1;
            if (m_l == LEX_ELSE)
            {
                gl();
                b_expr();
            }
            // fill previous dummy
            m_program->pd().code[dummyPos].concr = m_program->pd().code.size();
        }

        void function()
        {
            if (m_l != LEX_LPAREN)
                error(E_EXPECTED_TOKEN, "(");
            gl();
            Function *f = FunctionFactory::get(m_funcName);
            if (f == NULL)
                error(E_UNKNOWN_IDENTIFIER, m_funcName);
            m_isFuncArg++;
            int argCount = 0;
            if (m_l != LEX_RPAREN)
            {
                arg();
                argCount++;
            }
            while (m_l == LEX_COMMA)
            {
                gl();
                arg();
                argCount++;
            }
            if (m_l != LEX_RPAREN)
                error(E_EXPECTED_TOKEN, ")");
            gl();

            addCall(f, argCount);
            m_isFuncArg--;
        }

        void arg()
        {
            b_expr();
        }

        inline void addInstruction(int type, int concr)
        {
            m_program->pd().code.push_back(Instruction(type, concr));
        }

        inline void addOperation(int concr)
        {
            m_program->pd().code.push_back(Instruction(OPERATION, concr));
        }

        inline int addOperand(const GVariant &op)
        {
            m_program->pd().vars.push_back(op);
            int opIndex = m_program->pd().vars.size()-1;
            m_program->pd().code.push_back(Instruction(OPERAND, opIndex));
            return opIndex;
        }

        inline void addCall(Function *f, int argCount)
        {
            int functionIndex;
            std::vector<Function*>::const_iterator
                 it = std::find(m_program->pd().funcs.begin(),
                                m_program->pd().funcs.end(),
                                f);
            if (it == m_program->pd().funcs.end())
            {
                m_program->pd().funcs.push_back(f);
                functionIndex = m_program->pd().funcs.size()-1;
            } else
                functionIndex = it - m_program->pd().funcs.begin();
            addOperand(argCount);
            m_program->pd().code.push_back(Instruction(FUNCTION, functionIndex));
        }

    };
}}}} // namespace Internal



GuardRpn::GuardRpn(const FL::Patterns::Standart::DescriptionEbnf &description)
    : Guard(description)
{
}

GuardRpn::~GuardRpn()
{
}


EParsing GuardRpn::compile(Compilers::Input &i)
{
    Internal::RpnCompiler compiler(&m_rpnPrograms);
    if (!compiler.compile(&i))
        return compiler.lastError();
    else
        return EParsing(E_OK);
}

bool GuardRpn::check(Context &c, CheckInfo &info)
{
    GuardSet::iterator guard;

    forall(guard, m_rpnPrograms)
    {
        Internal::Program *program = guard->second;
        if (program == NULL)
            continue;

        CINode node = guard->first;
        if ( c.getNode(node.id, node.index) == NULL )
            continue;

        bool result = (program->execute(c)) &&
                      (program->lastError() == Internal::Program::NO_ERROR);

        if (!result)
            return false;
    }
    return true;
}
