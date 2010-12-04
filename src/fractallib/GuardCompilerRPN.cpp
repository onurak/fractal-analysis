#include "GuardCompilerRPN.h"
#include "tools/spirit.h"
#include <fstream>

using namespace FL;
using namespace FL::Patterns::RPN;

class ParseException
{
public:
    ParseException(int errNo, const std::string &description, int ln = -1, int col = -1)
        : m_errNo(errNo), m_description(description), m_ln(ln), m_col(col)
    {

    }
    int errNo() const { return m_errNo; }
    const std::string& description() const { return m_description; }
    int line() const { return m_ln; }
    int column() const { return m_col; }
private:
    int m_errNo;
    std::string m_description;
    int m_ln;
    int m_col;
};

/********** RPN COPMPILER ***********/

/* Grammar using recursive descend */
namespace GuardGrammar
{
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
    enum Lexeme
    {
        LEX_NAME,
        LEX_INDEXED_NAME,
        LEX_TRUE,
        LEX_FALSE,
        LEX_FLOAT,
        LEX_DECIMAL,
        LEX_PLUS,
        LEX_MINUS,
        LEX_MULT,
        LEX_DIV,
        LEX_AND,
        LEX_OR,
        LEX_NOT,
        LEX_LESS,
        LEX_GRTR,
        LEX_LEQ,
        LEX_GEQ,
        LEX_EQ,
        LEX_NEQ,
        LEX_LBRACE,
        LEX_RBRACE,
        LEX_COMMA,
        LEX_SEMICOLON,
        LEX_IF,
        LEX_THEN,
        LEX_ELSE,
        LEX_EOI,
        LEX_NONE,
        LEX_UNKNOWN
    };

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
    /*! \class LexicalAnalyser
      */
    class LexicalAnalyser
    {
    public:
        LexicalAnalyser()
        {
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

        ~LexicalAnalyser() { close(); }

        bool useIter(std::string::const_iterator &iter, std::string::const_iterator &end)
        {
            close();
            this->iter = &iter;
            this->end  = &end;
            if (this->iter && this->end)
            {
                m_c = iter != end ? *iter : LEX_EOI;
                return true;
            }
            else
            {
                c = LEX_NONE;
                return false;
            }
        }

        void close()
        {
            c = LEX_NONE;
        }

        Lexeme gl()
        {
            Lexeme prev = c;
            c = LEX_NONE;

            while (c == LEX_NONE)
            {
                // eof
                if (*iter == *end)
                {
                    c = LEX_EOI;
                }
                // spaces
                else if (isspace(m_c) && prev != LEX_INDEXED_NAME)
                {
                     gc();
                }
                // names
                else if (isalpha(m_c))
                {
                    name = m_c;
                    while ( isalnum(gc()) )
                        name += m_c;
                    // reserved word
                    std::map<std::string, Lexeme>::const_iterator
                            it_rw = reserved_words.find(name);
                    if (it_rw != reserved_words.end())
                        c = it_rw->second;
                    // indexed name
                    else if (m_c == '_')
                    {
                        c = LEX_INDEXED_NAME;
                        gc();
                    }
                    // name
                    else
                        c = LEX_NAME;
                }
                // numbers
                else if (isdigit(m_c))
                {
                    name = m_c;
                    while ( isdigit(gc()) )
                        name += m_c;
                    if (m_c == '.')
                    {
                        name += m_c;
                        while (isdigit(gc()))
                            name += m_c;
                        number = atof(name.c_str());
                        c = LEX_FLOAT;
                    }
                    else
                    {
                        number = double(atoi(name.c_str()));
                        c = LEX_DECIMAL;
                    }


                }
                // rel ==
                else if (m_c == '=')
                {
                    gc();
                    if (m_c == '=')
                    {
                        c = LEX_EQ;
                        gc();
                    }
                    else
                        c = LEX_UNKNOWN;
                }
                // rel !=
                else if (m_c == '!')
                {
                    gc();
                    if (m_c == '=')
                    {
                        c = LEX_NEQ;
                        gc();
                    }
                    else
                        c = LEX_UNKNOWN;
                }
                // rel < and <=
                else if (m_c == '<')
                {
                    gc();
                    if (m_c == '=')
                    {
                        c = LEX_LEQ;
                        gc();
                    }
                    else
                        c = LEX_LESS;
                }
                // rel > and >=
                else if (m_c == '>')
                {
                    gc();
                    if (m_c == '=')
                    {
                        c = LEX_GEQ;
                        gc();
                    }
                    else
                        c = LEX_GRTR;
                }
                // math +
                else if (m_c == '+')
                {
                    c = LEX_PLUS;
                    gc();
                }
                // math -
                else if (m_c == '-')
                {
                    c = LEX_MINUS;
                    gc();
                }
                // math *
                else if (m_c == '*')
                {
                    c = LEX_MULT;
                    gc();
                }
                // math /
                else if (m_c == '/')
                {
                    c = LEX_DIV;
                    gc();
                }
                // delimeters
                else if (m_c == '(')
                {
                    c = LEX_LBRACE;
                    gc();
                }
                else if (m_c == ')')
                {
                    c = LEX_RBRACE;
                    gc();
                }
                else if (m_c == ',')
                {
                    c = LEX_COMMA;
                    gc();
                }
                else
                    c = LEX_UNKNOWN;
            }
            return c;
        }

        Lexeme c;         //!< Current lexeme
        std::string name; //!< Current name (if c is LEX_NAME or LEX_INDEXED)
        double number;    //!< Current double (if c is LEX_FLOAT or LEX_DECIMAL)
    private:
        std::string::const_iterator *iter, *end;

        std::map<std::string, Lexeme> reserved_words;

        char m_c;

        char gc()
        {
            ++(*iter);
            return m_c = (*iter != *end ? **iter : LEX_EOI);
        }
    };

