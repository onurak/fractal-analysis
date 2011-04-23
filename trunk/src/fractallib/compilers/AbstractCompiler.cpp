#include "AbstractCompiler.h"

using namespace FL;
using namespace FL::Compilers;

Lexeme::Lexeme(int id, int index)
    : id(id), index(index)
{

}


Input::Input(const Input &i)
    : m_line(i.m_line), m_column(i.m_column),
      m_refCount(0), m_pc(-1),m_input(i.m_input)
{
}

Input::Input(Iterator &it)
    : m_line(1), m_column(1),
      m_refCount(0), m_pc(-1), m_input(it)

{
}

void Input::addRef()
{
    m_refCount++;
}

void Input::releaseRef()
{
    if (--m_refCount == 0)
        delete this;
}

int Input::gc()
{
    m_pc = c();
    ++m_input;
    if (c() != '\n')
        m_column++;
    else
    {
        m_column = 1;
        m_line++;
    }
    return c();
}

int Input::c() const
{
    static Iterator end;
    return m_input != end ? *m_input : EOF;
}

int Input::pc() const
{
    return m_pc;
}

int Input::line() const
{
    return m_line;
}

int Input::column() const
{
    return m_column;
}

bool Input::isEoi() const
{
    return *m_input == EOF;
}

AbstractCompiler::AbstractCompiler()
    : m_lex(LEX_UNKNOWN, -1), m_lexprev(LEX_UNKNOWN, -1), m_l(m_lex.id),
      m_input(NULL), m_lastError(E_OK, -1, -1, "")
{

}

AbstractCompiler::AbstractCompiler(Input *input)
    : m_lex(LEX_UNKNOWN, -1), m_lexprev(LEX_UNKNOWN, -1), m_l(m_lex.id),
      m_input(NULL), m_lastError(E_OK, -1, -1, "")
{
    setInput(input);
}

AbstractCompiler::~AbstractCompiler()
{
    setInput(NULL);
}

void AbstractCompiler::setInput(Input *input)
{
    if (m_input)
        m_input->releaseRef();
    m_input = input;
    if (m_input)
        m_input->addRef();
}

int AbstractCompiler::c() const
{
    return m_input->c();
}

std::string AbstractCompiler::psymbol() const
{
    std::string result;
    return result = (char)c();
}

void AbstractCompiler::error(int id, const std::string &arg)
{
    throw FL::Exceptions::EParsing(id, m_input->line(), m_input->column(), arg);
}

bool AbstractCompiler::compile(const std::string &s, CompilationFlags flags)
{

    std::istringstream stream(s);
    stream >> std::noskipws;
    Input::Iterator it(stream);
    setInput(new Input(it));
    return run(flags);
}

/*
bool AbstractCompiler::parseFile(const std::string &fileName, CompilationFlags flags)
{

    std::ifstream stream(fileName.c_str());
    if (!stream.is_open())
    {
        m_lastError.set(E_INVALID_INPUT, -1, -1, fileName);
        return false;
    }
    setInput(InputIterator(stream));

    return run();
    return false;
}*/

bool AbstractCompiler::compile(Input *input, CompilationFlags flags)
{
    if (input == NULL)
        return false;
    setInput(input);
    bool result = run(flags);
    setInput(NULL);
    return result;
}

bool AbstractCompiler::isEoi() const
{
    return m_input->isEoi();
}

bool AbstractCompiler::run(CompilationFlags flags)
{
    m_symbolsTable.clear();
    bool result;
    try
    {
        if (isEoi())
            error(E_EMPTY_INPUT);
        gl();
        S();
        m_lastError.set(E_OK);
        result = true;
    }
    catch (Exceptions::EParsing &e)
    {
        m_lastError = e;
        result = false;
    }

    return result;
}

int AbstractCompiler::gc()
{
    return m_input->gc();
}

void AbstractCompiler::gl()
{
    m_lexprev = m_lex;
    m_l = LEX_UNKNOWN;
    m_lex.index = -1;
    vgl();
}

