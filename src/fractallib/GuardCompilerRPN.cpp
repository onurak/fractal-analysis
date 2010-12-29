#include "GuardCompilerRPN.h"
#include "tools/spirit.h"
#include <fstream>

using namespace FL;
using namespace FL::Patterns::RPN;


/********** RPN COPMPILER ***********/

/* Grammar using recursive descend */
namespace GuardGrammar
{
    using namespace FL::Compilers;

    GuardStructureRPN *m_program;

    inline void addInstruction(int type, int concr)
    {
        m_program->code.push_back(Instruction(type, concr));
    }

    inline void addOperation(int concr)
    {
        m_program->code.push_back(Instruction(OPERATION, concr));
    }

    inline int addOperand(const GVariant &op)
    {
        m_program->vars.push_back(op);
        int opIndex = m_program->vars.size()-1;
        m_program->code.push_back(Instruction(OPERAND, opIndex));
        return opIndex;
    }

    inline void addCall(Predicates::Predicate *p, int argCount)
    {
        int predicateIndex;
        std::vector<Predicates::Predicate*>::const_iterator
             it = std::find(m_program->predicates.begin(),
                            m_program->predicates.end(),
                            p);
        if (it == m_program->predicates.end())
        {
            m_program->predicates.push_back(p);
            predicateIndex = m_program->predicates.size()-1;
        } else
            predicateIndex = it - m_program->predicates.begin();
        addOperand(argCount);
        m_program->code.push_back(Instruction(PREDICATE, predicateIndex));
    }

    //**** LEXEMES *****

    const Lexeme LEX_TRUE = 1;
    const Lexeme LEX_FALSE = 2;
    const Lexeme LEX_PLUS = 3;
    const Lexeme LEX_MINUS = 4;
    const Lexeme LEX_MULT = 5;
    const Lexeme LEX_DIV = 6;
    const Lexeme LEX_AND = 7;
    const Lexeme LEX_OR = 8;
    const Lexeme LEX_NOT = 9;
    const Lexeme LEX_LESS = 10;
    const Lexeme LEX_GRTR = 11;
    const Lexeme LEX_LEQ = 12;
    const Lexeme LEX_GEQ = 13;
    const Lexeme LEX_EQ = 14;
    const Lexeme LEX_NEQ = 15;
    const Lexeme LEX_LBRACE = 16;
    const Lexeme LEX_RBRACE = 17;
    const Lexeme LEX_COMMA = 18;
    const Lexeme LEX_SEMICOLON = 19;
    const Lexeme LEX_IF = 20;
    const Lexeme LEX_THEN = 21;
    const Lexeme LEX_ELSE = 22;
    const Lexeme LEX_INDEXED_NAME = 23;

    int LEX2OP(Lexeme lex)
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

    //****** LEXICAL ANALYSER *******
    /*! \class GuardLexicalAnalyser
      */
    class GuardLexicalAnalyser : public LexicalAnalyser
    {
    public:
        GuardLexicalAnalyser()
        {
            m_oneLineComment = '#';
            // fill reserved word list
            reserved_words["true"]    = LEX_TRUE;
            reserved_words["false"]   = LEX_FALSE;
            reserved_words["if"]      = LEX_IF;
            reserved_words["then"]    = LEX_THEN;
            reserved_words["else"]    = LEX_ELSE;
            reserved_words["not"]     = LEX_NOT;
            reserved_words["or"]      = LEX_OR;
            reserved_words["and"]     = LEX_AND;
        }

