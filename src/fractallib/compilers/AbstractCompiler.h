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

#ifndef ABSTRACTCOMPILER_H
#define ABSTRACTCOMPILER_H

#include "../Common.h"
#include "../exceptions/EException.h"
#include "../../G/GVariant.h"
#include <iterator>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>


namespace FL {
    enum CompilationFlags
    {
        cfNone         = 0x0,
        cfPartialInput = 0x1
    };

    // Most common comiler's errors
    const int E_EMPTY_INPUT          = 1; //!< The input is at EOF
    const int E_INVALID_INPUT        = 2; //!< The input is invalid (e.g. not opened)
    const int E_UNEXPECTED_TOKEN     = 3; //!< Unexpected token at pos...
    const int E_EXPECTED_TOKEN       = 4; //!< Token '<t>' expected at pos...
    const int E_UNKNOWN_IDENTIFIER   = 5; //!< Unknown identifier name
    const int E_SYNTAX_ERROR         = 6; //!< Syntax error (invalid token sequence)
    const int E_INVALID_DATA         = 7; //!< Invalid arguments passed (excluding input e.g. output structure)

    /*! Get standart compilation error description */
    inline std::string errorDescription(const FL::Exceptions::EParsing &e)
    {
        std::string result;
        char buf[100];
        switch (e.id())
        {
            case FL::E_OK:
                result = "OK";
                break;
            case FL::E_EMPTY_INPUT:
                result = "Input is empty";
                break;
            case FL::E_EXPECTED_TOKEN:
                sprintf(buf, "At line %d: Expected '%s'",
                        e.line(), e.arg().c_str());
                result = buf;
                break;
            case FL::E_UNEXPECTED_TOKEN:
                sprintf(buf, "At line %d: Unexpected '%s'",
                        e.line(), e.arg().c_str());
                result = buf;
                break;
            case FL::E_SYNTAX_ERROR:
                sprintf(buf, "At line %d: Syntax error '%s'",
                        e.line(), e.arg().c_str());
                result = buf;
                break;
            case FL::E_UNKNOWN_IDENTIFIER:
                sprintf(buf, "At line %d: Unknown identifier '%s'",
                        e.line(), e.arg().c_str());
                result = buf;
                break;
        }
        return result;
    }
}

namespace FL { namespace Compilers {

typedef int LexemeId;

struct Lexeme
{
    Lexeme(int id, int index);
    int id;
    int index;
};

class Input
{
public:
    typedef std::istream_iterator<char> Iterator;
private:
    Input(const Input &i);
public:
    Input(Iterator &it);
    void addRef();
    void releaseRef();
    int c() const;
    int pc() const;
    int gc();
    int line() const;
    int column() const;
    bool isEoi() const;
private:
    int m_line;
    int m_column;
    int m_refCount;
    int m_pc;
    Iterator &m_input;

};

class AbstractCompiler
{

public:
    AbstractCompiler();

    virtual ~AbstractCompiler();

    bool compile(const std::string &s, CompilationFlags flags = cfNone);
    bool compile(Input *input, CompilationFlags flags = cfNone);

    const Exceptions::EParsing& lastError() const;
protected:
    AbstractCompiler(Input *input);
    virtual void S() = 0;
    int gc();
    void gl();
    virtual void vgl();
    int c() const;
    std::string psymbol() const;
protected:
    bool run(CompilationFlags flags);
    bool isEoi() const;
    void runSubcompiler(AbstractCompiler &subcompiler);
    void setInput(Input *input);
    void error(int id, const std::string &arg = "");
protected:
    Lexeme m_lex;
    Lexeme m_lexprev;
    LexemeId &m_l;
    Input *m_input;

    std::map<std::string, LexemeId> m_reservedWords;
    std::string m_commentBegin;
    std::string m_commentEnd;
    std::string m_oneLineCommentBegin;

    std::map<LexemeId, bool> m_lexemeFilter;

    std::vector<GVariant> m_symbolsTable;
    int addSymbol(const GVariant& v);
    bool wantLexeme(LexemeId lex);
    void filterLexeme(LexemeId lex, bool filter);
    bool isNextSymbols(const std::string &s);
private:
    Exceptions::EParsing m_lastError;

public:
    // Standart lexemes
    static const LexemeId LEX_EOI             = 0;
    static const LexemeId LEX_UNKNOWN         = -1;
    static const LexemeId LEX_NONE            = -2;
    static const LexemeId LEX_NAME            = -3;
    static const LexemeId LEX_RESWORD         = -4;
    static const LexemeId LEX_INTEGER         = -5;
    static const LexemeId LEX_FLOAT           = -6;
    static const LexemeId LEX_PLUS            = -7;
    static const LexemeId LEX_MINUS           = -8;
    static const LexemeId LEX_DIV             = -9;
    static const LexemeId LEX_MULT            = -10;
    static const LexemeId LEX_EQ              = -11;
    static const LexemeId LEX_NEQ             = -12;
    static const LexemeId LEX_GRTR            = -13;
    static const LexemeId LEX_LESS            = -14;
    static const LexemeId LEX_GEQ             = -15;
    static const LexemeId LEX_LEQ             = -16;
    static const LexemeId LEX_LPAREN          = -17;
    static const LexemeId LEX_RPAREN          = -18;
    static const LexemeId LEX_COMMA           = -19;
    static const LexemeId LEX_DOT             = -20;
    static const LexemeId LEX_SEMICOLON       = -21;
    static const LexemeId LEX_COLON           = -22;
    static const LexemeId LEX_ASSIGN          = -23;
    static const LexemeId LEX_COMMENT         = -24;
    static const LexemeId LEX_UNDERSCORE      = -25;
    static const LexemeId LEX_NOT             = -26;
    static const LexemeId LEX_OR              = -27;
    static const LexemeId LEX_AND             = -28;
    static const LexemeId LEX_AT              = -29;
    static const LexemeId LEX_QUESTION        = -30;

};

}} // namespaces

#endif // ABSTRACTCOMPILER_H
