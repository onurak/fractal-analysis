#include "GuardRpn.h"
#include "../../compilers/AbstractCompiler.h"
#include "../../trees/Tree.h"
#include "../functions/FunctionFactory.h"
#include <vector>
#include <stack>


using namespace FL::Patterns::Standard;
using namespace FL::Patterns::Functions;
using namespace FL::Exceptions;
using namespace FL::Trees;

namespace FL { namespace Patterns { namespace Standard { namespace Internal
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
                        FunctionArgs args(nArgs);
                        for ( ; nArgs > 0; nArgs--)
                            args[nArgs-1] = st.pop();

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


                    // GOTO_COND ("false jump")
                    case GOTO_COND:
                    {
                        checkStack(1);
                        if (! (*st.pop()) )
                            if (!m_pd.code.gotoPos(i.concr))
                                return error(INVALID_GOTO_LABEL);
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

    /** \class RpnCompiler
      * \brief Actual RPN guard compiler
      *
      * Grammar:
      * \code
      * S            = [Attribute ("," Attribute)*] ";"
      * Attribute    = NodeCond | VisualAttribute
      * NodeCond     = MaskedNode ":" Expr
      * Expr         = BoolOr | IfThenElse
      * BoolOr       = BoolAnd ("||" BoolAnd)*
      * BoolAnd      = Relation ("&&" Relation)*
      * Relation     = MathAdd (("==" | "!=" | ">=" | "<=" | ">" | "<") MathAdd)*
      * MathAdd      = MathMult (("+" | "-") MathMult)
      * MathMult     = NegTerm (("*" | "/") NegTerm)
      * NegTerm      = ["+" | "-" | "!"] Term
      * Term         = "(" BoolOr ")" | FuncCall | Number | BoolConst
      *
      * IfThenElse   = "if" Expr "then" Expr "else" Expr
      *
      * FuncCall     = Name ArgList
      * ArgList      = "("   Expr ("," Expr)*   ")"
      *
      * Node         = Name ["_" digit+]
      * MaskedNode   = NodeMask ["_" NodeIndex]
      * NodeMask     = (Name | "*")
      * NodeIndex    = digit+ | "*"
      * Name         = alpha (digit | alpha)*
      * Number       = digit* ["." digit*] | "." digit+
      * BoolConst    = "true" | "false"
      *
      * VisualAttribute = "visual" ":" VisualFigure ("," VisualFigure)*
      * VisualFigure    = "line" "(" Expr "," Expr "," Expr "," Expr ")"
      * \endcode
      */

    class RpnCompiler : public AbstractCompiler
    {
    public:
        GuardRpn::GuardSet *m_programs;
        Program *m_program;
        CINode m_cinode;
        int m_funcCallingDepth;
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
            m_reservedWords["visual"]  = LEX_VISUAL;
            m_reservedWords["line"]    = LEX_VISUAL_LINE;

            // enable standard lexems
            filterLexeme(LEX_PLUS,      true);
            filterLexeme(LEX_MINUS,     true);
//            filterLexeme(LEX_MULT,      true);
            filterLexeme(LEX_COLON,     true);
            filterLexeme(LEX_LPAREN,    true);
            filterLexeme(LEX_RPAREN,    true);
            filterLexeme(LEX_INTEGER,   true);
            filterLexeme(LEX_FLOAT,     true);
            //filterLexeme(LEX_AND,       true);
            //filterLexeme(LEX_OR,        true);
            //filterLexeme(LEX_NOT,       true);
            filterLexeme(LEX_EQ,        true);
            filterLexeme(LEX_NEQ,       true);
            filterLexeme(LEX_GRTR,      true);
            filterLexeme(LEX_GEQ,       true);
            filterLexeme(LEX_LESS,      true);
            filterLexeme(LEX_LEQ,       true);
            filterLexeme(LEX_DIV,       true);
            filterLexeme(LEX_COMMA,     true);
            filterLexeme(LEX_SEMICOLON, true);
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
                // wildcard OR MULTIPLICATION
                else if (c() == '*')
                {
                    gc();

                    // Is it name or index?
                    if (m_lexprev.id != LEX_INDEXED_WILDCARD &&
                        m_lexprev.id != LEX_INDEXED_NAME)
                    {
                        if (c() != '_')
                        {
                            m_l = LEX_MULT;
//                            throw EParsing(E_SYNTAX_ERROR, m_input->line(), m_input->column(),
//                                           "Name wildcard must have an index");
                        }
                        else
                        {
                            m_name = "*";
                            m_l = LEX_INDEXED_WILDCARD;
                            gc();
                        }
                    }
                    else
                    {
                        m_l = LEX_WILDCARD;
                    }
                }
                else
                {
                    AbstractCompiler::vgl();
                }
            }
        }