        virtual Lexeme igl() throw (ParseException)
        {
            Lexeme prev = m_lex;
            m_lex = LEX_NONE;

            while (m_lex == LEX_NONE)
            {
                // eof
                if (*iter == *end)
                {
                    m_lex = LEX_EOI;
                }
                // spaces
                else if (isspace(m_c) && prev != LEX_INDEXED_NAME)
                {
                     gc();
                }
                // names
                else if (isalpha(m_c))
                {
                    m_name = m_c;
                    while ( isalnum(gc()) )
                        m_name += m_c;
                    // reserved word
                    std::map<std::string, Lexeme>::const_iterator
                            it_rw = reserved_words.find(m_name);
                    if (it_rw != reserved_words.end())
                        m_lex = it_rw->second;
                    // indexed name
                    else if (m_c == '_')
                    {
                        m_lex = LEX_INDEXED_NAME;
                        gc();
                    }
                    // name
                    else
                        m_lex = LEX_NAME;
                }
                // numbers
                else if (isdigit(m_c))
                {
                    m_name = m_c;
                    while ( isdigit(gc()) )
                        m_name += m_c;
                    if (m_c == '.')
                    {
                        m_name += m_c;
                        while (isdigit(gc()))
                            m_name += m_c;
                        m_number = atof(m_name.c_str());
                        m_lex = LEX_FLOAT;
                    }
                    else
                    {
                        m_number = double(atoi(m_name.c_str()));
                        m_lex = LEX_INTEGER;
                    }


                }
                // rel ==
                else if (m_c == '=')
                {
                    gc();
                    if (m_c == '=')
                    {
                        m_lex = LEX_EQ;
                        gc();
                    }
                    else
                        m_lex = LEX_UNKNOWN;
                }
                // rel !=
                else if (m_c == '!')
                {
                    gc();
                    if (m_c == '=')
                    {
                        m_lex = LEX_NEQ;
                        gc();
                    }
                    else
                        m_lex = LEX_UNKNOWN;
                }
                // rel < and <=
                else if (m_c == '<')
                {
                    gc();
                    if (m_c == '=')
                    {
                        m_lex = LEX_LEQ;
                        gc();
                    }
                    else
                        m_lex = LEX_LESS;
                }
                // rel > and >=
                else if (m_c == '>')
                {
                    gc();
                    if (m_c == '=')
                    {
                        m_lex = LEX_GEQ;
                        gc();
                    }
                    else
                        m_lex = LEX_GRTR;
                }
                // math +
                else if (m_c == '+')
                {
                    m_lex = LEX_PLUS;
                    gc();
                }
                // math -
                else if (m_c == '-')
                {
                    m_lex = LEX_MINUS;
                    gc();
                }
                // math * or wildcard indexed name
                else if (m_c == '*')
                {
                    gc();
                    if (m_c != '_')
                        m_lex = LEX_MULT;
                    else
                    {
                        m_name = "*";
                        m_lex = LEX_INDEXED_NAME;
                        gc();
                    }
                }
                // math /
                else if (m_c == '/')
                {
                    m_lex = LEX_DIV;
                    gc();
                }
                // delimeters
                else if (m_c == '(')
                {
                    m_lex = LEX_LBRACE;
                    gc();
                }
                else if (m_c == ')')
                {
                    m_lex = LEX_RBRACE;
                    gc();
                }
                else if (m_c == ',')
                {
                    m_lex = LEX_COMMA;
                    gc();
                }
                else
                    m_lex = LEX_UNKNOWN;
            }
            return m_lex;
        }

    private:
        std::map<std::string, Lexeme> reserved_words;
    };

    //****** SYNTAX ANALYSER *******
    /*! \class GuardSyntaxAnalyser

      *
      */
    class GuardSyntaxAnalyser : public SyntaxAnalyser
    {
    public:
        GuardSyntaxAnalyser()
        {
            m_lexical = new GuardLexicalAnalyser();
        }
    protected:
        virtual void S()
        {
            b_expr();
        }

        void b_expr()
        {
            b_term();
            while (lex == LEX_OR)
            {
                gl();
                b_term();
                addOperation(BOOL_OR);
            }
        }

        void b_term()
        {
            b_not();
            while (lex == LEX_AND)
            {
                gl();
                b_not();
                addOperation(BOOL_AND);
            }
        }

        void b_not()
        {
            if (lex == LEX_NOT)
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
            if (lex == LEX_TRUE)
            {
                gl();
                addOperand(true);
            }
            else if (lex == LEX_FALSE)
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
            if (lex == LEX_EQ   || lex == LEX_NEQ || lex == LEX_LESS ||
                lex == LEX_GRTR || lex == LEX_LEQ || lex == LEX_GEQ)
            {
                Lexeme prevLex = lex;
                gl();
                m_expr();
                addOperation(LEX2OP(prevLex));
            }
        }

        void m_expr()
        {
            m_term();
            while (lex == LEX_PLUS || lex == LEX_MINUS)
            {
                Lexeme prevLex = lex;
                gl();
                m_term();
                addOperation(LEX2OP(prevLex));
            }
        }

        void m_term()
        {
            m_sig_factor();
            while (lex == LEX_MULT || lex == LEX_DIV)
            {
                Lexeme prevLex = lex;
                gl();
                m_factor();
                addOperation(LEX2OP(prevLex));
            }
        }

        void m_sig_factor()
        {
            if (lex == LEX_PLUS)
            {
                gl();
                //addOperation(MATH_UNARY_PLUS); // better don't
            }
            else if (lex == LEX_MINUS)
            {
                gl();
                addOperation(MATH_UNARY_MINUS);
            }
            m_factor();
        }