    //****** SYNTAX ANALYSER *******
    /*! \class SyntaxAnalyser

      *
      */
    class SyntaxAnalyser
    {
    public:
        int analyse(std::string::const_iterator &i, std::string::const_iterator &end)
        {
            if (!m_lexical.useIter(i, end))
                return 0;
            gl();
            try {
                S();
            } catch (ParseException &e) {
                return e.errNo();
            }

            if (lex == LEX_EOI)
                return -1;
            return 0;
        }

        void S()
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
            else if (lex == LEX_FLOAT || lex == LEX_DECIMAL)
            {
                addOperand(m_lexical.number);
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
                    throw ParseException(1, "')' expected");
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
                throw ParseException(1, "'then' expected");
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
                throw ParseException(1, "Function name expected");
            Predicates::Predicate *p =
                    Predicates::PredicateFactory::predicateByName(m_lexical.name);
            if (p == NULL)
                throw ParseException(1, "Unknown function: " + m_lexical.name);

            gl();
            if (lex != LEX_LBRACE)
                throw ParseException(1, "'(' expected");
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
                throw ParseException(1, "')' expected");
            gl();

            addCall(p, argCount);
        }

        int arg()
        {
            int argCount = 1;
            if (lex == LEX_IF)
            {
                gl();
                if_statement();
            }
            else if (lex == LEX_NAME)
            {
                if ( Predicates::PredicateFactory::predicateByName(m_lexical.name) )
                    function();
                else {
                    addOperand( UniqueNamer::id_of_name(m_lexical.name) );
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
                addOperand(m_lexical.number);
                gl();
            }
            else if (lex == LEX_INDEXED_NAME)
            {
                std::string indexedName = m_lexical.name;
                int indexedNo;
                gl();
                if (lex == LEX_DECIMAL)
                {
                    indexedNo = m_lexical.number;
                    gl();
                }
                else if (lex == LEX_MULT)
                {
                    indexedNo = -1;
                    gl();
                }
                else throw ParseException(1, "Number of wildcard expected");
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
                throw ParseException(1, "Unknown function argument");
            return argCount;
        }

    private:
        LexicalAnalyser m_lexical;
        Lexeme lex;
        Lexeme gl()
        {
            return lex = m_lexical.gl();
        }
    };
} // namespace

bool GuardCompilerRPN::compile(const std::string &text, GuardStructure &guard)
{
    GuardGrammar::m_program = dynamic_cast<GuardStructureRPN*>(&guard);
    if (GuardGrammar::m_program == NULL)
        return false;

    std::string::const_iterator i = text.begin(), end = text.end();
    int errorPos = GuardGrammar::SyntaxAnalyser().analyse(i, end);

    /*
    GuardGrammar::Grammar<std::string::const_iterator> g;
    errorPos = spiritParseString(text, g);
    */
    //GuardGrammar::

    return errorPos == -1;
}