    protected:
        //**** LEXEMES *****
        static const LexemeId LEX_TRUE              = 1;
        static const LexemeId LEX_FALSE             = 2;
        static const LexemeId LEX_IF                = 20;
        static const LexemeId LEX_THEN              = 21;
        static const LexemeId LEX_ELSE              = 22;
        static const LexemeId LEX_INDEXED_NAME      = 23;
        static const LexemeId LEX_WILDCARD          = 24;
        static const LexemeId LEX_INDEXED_WILDCARD  = 25;
        static const LexemeId LEX_VISUAL            = 50;
        static const LexemeId LEX_VISUAL_LINE       = 51;

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
            m_funcCallingDepth = 0;

            if (m_l != LEX_SEMICOLON)
            {
                Attribute();

                while (m_l == LEX_COMMA)
                {
                    gl();
                    Attribute();
                }

                if (m_l != LEX_SEMICOLON)
                    throw EParsing(E_EXPECTED_TOKEN, m_input->line(), m_input->column(), ";");
            }
        }

        void Attribute()
        {
            if (m_l == LEX_NAME || m_l == LEX_INDEXED_NAME || m_l == LEX_INDEXED_WILDCARD)
                NodeCond();
            else if (m_l == LEX_VISUAL)
                VisualAttribute();
            else
                throw EParsing(E_EXPECTED_TOKEN, m_input->line(), m_input->column(), "attribute");
        }

        void VisualAttribute()
        {
            if (m_l != LEX_VISUAL)
                throw EParsing(E_EXPECTED_TOKEN, m_input->line(), m_input->column(), "visual");
            gl();
            if (m_l != LEX_COLON)
                throw EParsing(E_EXPECTED_TOKEN, m_input->line(), m_input->column(), ":");
            gl();
            if (m_l == LEX_VISUAL_LINE)
            {
                gl();
                VisualLine();
            }
            else
                throw EParsing(E_EXPECTED_TOKEN, m_input->line(), m_input->column(), "line");

            while (m_l == LEX_COMMA)
            {
                if (m_l == LEX_VISUAL_LINE)
                {
                    gl();
                    VisualLine();
                }
                else
                    throw EParsing(E_EXPECTED_TOKEN, m_input->line(), m_input->column(), "line");
            }
        }

        void VisualLine()
        {

        }

        void NodeCond()
        {
            m_program = new Program();
            try
            {
                MaskedNode();

                if (m_programs->findGuard(m_cinode.id, m_cinode.index))
                    error(E_SYNTAX_ERROR, "Duplicated guard");

                if (m_l != LEX_COLON)
                    throw EParsing(E_EXPECTED_TOKEN, m_input->line(), m_input->column(), ":");
                gl();
                Expr();

                m_programs->push_back(
                            GuardRpn::GuardPair(m_cinode.id, m_cinode.index, m_program));
            }
            catch (...)
            {
                delete m_program;
                m_program = NULL;
            }
        }

        void Expr()
        {
            if (m_l == LEX_IF)
                IfThenElse();
            else
                BoolOr();
        }

        void BoolOr()
        {
            BoolAnd();
            while (m_l == LEX_OR)
            {
                gl();
                BoolAnd();
                addOperation(BOOL_OR);
            }
        }

        void BoolAnd()
        {
            Relation();
            while (m_l == LEX_AND)
            {
                gl();
                Relation();
                addOperation(BOOL_AND);
            }
        }

