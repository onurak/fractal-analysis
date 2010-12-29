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

#include "FilePAT.h"
#include "AbstractCompiler.h"
#include "../tools/spirit.h"
#include "../PatternGuardRPN.h"

using namespace FL;


namespace PATGrammar
{
    using namespace FL::Compilers;
    FL::PatternCollection *m_patterns;

    /* Grammar:
     * file                = [parameters_section] patterns_section [synonyms_section]
     * parameters_section  = "PARAMETERS" ":" parameter+
     * patterns_section    = "PATTERNS" ":" pattern+
     * synonyms_section    = "SYNONYMS" ":" synonym+
     * parameter           = name "=" default_value ";" [comment]
     * pattern             = description "@" guard ";" [comment]
     * synonym             = name "=" name ";" [comment]
     * name                = alpha alnum*;
     * default_value       = name | number
     * description         = (anychar - "@" - ";" - "#")+
     * guard               = (anychar - "@" - ";" - "#")+
     * comment             = "#" (anychar - "\n")* "\n"
     */
    //**** LEXEMES *****
    const Lexeme LEX_PARAMETERS = 1;
    const Lexeme LEX_PATTERNS   = 2;
    const Lexeme LEX_SYNONYMS   = 3;
    const Lexeme LEX_AT         = 4;
    const Lexeme LEX_SEMI       = 5;
    const Lexeme LEX_COLON      = 6;
    const Lexeme LEX_EQ         = 7;

    //****** LEXICAL ANALYSER *******
    /*! \class PATLexicalAnalyser
      */
    class PATLexicalAnalyser : public LexicalAnalyser
    {
    public:

        PATLexicalAnalyser()
        {
            m_oneLineComment ='#';
        }

        virtual Lexeme igl() throw (ParseException)
        {
            m_lex = LEX_NONE;

            while (m_lex == LEX_NONE)
            {
                // eof
                if (iter == end)
                {
                    m_lex = LEX_EOI;
                }
                // spaces
                else if (isspace(m_c))
                {
                     gc();
                }
                // names
                else if (isalpha(m_c))
                {
                    m_name = m_c;
                    while ( isalnum(gc()) )
                        m_name += m_c;
                    if (m_name == "PARAMETRES")
                        m_lex = LEX_PARAMETERS;
                    else if (m_name == "PATTERNS")
                        m_lex = LEX_PATTERNS;
                    else if (m_name == "SYNONYMS")
                        m_lex = LEX_SYNONYMS;
                    else
                        m_lex = LEX_NAME;
                }
                // semicolon
                else if (m_c == ';')
                {
                    m_lex = LEX_SEMI;
                    gc();
                }
                // colon
                else if (m_c == ':')
                {
                    m_lex = LEX_COLON;
                    gc();
                }
                // at
                else if (m_c == '@')
                {
                    m_lex = LEX_AT;
                    gc();
                }
                // eq
                else if (m_c == '=')
                {
                    m_lex = LEX_EQ;
                    gc();
                }
                // comment
                else if (m_c == '#')
                {
                    while (m_c != '\n' && (*iter != *end))
                        gc();
                }
                // number
                else if (!p_number())
                    m_lex = LEX_UNKNOWN;

            }
            return m_lex;
        }




    private:

    };

    //****** SYNTAX ANALYSER *******
    /*! \class SyntaxAnalyser

      *
      */
    class PATSyntaxAnalyser : public SyntaxAnalyser
    {
    public:
        PATSyntaxAnalyser ()
        {
            m_lexical = new PATLexicalAnalyser();
        }

    protected:

        virtual void S()
        {
            file();
        }

        void file()
        {
            // parameters
            if (lex == LEX_PARAMETERS)
            {
                gl();
                if (lex != LEX_COLON)
                    throw ParseException("':' expected");
                gl();
                parameters_section();
            }
            // patterns
            if (lex != LEX_PATTERNS)
                throw ParseException("'PATTERNS' expected");
            gl();
            if (lex != LEX_COLON)
                throw ParseException("':' expected");
            patterns_section();
            // synonyms
            if (lex == LEX_SYNONYMS)
            {
                gl();
                if (lex != LEX_COLON)
                    throw ParseException("':' expected");
                gl();
                synonyms_section();
            }
        }

