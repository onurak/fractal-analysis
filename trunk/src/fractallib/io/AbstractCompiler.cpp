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

#include "AbstractCompiler.h"
#include <math.h>
#include "../Exceptions.h"

using namespace FL::Compilers;

LexicalAnalyser::LexicalAnalyser()
{
    m_c = 0;
    m_oneLineComment = 0;
    m_lex = LEX_EOI;
    m_name = "";
    m_number = 0;
}

LexicalAnalyser::~LexicalAnalyser()
{
}

void LexicalAnalyser::useIter(
        std::string::const_iterator begin,
        std::string::const_iterator end)
{
    this->begin = this->iter = begin;
    this->end = end;
    m_pos.set(0, 0, 0);
    m_c = *iter;
}

Lexeme LexicalAnalyser::gl()
{
    return igl();
}

std::string LexicalAnalyser::getRaw(const std::string &overs)
{
    std::string result;
    while (!in_overs(m_c, overs) && iter != end)
    {
        if (m_c == m_oneLineComment)
            while (m_c != '\n' && iter != end)
                gc();
        result += m_c;
        gc();
    }
    return result;
}

int LexicalAnalyser::lookForwardFor(char symbol)
{
    int dist = -1;
    std::string::const_iterator i = iter;
    while (i != end && *i != symbol)
        ++i, ++dist;
    return i != end ? dist : -1;
}

char LexicalAnalyser::gc()
{
    ++iter;
    m_pos.linear += 1;
    if (*iter != '\n')
        m_pos.column += 1;
    else
    {
        m_pos.column = 0;
        m_pos.line += 1;
    }
    return m_c = (iter != end) ? *iter : LEX_EOI;
}

bool LexicalAnalyser::in_overs(char c, const std::string &overs)
{
    return find_(c, overs);
}


bool LexicalAnalyser::p_name()
{
    if (isalpha(m_c))
    {
        m_name = m_c;
        while (isalnum(gc()))
            m_name += m_c;
        m_lex = LEX_NAME;
        return true;
    }
    else
        return false;
}


bool LexicalAnalyser::p_int()
{
    int sign = 1;
    if (m_c == '+')
    {
        //sign = 1;
        gc();
    }
    else if (m_c == '-')
    {
        sign = -1;
        gc();
    }

    int result = 0;
    if (!isdigit(m_c))
        return false;
    while (isdigit(m_c))
    {
        result = result * 10 + (int)(m_c - '0');
        gc();
    }

    m_lex = LEX_INTEGER;
    m_number = result * sign;
    return true;
}

bool LexicalAnalyser::p_uint()
{
    if (!isdigit(m_c))
        return false;
    int result = 0;
    while (isdigit(m_c))
    {
        result = result * 10 + (int)(m_c - '0');
        gc();
    }
    m_number = result;
    return true;
}

bool LexicalAnalyser::p_float()
{
    int sign = 1;
    if (m_c == '+')
    {
        //sign = 1;
        gc();
    }
    else if (m_c == '-')
    {
        sign = -1;
        gc();
    }

    double base = 0;
    bool haveBase = true;
    if (m_c == '.')
        haveBase = false;
    else if (!isdigit(m_c))
        return false;
    for (; isdigit(m_c); gc())
        base = base * 10 + double(m_c - '0');

    double rest = 0;
    if (m_c == '.')
    {
        gc();
        if (!haveBase && !isdigit(m_c))
            return false;
        int restLen;
        for (restLen = 1; isdigit(m_c); gc(), ++restLen)
            rest = rest * 10 + (double)(m_c - '0');
        rest = rest * pow(10, -restLen);
    }
    else if (m_c == 'e' || m_c == 'E')
    {
        gc();
        int restSign = 1;
        if (m_c == '+') gc();
        else if (m_c == '-') { restSign = -1; gc(); }

        if (!isdigit(m_c))
            return false;
        for (; isdigit(m_c); gc())
            rest = rest * 10 + double(m_c - '0');
        if (restSign == -1)
            rest = -rest;
        rest = pow(10, rest);
    }

    if (sign == 1)
        m_number = +base + rest;
    else
        m_number = -base + rest;
    m_lex = LEX_FLOAT;
    return true;
}

bool LexicalAnalyser::p_number()
{
    int sign = 1;
    if (m_c == '+')
    {
        //sign = 1;
        gc();
    }
    else if (m_c == '-')
    {
        sign = -1;
        gc();
    }

    bool isFloat = false;
    double base = 0;
    bool haveBase = true;
    if (m_c == '.')
    {
        haveBase = false;
        isFloat = true;
    }
    else if (!isdigit(m_c))
        return false;
    for (; isdigit(m_c); gc())
        base = base * 10 + double(m_c - '0');

    double rest = 0;
    if (m_c == '.')
    {
        gc();
        if (!haveBase && !isdigit(m_c))
            return false;
        int restLen;
        for (restLen = 1; isdigit(m_c); gc(), ++restLen)
            rest = rest * 10 + (double)(m_c - '0');
        rest = rest * pow(10, -restLen);
        isFloat = true;
    }
    else if (m_c == 'e' || m_c == 'E')
    {
        gc();
        int restSign = 1;
        if (m_c == '+') gc();
        else if (m_c == '-') { restSign = -1; gc(); }

        if (!isdigit(m_c))
            return false;
        for (; isdigit(m_c); gc())
            rest = rest * 10 + double(m_c - '0');
        if (restSign == -1)
            rest = -rest;
        rest = pow(10, rest);
        isFloat = true;
    }

    if (sign == 1)
        m_number = +base + rest;
    else
        m_number = -base + rest;
    m_lex = isFloat ? LEX_FLOAT : LEX_INTEGER;
    return true;
}


bool SyntaxAnalyser::analyse(std::string::const_iterator begin,
                             std::string::const_iterator end)
{
    m_lexical->useIter(begin, end);
    gl();
    try
    {
        S();
    } catch (ParseException &e)
    {
        m_lastErrorDescription = e.msg();
        return false;
    }
    if (lex != LEX_EOI)
    {
        m_lastErrorDescription = std::string("Unexpected symbol: ") + m_lexical->c();
        return false;
    }
    return true;
}