        void Relation()
        {
            MathAdd();

            while (m_l == LEX_EQ  || m_l == LEX_NEQ  || m_l == LEX_GEQ ||
                   m_l == LEX_LEQ || m_l == LEX_GRTR || m_l == LEX_LESS)
            {
                LexemeId prevLex = m_l;
                gl();
                MathAdd();
                addOperation(LEX2OP(prevLex));
            }
        }

        void MathAdd()
        {
            MathMult();

            while (m_l == LEX_PLUS || m_l == LEX_MINUS)
            {
                LexemeId prevLex = m_l;
                gl();
                MathMult();
                addOperation(prevLex == LEX_PLUS ? MATH_ADD : MATH_SUB);
            }
        }

        void MathMult()
        {
            NegTerm();

            while (m_l == LEX_MULT || m_l == LEX_DIV)
            {
                LexemeId prevLex = m_l;
                gl();
                NegTerm();
                addOperation(prevLex == LEX_MULT ? MATH_MUL : MATH_DIV);
            }
        }

        void NegTerm()
        {
            LexemeId prevL = m_l;
            if (m_l == LEX_PLUS || m_l == LEX_MINUS || m_l == LEX_NOT)
                gl();

            Term();

            if (prevL == LEX_MINUS)
            {
                addOperand(0);
                addOperation(MATH_SUB);
            }
            else if (prevL == LEX_NOT)
            {
                addOperation(BOOL_NOT);
            }
        }

        void Term()
        {
            if (m_l == LEX_LPAREN)
            {
                gl();
                Expr();
                if (m_l != LEX_RPAREN)
                    throw EParsing(E_EXPECTED_TOKEN, m_input->line(), m_input->column(), ")");
                gl();
            }
            else if (m_l == LEX_TRUE)
            {
                addOperand(1);
                gl();
            }
            else if (m_l == LEX_FALSE)
            {
                addOperand(0);
                gl();
            }
            else if (m_l == LEX_NAME)
            {
                Functions::Function *f = Functions::FunctionFactory::get(m_name);
                if (f != NULL)
                {
                    FuncCall();
                }
                else if (m_funcCallingDepth > 0)
                {
                    //int opIndex = addOperand(GVariant(IndexedName(IDGenerator::idOf(m_name), -1)));
                    //addInstruction(LOAD_NODES, opIndex);
                    addOperand(IDGenerator::idOf(m_name));
                    gl();
                }
                else
                    throw EParsing(E_UNEXPECTED_TOKEN, m_input->line(), m_input->column(), m_name);
            }
            else if (m_l == LEX_INDEXED_NAME)
            {
                if (m_funcCallingDepth > 0)
                {
                    /*
                    int nameId =
                            m_name != "*" ?
                                IDGenerator::idOf(m_name) :
                                IDGenerator::WILDCARD;
                    */

                    int nameId = IDGenerator::idOf(m_name);

                    gl();
                    if (m_l == LEX_INTEGER)
                    {
                        int opIndex = addOperand(GVariant(IndexedName(
                                       nameId,
                                       m_symbolsTable[m_lex.index])));
                        addInstruction(
                             nameId != IDGenerator::WILDCARD ? LOAD_NODE : LOAD_NODES,
                             opIndex);
                        gl();
                    }
                    /*
                    else if (m_l == LEX_WILDCARD)
                    {
                        int opIndex = addOperand(GVariant(IndexedName(nameId, IDGenerator::WILDCARD)));
                        addInstruction(LOAD_NODES, opIndex);
                        gl();
                    }
                    */
                    else
                        throw EParsing(E_UNEXPECTED_TOKEN, m_input->line(), m_input->column());
                }
                else
                    throw EParsing(E_UNEXPECTED_TOKEN, m_input->line(), m_input->column(), m_name);
            }
            else if (m_l == LEX_INTEGER)
            {
                addOperand(m_symbolsTable[m_lex.index]);
                gl();
            }
            else if (m_l == LEX_FLOAT)
            {
                addOperand(m_symbolsTable[m_lex.index]);
                gl();
            }
            else
                throw EParsing(E_UNEXPECTED_TOKEN, m_input->line(), m_input->column());
        }