        void m_factor()
        {
            if (lex == LEX_IF)
            {
                gl();
                if_statement();
            }
            else if (lex == LEX_TRUE)
            {
                gl();
                addOperand(true);
            }
            else if (lex == LEX_FALSE)
            {
                gl();
                addOperand(false);
            }
            else if (lex == LEX_FLOAT || lex == LEX_INTEGER)
            {
                addOperand(m_lexical->number());
                gl();
            }
            else if (lex == LEX_NAME)
            {
                function();
            }
            else if (lex == LEX_LBRACE)
            {
                gl();
                b_expr();
                if (lex != LEX_RBRACE)
                    throw ParseException("')' expected");
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
            if (lex != LEX_THEN)
                throw ParseException("'then' expected");
            // add "jump_if_false ELSE" dummy
            m_program->code.push_back(Instruction(GOTO_COND, -1));
            int dummyPos = m_program->code.size()-1;
            gl();
            b_expr();
            // add "jump_to END" dummy, fill previous dummy
            m_program->code.push_back(Instruction(GOTO, -1));
            m_program->code[dummyPos].concr = m_program->code.size();
            dummyPos = m_program->code.size()-1;
            if (lex == LEX_ELSE)
            {
                gl();
                b_expr();
            }
            // fill previous dummy
            m_program->code[dummyPos].concr = m_program->code.size();
        }

        void function()
        {
            if (lex != LEX_NAME)
                throw ParseException("Function name expected");
            Predicates::Predicate *p =
                    Predicates::PredicateFactory::predicateByName(m_lexical->name());
            if (p == NULL)
                throw ParseException("Unknown function: " + m_lexical->name());

            gl();
            if (lex != LEX_LBRACE)
                throw ParseException("'(' expected");
            gl();
            int argCount = 0;
            if (lex != LEX_RBRACE)
                argCount = arg();
            while (lex == LEX_COMMA)
            {
                gl();
                argCount += arg();
            }
            if (lex != LEX_RBRACE)
                throw ParseException("')' expected");
            gl();

            addCall(p, argCount);
        }

        int arg()
        {
            int argCount = 1;

            if (lex == LEX_NAME &&
                !Predicates::PredicateFactory::predicateByName(m_lexical->name()))
            {
                addOperand( UniqueNamer::id_of_name(m_lexical->name()) );
                gl();
            }
            else
            if (lex == LEX_INDEXED_NAME)
            {
                std::string indexedName = m_lexical->name();
                int indexedNo;
                gl();
                if (lex == LEX_INTEGER)
                {
                    indexedNo = m_lexical->number();
                    gl();
                }
                else if (lex == LEX_MULT)
                {
                    if (UniqueNamer::id_of_name(indexedName) == UniqueNamer::WILDCARD)
                        throw ParseException("Wildcard '*' can't be both on name and on index");
                    indexedNo = -1;
                    gl();
                }
                else throw ParseException("Number of wildcard expected");
                // push indexed parameters
                int opIndex = addOperand(
                        GVariant(FL::Predicates::IndexedName(indexedName, indexedNo)) );
                // push LOAD_NODE or LOAD_NODES instruction
                if (indexedNo == -1)
                    addInstruction(LOAD_NODES, opIndex);
                else
                    addInstruction(LOAD_NODE, opIndex);

            }
            else
                b_expr();

            /*
            if (lex == LEX_IF)
            {
                gl();
                if_statement();
            }
            else if (lex == LEX_NAME)
            {
                if ( Predicates::PredicateFactory::predicateByName(m_lexical->name()) )
                    function();
                else {
                    addOperand( UniqueNamer::id_of_name(m_lexical->name()) );
                    gl();
                }
            }
            else if (lex == LEX_TRUE || lex == LEX_FALSE)
            {
                addOperand(lex == LEX_TRUE ? true : false);
                gl();
            }
            else if (lex == LEX_FLOAT || lex == LEX_DECIMAL)
            {
                addOperand(m_lexical->number);
                gl();
            }
            else if (lex == LEX_INDEXED_NAME)
            {
                std::string indexedName = m_lexical->name;
                int indexedNo;
                gl();
                if (lex == LEX_DECIMAL)
                {
                    indexedNo = m_lexical->number;
                    gl();
                }
                else if (lex == LEX_MULT)
                {
                    indexedNo = -1;
                    gl();
                }
                else throw ParseException("Number of wildcard expected");
                // push indexed parameters
                int opIndex = addOperand(
                        GVariant(FL::Predicates::IndexedName(indexedName, indexedNo)) );
                // push LOAD_NODE or LOAD_NODES instruction
                if (indexedNo == -1)
                    addInstruction(LOAD_NODES, opIndex);
                else
                    addInstruction(LOAD_NODE, opIndex);

            }
            else
                throw ParseException("Unknown function argument");
                */
            return argCount;
        }

    };
} // namespace

bool GuardCompilerRPN::compile(const std::string &text, GuardStructure &guard)
{
    GuardGrammar::m_program = dynamic_cast<GuardStructureRPN*>(&guard);
    if (GuardGrammar::m_program == NULL)
        return false;


    GuardGrammar::GuardSyntaxAnalyser analyser;
    if (!analyser.analyse(text.begin(), text.end()))
    {
        errorPos = analyser.pos();
        errorDescription = analyser.lastErrorDescription();
        return false;
    }

    /*
    GuardGrammar::Grammar<std::string::const_iterator> g;
    errorPos = spiritParseString(text, g);
    */
    //GuardGrammar::

    return true;
}