void AbstractCompiler::vgl()
{
    while (m_l == LEX_UNKNOWN && m_l != LEX_EOI)
    {
        /////////////////////////////////
        // LEX_EOI
        /////////////////////////////////
        if (c() == EOF)
        {
            m_l = LEX_EOI; continue;
        }

        /////////////////////////////////
        // Whitespace
        /////////////////////////////////
        if (isspace(c()))
        {
            gc(); continue;
        }

        /////////////////////////////////
        // LEX_NAME
        /////////////////////////////////
        if (wantLexeme(LEX_NAME) && isalpha(c()))
        {
            std::string name;
            do
            {
                name += c();
                gc();
            } while (isalnum(c()));

            std::map<std::string, LexemeId>::const_iterator it =
                m_reservedWords.find(name);
            if (it == m_reservedWords.end())
                m_l = LEX_NAME;
            else
                m_l = LEX_RESWORD;
            m_lex.index = addSymbol(name);
            continue;
        }

        /////////////////////////////////
        // LEX_INTEGER, LEX_FLOAT
        /////////////////////////////////
        if (
            (wantLexeme(LEX_INTEGER) && isdigit(c())) ||
            (wantLexeme(LEX_FLOAT) && (isdigit(c()) || c() == '.'))
            )
        {
            if (wantLexeme(LEX_FLOAT))
            {
                std::string intPart, floatPart;
                bool isFloat = false;
                do
                {
                    intPart += c();
                    gc();
                } while (isdigit(c()));

                if (c() == '.')
                {
                    isFloat = true; gc();
                }

                while (isdigit(c()))
                {
                    floatPart += c();
                    gc();
                }


                if (isFloat || !wantLexeme(LEX_INTEGER))
                {
                    m_l = LEX_FLOAT;
                    std::string value = intPart + '.' + floatPart;
                    m_lex.index = addSymbol(atof(value.c_str()));
                }
                else
                {
                    m_l = LEX_INTEGER;
                    m_lex.index = addSymbol(atoi(intPart.c_str()));
                }
                continue;
            }
            else
            {
                std::string intPart;
                do
                {
                    intPart += c();
                    gc();
                } while (isdigit(c()));
                m_l = LEX_INTEGER;
                m_lex.index = addSymbol(atoi(intPart.c_str()));
            }
            continue;
        }

        /////////////////////////////////
        // LEX_PLUS
        /////////////////////////////////
        if (wantLexeme(LEX_PLUS) && c() == '+')
        {
            m_l = LEX_PLUS;
            gc();
            continue;
        }

        /////////////////////////////////
        // LEX_MINUS
        /////////////////////////////////
        if (wantLexeme(LEX_MINUS) && c() == '-')
        {
            m_l = LEX_MINUS;
            gc();
            continue;
        }

        /////////////////////////////////
        // LEX_DIV
        /////////////////////////////////
        if (wantLexeme(LEX_DIV) && c() == '/')
        {
            m_l = LEX_DIV;
            gc();
            continue;
        }

        /////////////////////////////////
        // LEX_MULT
        /////////////////////////////////
        if (wantLexeme(LEX_MULT) && c() == '*')
        {
            m_l = LEX_MULT;
            gc();
            continue;
        }

        /////////////////////////////////
        // LEX_QUESTION
        /////////////////////////////////
        if (wantLexeme(LEX_QUESTION) && c() == '?')
        {
            m_l = LEX_QUESTION;
            gc();
            continue;
        }


        /////////////////////////////////
        // LEX_ASSIGN, LEX_EQ
        /////////////////////////////////
        if (wantLexeme(LEX_ASSIGN) && c() == '=')
        {
            gc();
            if (wantLexeme(LEX_EQ) && c() == '=')
            {
                m_l = LEX_EQ;
                gc();
                continue;
            }
            else
            {
                m_l = LEX_ASSIGN;
                gc();
                continue;
            }
        }

        if (wantLexeme(LEX_EQ) && c() == '=')
        {
            m_l = LEX_EQ;
            gc();
            continue;
        }

        /////////////////////////////////
        // LEX_NEQ, LEX_NOT
        /////////////////////////////////
        if (wantLexeme(LEX_NEQ) && c() == '!')
        {
            gc();
            if (wantLexeme(LEX_NOT) && c() != '=')
            {
                m_l = LEX_NOT;
                continue;
            }
            else
            {
                if (c() == '=')
                {
                    m_l = LEX_NEQ;
                    continue;
                }
            }

            continue;
        }

        /////////////////////////////////
        // LEX_GRTR, LEX_GEQ
        /////////////////////////////////
        if (wantLexeme(LEX_GRTR) && c() == '>')
        {
            gc();
            if (wantLexeme(LEX_GEQ) && c() == '=')
            {
                m_l = LEX_GEQ;
                gc();
            }
            else
                m_l = LEX_GRTR;
            continue;
        }

        /////////////////////////////////
        // LEX_LESS, LEX_LEQ
        /////////////////////////////////
        if (wantLexeme(LEX_LESS) && c() == '<')
        {
            gc();
            if (wantLexeme(LEX_LEQ) && c() == '=')
            {
                m_l = LEX_LEQ;
                gc();
            }
            else
                m_l = LEX_LESS;
            continue;
        }

        /////////////////////////////////
        // LEX_LPAREN
        /////////////////////////////////
        if (wantLexeme(LEX_LPAREN) && c() == '(')
        {
            m_l = LEX_LPAREN;
            gc();
            continue;
        }

        /////////////////////////////////
        // LEX_RPAREN
        /////////////////////////////////
        if (wantLexeme(LEX_RPAREN) && c() == ')')
        {
            m_l = LEX_RPAREN;
            gc();
            continue;
        }

        /////////////////////////////////
        // LEX_COMMA
        /////////////////////////////////
        if (wantLexeme(LEX_COMMA) && c() == ',')
        {
            m_l = LEX_COMMA;
            gc();
            continue;
        }

        /////////////////////////////////
        // LEX_DOT
        /////////////////////////////////
        if (wantLexeme(LEX_DOT) && c() == '.')
        {
            m_l = LEX_DOT;
            gc();
            continue;
        }

        /////////////////////////////////
        // LEX_SEMICOLON
        /////////////////////////////////
        if (wantLexeme(LEX_SEMICOLON) && c() == ';')
        {
            m_l = LEX_SEMICOLON;
            gc();
            continue;
        }

        /////////////////////////////////
        // LEX_COLON
        /////////////////////////////////
        if (wantLexeme(LEX_COLON) && c() == ':')
        {
            m_l = LEX_COLON;
            gc();
            continue;
        }

        /////////////////////////////////
        // LEX_OR
        /////////////////////////////////
        if (wantLexeme(LEX_OR) && c() == '|')
        {
            gc();
            if (c() == '|')
            {
                m_l = LEX_OR;
                gc();
            }
            continue;
        }

        /////////////////////////////////
        // LEX_AND
        /////////////////////////////////
        if (wantLexeme(LEX_AND) && c() == '&')
        {
            gc();
            if (c() == '&')
            {
                m_l = LEX_AND;
                gc();
            }
            continue;
        }

        /////////////////////////////////
        // LEX_UNDERSCORE
        /////////////////////////////////
        if (wantLexeme(LEX_UNDERSCORE) && c() == '_')
        {
            m_l = LEX_UNDERSCORE;
            gc();
            continue;
        }

        /////////////////////////////////
        // LEX_AT
        /////////////////////////////////
        if (wantLexeme(LEX_AT) && c() == '@')
        {
            m_l = LEX_AT;
            gc();
            continue;
        }

        /////////////////////////////////
        // LEX_COMMENT
        /////////////////////////////////
        if (wantLexeme(LEX_COMMENT) && isNextSymbols(m_oneLineCommentBegin))
        {

        }

        /////////////////////////////////
        // LEX_NONE
        /////////////////////////////////
        m_lex.id = LEX_NONE;
    }
}

bool AbstractCompiler::isNextSymbols(const std::string &s)
{
    return false;
}

int AbstractCompiler::addSymbol(const GVariant& v)
{
    std::vector<GVariant>::const_iterator i =
            std::find(m_symbolsTable.begin(), m_symbolsTable.end(), v);
    if (i == m_symbolsTable.end())
    {
        m_symbolsTable.push_back(v);
        return m_symbolsTable.size() - 1;
    }
    else
    {
        return i - m_symbolsTable.begin();
    }
}

bool AbstractCompiler::wantLexeme(LexemeId lex)
{
    return m_lexemeFilter.find((int)lex) != m_lexemeFilter.end() &&
           m_lexemeFilter[(int)lex];
}

void AbstractCompiler::filterLexeme(LexemeId lex, bool filter)
{
    m_lexemeFilter[(int)lex] = filter;
}

void AbstractCompiler::runSubcompiler(AbstractCompiler &subcompiler)
{
    if (!subcompiler.compile(m_input, cfPartialInput))
        throw subcompiler.lastError();
}

const Exceptions::EParsing& AbstractCompiler::lastError() const
{
    return m_lastError;
}