        void FuncCall()
        {
            if (m_l != LEX_NAME)
                throw EParsing(E_UNEXPECTED_TOKEN, m_input->line(), m_input->column());

            Functions::Function *f = Functions::FunctionFactory::get(m_name);
            if (f == NULL)
                throw EParsing(E_UNEXPECTED_TOKEN, m_input->line(), m_input->column());


            gl();
            int argCount = ArgList();
            addCall(f, argCount);
        }

        int ArgList()
        {
            m_funcCallingDepth += 1;
            if (m_l != LEX_LPAREN)
                throw EParsing(E_EXPECTED_TOKEN, m_input->line(), m_input->column(), "(");
            gl();

            int argCount = 0;
            while (m_l != LEX_RPAREN && !m_input->isEoi())
            {
                Expr();
                argCount++;
                if (m_l != LEX_COMMA)
                    break;
                gl();
            }

            if (m_l != LEX_RPAREN)
                throw EParsing(E_EXPECTED_TOKEN, m_input->line(), m_input->column(), ")");
            gl();
            m_funcCallingDepth -= 1;

            return argCount;
        }

        void IfThenElse()
        {
            // Compile condition
            if (m_l != LEX_IF)
                throw EParsing(E_EXPECTED_TOKEN, m_input->line(), m_input->column(), "if");
            gl();

            Expr();

            // GOTO FALSE instruction stub
            size_t iGotoFalse = addInstruction(GOTO_COND, 0);

            if (m_l != LEX_THEN)
                throw EParsing(E_EXPECTED_TOKEN, m_input->line(), m_input->column(), "then");
            gl();

            // TRUE branch instructions
            Expr();

            // GOTO end of IF instruction stub
            size_t iGotoEnd = addInstruction(GOTO, 0);

            // Correct stub
            m_program->pd().code[iGotoFalse].concr = m_program->pd().code.size();

            // FALSE brach instructions
            if (m_l != LEX_ELSE)
                throw EParsing(E_EXPECTED_TOKEN, m_input->line(), m_input->column(), "else");
            gl();

            Expr();

            // Correct stub
            m_program->pd().code[iGotoEnd].concr = m_program->pd().code.size();
        }

        void MaskedNode()
        {
            if (m_l == LEX_NAME)
            {
                m_cinode.id    = IDGenerator::idOf(m_name);
                m_cinode.index = IDGenerator::WILDCARD;
                gl();
            }
            else if (m_l == LEX_INDEXED_NAME || m_l == LEX_INDEXED_WILDCARD)
            {
                m_cinode.id =
                        m_name != "*" ?
                            IDGenerator::idOf(m_name) :
                            IDGenerator::WILDCARD;
                gl();
                if (m_l == LEX_INTEGER)
                {
                    m_cinode.index = m_symbolsTable[m_lex.index];
                    gl();
                }
                else if (m_l == LEX_WILDCARD)
                {
                    m_cinode.index = IDGenerator::WILDCARD;
                    gl();
                }
                else
                    throw EParsing(E_UNEXPECTED_TOKEN, m_input->line(), m_input->column());
            }
            else
                throw EParsing(E_UNEXPECTED_TOKEN, m_input->line(), m_input->column());
        }



        //! Add instruction to code
        /*! \return Index of added instruction
          */
        inline size_t addInstruction(int type, int concr)
        {
            m_program->pd().code.push_back(Instruction(type, concr));
            return m_program->pd().code.size()-1;
        }

        //! Add instruction to load operand to stack
        /*! Operand must be in in program vars in specified position
          */
        inline void addOperation(int concr)
        {
            m_program->pd().code.push_back(Instruction(OPERATION, concr));
        }