        void parameters_section()
        {
            parameter();
            while (lex == LEX_NAME)
                parameter();
        }

        void patterns_section()
        {
            pattern();
            while (lex != LEX_SYNONYMS && lex != LEX_EOI)
                pattern();
        }

        void synonyms_section()
        {
            synonym();
            while (lex == LEX_NAME)
                synonym();
        }

        void parameter()
        {
            if (lex != LEX_NAME)
                throw ParseException("Name expected");
            std::string paramName = m_lexical->name();
            gl();
            if (lex != LEX_EQ)
                throw ParseException("'=' expected");
            gl();

            GVariant paramValue;
            if (lex == LEX_INTEGER || lex == LEX_FLOAT)
                paramValue = m_lexical->number();
            else if (lex == LEX_NAME)
                paramValue = m_lexical->name();
            else
                throw ParseException("Name or number expected");

            gl();
            if (lex != LEX_SEMI)
                throw ParseException("';' expected");
            gl();

            m_patterns->parameters.add(paramName, paramValue);
        }

        void pattern()
        {            
            if (m_lexical->lookForwardFor('@') != -1)
            {
                FL::Pattern *pattern =
                        new FL::Pattern(m_patterns->synonyms, m_patterns->parameters);

                // Description
                std::string description = m_lexical->getRaw("@");
                Patterns::DescriptionCompiler *dCompiler =
                        pattern->description()->compiler();
                if ( !dCompiler->compile(description, *(pattern->description()->structure())) )
                {
                    std::string errorDescription =
                            std::string("error in description: ") + dCompiler->errorDescription;
                    delete pattern;
                    throw ParseException(errorDescription);
                }
                pattern->description()->setSourceText(description, false);
                gl();
                if (lex != LEX_AT)
                {
                    delete pattern;
                    throw ParseException("'@' expected");
                }
                // Guard
                std::string guard = m_lexical->getRaw(";");
                Patterns::GuardCompiler *gCompiler =
                    pattern->guard()->compiler();
                if ( !gCompiler->compile(guard, *(pattern->guard()->structure())) )
                {
                    std::string errorDescription =
                            std::string("error in guard: ") + gCompiler->errorDescription;
                    delete pattern;
                    throw ParseException(errorDescription);
                }
                pattern->guard()->setSourceText(description, false);
                gl();
                if (lex != LEX_SEMI)
                {
                    delete pattern;
                    throw ParseException("';' expected after pattern");
                }

                // Ready
                m_patterns->push_back(pattern);
            }
            else
                gl();
        }

        void synonym()
        {
            if (lex != LEX_NAME)
                throw ParseException("Name expected");
            std::string left = m_lexical->name();
            gl();
            if (lex != LEX_EQ)
                throw ParseException("'=' expected");
            gl();

            if (lex != LEX_NAME)
                throw ParseException("Name expected");
            std::string right = m_lexical->name();

            gl();
            if (lex != LEX_SEMI)
                throw ParseException("';' expected");
            gl();

            m_patterns->synonyms.add(left, right);
        }

    };
} // namespace

bool FilePAT::loadFromString(const std::string &text, PatternCollection &patterns)
{
    patterns.synonyms.clear();
    PATGrammar::m_patterns = &patterns;
    PATGrammar::PATSyntaxAnalyser analyser;
    if (!analyser.analyse(text.begin(), text.end()))
    {
        char err[200];
        sprintf(err, "Error at pos (%d, %d): %s",
                analyser.pos().line,
                analyser.pos().column,
                analyser.lastErrorDescription().c_str());
        GError(GCritical, "FilePAT", 1, err);
        m_lastErrorDescription = err;
        return false;
    }
    return true;
}


bool FilePAT::loadFromFile(const std::string &fileName, PatternCollection &patterns)
{
    std::ifstream in(fileName.c_str(), std::ios_base::in);

    if (!in.is_open())
    {
        m_lastErrorDescription = std::string("Can't open file: ") + fileName;
        return false;
    }

    std::string text;
    in.unsetf(std::ios::skipws); // don't skip whitespaces
    copy(
            std::istream_iterator<char>(in),
            std::istream_iterator<char>(),
            std::back_inserter(text)
            );

    bool result = loadFromString(text, patterns);
    in.close();
    return result;
}

