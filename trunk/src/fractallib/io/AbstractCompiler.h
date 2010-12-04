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

#ifndef ABSTRACTCOMPILER_H
#define ABSTRACTCOMPILER_H

#include <string>

namespace FL { namespace Compilers {

typedef int Lexeme;

// Some common lexemes
const Lexeme LEX_EOI        = 0;
const Lexeme LEX_UNKNOWN    = -1;
const Lexeme LEX_NONE       = -2;
const Lexeme LEX_NAME       = -3;
const Lexeme LEX_FLOAT      = -4;
const Lexeme LEX_INTEGER    = -5;

class ParseException
{
public:
    ParseException(const std::string &msg = ""): m_msg(msg) {}
    const std::string& msg() const { return m_msg; }
private:
    std::string m_msg;
};


class LexicalAnalyser
{
public:    

    // Position in file
    struct Position
    {
        Position(): line(0), column(0), linear(0) {}
        void set(int line, int column, int linear)
        {
            this->line = line; this->column = column; this->linear = linear;
        }
        int line;
        int column;
        int linear;
    };
public:
    //! Default constructor
    LexicalAnalyser();

    //! Destructor
    virtual ~LexicalAnalyser();

    //! Parse method
    void useIter(std::string::const_iterator begin,
                 std::string::const_iterator end);

    //! Get next lexeme
    Lexeme gl();

    Position pos() const { return m_pos; }

    Lexeme lex() const { return m_lex; }
    const std::string& name() const { return m_name; }
    double number() const { return m_number; }

    //! Get raw string data to nearest symbol from overs or up to end of string
    std::string getRaw(const std::string &overs);

    //! Look forward and check if there is specified symbol in input stream.
    /*! Do not checking current symbol.
      * \return Distance to symbol (zero means next gc() will return it) or -1
      */
    int lookForwardFor(char symbol);

    //! Get last error description
    const std::string & lastErrorDescription() const { return m_lastErrorMsg; }
protected:
    virtual Lexeme igl() throw (ParseException) = 0;

    //! Get next character
    /*! If last of input reached or no input provided than it will return LEX_EOI
      */
    char gc();

    //! Return true if c is one of the symbols in overs
    bool in_overs(char c, const std::string &overs);

    //// PREDEFINED COMPILERS ////

    bool p_name();        //!< Try parse name
    bool p_int();         //!< Try parse integer
    bool p_uint();        //!< Try parse integer without sign (e.g. not "+123" or "-123", only "123")
    bool p_float();       //!< Try parse float
    bool p_number();      //!< Try parse any number

    //// DATA ////
    char m_c;           //!< Current character
    Lexeme m_lex;       //!< Current lexeme
    std::string m_name; //!< Current name (if c is LEX_NAME)
    double m_number;    //!< Current double (if c is LEX_FLOAT or LEX_DECIMAL)
    char m_oneLineComment; //!< Begining of one line comment
    std::string::const_iterator iter, begin, end;
private:
    Position m_pos;
    std::string m_lastErrorMsg;
};

class SyntaxAnalyser
{
public:
    SyntaxAnalyser()
    {
        m_lexical = NULL;
        lex = LEX_UNKNOWN;
    }

    virtual ~SyntaxAnalyser()
    {
        if (m_lexical)
            delete m_lexical;
    }

    bool analyse(std::string::const_iterator begin, std::string::const_iterator end);

    LexicalAnalyser::Position pos() const
    {
        return m_lexical ? m_lexical->pos() : LexicalAnalyser::Position();
    }
    const std::string& lastErrorDescription() const { return m_lastErrorDescription; }
protected:
    virtual void S() = 0;

    LexicalAnalyser *m_lexical;
    Lexeme lex;
    Lexeme gl()
    {
        return m_lexical ? lex = m_lexical->gl() : LEX_UNKNOWN;
    }
    std::string m_lastErrorDescription;
private:

};


}} // namespaces

#endif // ABSTRACTCOMPILER_H
