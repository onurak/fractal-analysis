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

#include "FilePAT.h"
#include "../compilers/AbstractCompiler.h"
#include "../patterns/standard/EbnfRpnPattern.h"

using namespace FL::Patterns;
using namespace FL::IO;

namespace FL { namespace IO { namespace Internal {

using namespace FL::Compilers;
using namespace FL::Exceptions;

class PatCompiler : public AbstractCompiler
{
public:
    static const int LEX_RAW                  = 5;
    static const int LEX_SECTION_PATTERNS     = 10;
    static const int LEX_SECTION_SYNONYMS     = 20;

public:
    PatCompiler(PatternsSet *patterns)
        : m_patterns(patterns)
    {
        m_reservedWords["PATTERNS"] = LEX_SECTION_PATTERNS;
        m_reservedWords["SYNONYMS"] = LEX_SECTION_SYNONYMS;
    }
protected:
    enum Section
    {
        PATTERNS,
        SYNONYMS
    };

    Section m_section;

    virtual void vgl()
    {
        while (m_l == LEX_UNKNOWN)
        {
            if (c() == EOF)
            {
                m_l = LEX_EOI;
            }
            else if (isspace(c()))
            {
                gc();
            }
            else if (isalnum(c()))
            {
                std::string s;
                do {   s += c(); gc();  }
                while (!isEoi() && isalnum(c()));

                if (s == "PATTERNS")
                {
                    m_l = LEX_SECTION_PATTERNS;
                    continue;
                }

                if (s == "SYNONYMS")
                {
                    m_l = LEX_SECTION_SYNONYMS;
                    continue;
                }

                if (m_section == PATTERNS)
                {
                    do { s += c(); gc(); }
                    while (!isEoi() && c() != ';');

                    m_l = LEX_RAW;
                    m_lex.index = addSymbol(s);
                }
                else
                {
                    m_l = LEX_NAME;
                    m_lex.index = addSymbol(s);
                }
            }
            else if (c() == ':')
            {
                m_l = LEX_COLON;
                gc();
            }
            else if (c() == '=')
            {
                m_l = LEX_EQ;
                gc();
            }
            else if (c() == ';')
            {
                m_l = LEX_SEMICOLON;
                gc();
            }
            else if (c() == ',')
            {
                m_l = LEX_COMMA;
                gc();
            }
            else if (c() == '.')
            {
                m_l = LEX_DOT;
                gc();
            }
            else if (c() == '#')
            {
                while (gc() != '\n' && !isEoi())
                    ;
            }
            else
            {
                std::string ch;
                ch = (char)c();
                error(E_UNEXPECTED_TOKEN, ch);
            }
        }
    }

    virtual void S()
    {
        section_patterns();
        section_synonyms();
    }

    void section_patterns()
    {
        m_section = PATTERNS;
        if (m_l != LEX_SECTION_PATTERNS)
            error(E_EXPECTED_TOKEN, "PATTERNS");
        gl();
        if (m_l != LEX_COLON)
            error(E_EXPECTED_TOKEN, ":");
        gl();
        if (m_l != LEX_RAW)
            error(E_EXPECTED_TOKEN, "Pattern");

        pattern();
        while (m_l != LEX_SECTION_SYNONYMS &&
               m_l != LEX_EOI && m_l == LEX_RAW)
            pattern();
    }

    void pattern()
    {
        Standard::EbnfRpnPattern pc;
        Pattern *p = new Pattern(pc);
        std::string patString = m_symbolsTable[m_lex.index];
        EParsing e = p->compile(patString + ";");
        if (e.id() != E_OK)
        {
            delete p;
            error(e.id(), e.arg());
        }
        m_patterns->push_back(p);
        gl();
        if (m_l != LEX_SEMICOLON)
            error(E_EXPECTED_TOKEN, ";");
        gl();
    }

    void section_synonyms()
    {
        m_section = SYNONYMS;
        if (m_l != LEX_SECTION_SYNONYMS)
            return;
        gl();
        if (m_l != LEX_COLON)
            error(E_EXPECTED_TOKEN, ":");
        gl();
        synonym();
        while (m_l != LEX_EOI)
            synonym();
    }

    void synonym()
    {
        if (m_l != LEX_NAME)
            error(E_EXPECTED_TOKEN, "Pattern name");
        std::string name1 = m_symbolsTable[ m_lex.index ];
        gl();
        if (m_l != LEX_EQ)
            error(E_EXPECTED_TOKEN, "=");
        gl();
        if (m_l != LEX_NAME)
            error(E_EXPECTED_TOKEN, "Pattern name");
        std::string name2 = m_symbolsTable[ m_lex.index ];
        FL::IDGenerator::makeSynonyms(name1, name2);
        gl();

        while (m_l == LEX_COMMA)
        {
            gl();
            if (m_l != LEX_NAME)
                error(E_EXPECTED_TOKEN, "Pattern name");
            std::string name2 = m_symbolsTable[ m_lex.index ];
            FL::IDGenerator::makeSynonyms(name1, name2);
            gl();
        }

        if (m_l != LEX_SEMICOLON)
            error(E_EXPECTED_TOKEN, ";");
        gl();
    }

    PatternsSet *m_patterns;
};

}}} // namespace


bool FilePAT::open(const std::string &fileName, FL::Patterns::PatternsSet &set)
{
    Internal::PatCompiler compiler(&set);

    std::ifstream file(fileName.c_str());
    file >> std::noskipws;
    FL::Compilers::Input::Iterator it(file);
    FL::Compilers::Input *input = new FL::Compilers::Input(it);
    bool result = compiler.compile(input);
    m_lastError = compiler.lastError();
    return result;
}