        //! Add instruction to load operand to stack
        /*! Operand will be added to program variables
          */
        inline int addOperand(const GVariant &op)
        {
            int opIndex = -1;

            for (int i = 0; i < (int) m_program->pd().vars.size(); ++i)
            {
                GVariant &v = m_program->pd().vars[i];
                if (v.type() == op.type() &&
                    v.type() != G_VAR_CUSTOM &&
                    op.type() != G_VAR_CUSTOM &&
                    v == op)
                {
                    opIndex = i;
                    break;
                }
            }
            if (opIndex == -1)
            {
                m_program->pd().vars.push_back(op);
                opIndex = m_program->pd().vars.size()-1;
            }

            m_program->pd().code.push_back(Instruction(OPERAND, opIndex));
            return opIndex;
        }

        //! Add instruction to call function
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
            }
            else
            {
                functionIndex = it - m_program->pd().funcs.begin();
            }
            addOperand(argCount);
            m_program->pd().code.push_back(Instruction(FUNCTION, functionIndex));
        }

    };
}}}} // namespace Internal



GuardRpn::GuardRpn(const FL::Patterns::Description &description)
    : Guard(description)
{
}

GuardRpn::~GuardRpn()
{
    GuardSet::iterator i;
    forall(i, m_rpnPrograms)
        delete i->program;
    m_rpnPrograms.clear();

    RawVisualAttributes::iterator a;
    std::vector<Internal::Program*>::iterator p;
    forall(a, m_visualAttributes)
        forall(p, a->points)
            delete *p;
    m_visualAttributes.clear();
}


EParsing GuardRpn::compile(Compilers::Input &i)
{
    Internal::RpnCompiler compiler(&this->m_rpnPrograms);
    if (!compiler.compile(&i))
        return compiler.lastError();
    else
        return EParsing(E_OK);
}

bool GuardRpn::getGuardsForNode(FL::Trees::Node *node)
{
    m_nodeGuards.clear();

    Internal::Program *p;

    // Look for exact match
    if ((p = m_rpnPrograms.findGuard(node->id(), node->index())) != NULL)
        m_nodeGuards.push_back(p);

    // Look for match with index wildcard
    if ((p = m_rpnPrograms.findGuard(node->id(), FL::IDGenerator::WILDCARD)) != NULL)
        m_nodeGuards.push_back(p);

    // Look for match with name wildcard
    if ((p = m_rpnPrograms.findGuard(FL::IDGenerator::WILDCARD, node->index())) != NULL)
        m_nodeGuards.push_back(p);

    // Look for match with name and index wildcards
    if ((p = m_rpnPrograms.findGuard(FL::IDGenerator::WILDCARD, FL::IDGenerator::WILDCARD)) != NULL)
        m_nodeGuards.push_back(p);

    return m_nodeGuards.size() > 0;
}

bool GuardRpn::check(Context &c)
{
    using namespace FL::Patterns;

    // Check programs for those items which already
    // added to last parsed

    if (m_rpnPrograms.size() == 0)
        return true;

    Layer::ConstIterator itNode;
    forall(itNode, c.lastParsed())
    {
        Node *node = *itNode;
        // If program exists - execute it
        if (getGuardsForNode(node))
        {
            // Remember node for Node() function
            c.setCurrentItNode(node);

            std::vector<Internal::Program*>::iterator itGuardProgram;
            forall(itGuardProgram, m_nodeGuards)
            {
                Internal::Program *program = *itGuardProgram;

                // Execute program
                GVariant result = program->execute(c);

                // Check guard execution result
                if (program->lastError() != Internal::Program::NO_ERROR)
                    return false;

                if (result.canCastTo(G_VAR_BOOL))
                {
                    if (! (bool)result)
                        return false;
                }
                else if (result.type() == G_VAR_CUSTOM ||
                         result.type() == G_VAR_PVOID)
                {
                    if ((void*) result == NULL)
                        return false;
                }
            }
        }
    }

    return true;
}

GuardRpn::VisualAttributes GuardRpn::visualAttributes()
{
    VisualAttributes attr;

    RawVisualAttributes::iterator rattr;
    forall(rattr, m_visualAttributes)
    {
        std::vector<double> points;
        std::vector<Internal::Program*>::iterator p;
        forall(p, rattr->points)
        {
            //points.push_back();
        }

        //attr.push_back(VisualAttribute(rattr->type, points));
    }

    return attr;
}
