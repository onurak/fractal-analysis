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
#include "../tools/spirit.h"
#include "../PatternGuardRPN.h"

using namespace FL;


class EParse
{
public:
    EParse(const std::string msg): msg(msg) {}
    std::string msg;
};

/*
namespace PATGrammar {

    FL::Pattern *m_pattern;
    FL::PatternCollection *m_patterns;
    std::string m_fileName, m_synLeft;

    void onDescription(std::string description)
    {
        m_pattern = new FL::Pattern(m_patterns->synonyms, m_patterns->parameters);
        if (!description.empty())
        {
            Patterns::DescriptionCompiler *compiler = m_pattern->description()->compiler();
            if ( !compiler->compile(description, *(m_pattern->description()->structure())) )
            {
                delete m_pattern;
                throw EParse(EInvalidFileFormat);
            }
            m_pattern->description()->setSourceText(description, false);
        }
    }

    void onGuard(std::string guard)
    {
        if (!guard.empty())
        {
            Patterns::GuardCompiler *compiler = m_pattern->guard()->compiler();
            if ( !compiler->compile(guard, *(m_pattern->guard()->structure())) )
            {
                delete m_pattern;
                throw EParse(EInvalidFileFormat);
            }
            m_pattern->guard()->setSourceText(guard, false);
            m_patterns->push_back(m_pattern);
        } else
            delete m_pattern;

    }

    void onSynLeft(std::string name)
    {
        m_synLeft = name;
    }

    void onSynRight(std::string name)
    {
        m_patterns->synonyms.add(m_synLeft, name);
    }

    template <typename Iterator>
    struct Grammar : qi::grammar<Iterator>
    {
        Grammar() : Grammar::base_type(file)
        {
            using ascii::alpha;
            using ascii::alnum;
            using ascii::char_;
            using qi::eps;
            using qi::eol;
            using boost::spirit::lit;

            file              = patterns_section >> -synonyms_section;
            patterns_section  = lit("PATTERNS") >> *space >> ":" >> +pattern;
            pattern           = *space >>
                                description[onDescription] >> *space >>
                                lit("@")                   >> *space >>
                                guard[onGuard]             >> *space >>
                                lit(";")                   >> *space >>
                                -comment;
            description      %= +(char_ - char_('@') - char_('#'));
            guard            %= +(char_ - char_(';') - char_('#'));
            synonyms_section  = lit("SYNONYMS") >> *space >> ":" >> +synonym;
            synonym           = *space >>
                                pattern_name[onSynLeft]  >> *space >>
                                lit("=")                 >> *space >>
                                pattern_name[onSynRight] >> *space >>
                                lit(";")                 >> *space >>
                                -comment;
            pattern_name     %= alpha >> *alnum;
            comment           = lit("#") >> *(char_ - eol) >> eol;
            space             = lit(' ') | lit('\t') | lit('\n');
        }

        qi::rule<Iterator>
            file, patterns_section, synonyms_section,
            pattern, synonym, comment, space;
        qi::rule<Iterator, std::string()>
            description, guard, pattern_name;
    };

} // namespace
*/

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

namespace PATGrammar
{
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
    enum Lexeme
    {
        LEX_PARAMETERS,
        LEX_PATTERNS,
        LEX_SYNONYMS,
        LEX_NAME,
        LEX_AT,
        LEX_SEMI,
        LEX_FLOAT,
        LEX_DECIMAL,
        LEX_COLON,
        LEX_EQ,
        LEX_EOI,
        LEX_NONE,
        LEX_UNKNOWN
    };

    //****** LEXICAL ANALYSER *******
    /*! \class LexicalAnalyser
      */
    class LexicalAnalyser
    {
    public:
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
            c = LEX_NONE;

            while (c == LEX_NONE)
            {
                // eof
                if (*iter == *end)
                {
                    c = LEX_EOI;
                }
                // spaces
                else if (isspace(m_c))
                {
                     gc();
                }
                // names
                else if (isalpha(m_c))
                {
                    name = m_c;
                    while ( isalnum(gc()) )
                        name += m_c;
                    if (name == "PARAMETRES")
                        c = LEX_PARAMETERS;
                    else if (name == "PATTERNS")
                        c = LEX_PATTERNS;
                    else if (name == "SYNONYMS")
                        c = LEX_SYNONYMS;
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
                // semicolon
                else if (m_c == ';')
                {
                    c = LEX_SEMI;
                    gc();
                }
                // colon
                else if (m_c == ':')
                {
                    c = LEX_COLON;
                    gc();
                }
                // at
                else if (m_c == '@')
                {
                    c = LEX_AT;
                    gc();
                }
                // eq
                else if (m_c == '=')
                {
                    c = LEX_EQ;
                    gc();
                }
                // comment
                else if (m_c == '#')
                {
                    while (m_c != '\n' && (*iter != *end))
                        gc();
                }
                else
                    c = LEX_UNKNOWN;
            }
            return c;
        }

        std::string getRaw(const std::string &overs)
        {
            std::string result;
            while (!in_overs(m_c, overs) && *iter != *end)
            {
                if (m_c == '#')
                    while (m_c != '\n' && *iter != *end)
                        gc();
                result += m_c;
                gc();
            }
            return result;
        }

        Lexeme c;         //!< Current lexeme
        std::string name; //!< Current name (if c is LEX_NAME)
        double number;    //!< Current double (if c is LEX_FLOAT or LEX_DECIMAL)
    private:
        std::string::const_iterator *iter, *end;

        char m_c;

        char gc()
        {
            ++(*iter);
            return m_c = (*iter != *end ? **iter : LEX_EOI);
        }

        bool in_overs(char c, const std::string &overs)
        {
            return find_(c, overs);

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
            file();
        }

        void file()
        {
            // parameters
            if (lex == LEX_PARAMETERS)
            {
                gl();
                if (lex != LEX_COLON)
                    throw ParseException(1, "':' expected");
                gl();
                parameters_section();
            }
            // patterns
            if (lex != LEX_PATTERNS)
                throw ParseException(1, "'PATTERNS' expected");
            gl();
            if (lex != LEX_COLON)
                throw ParseException(1, "':' expected");
            patterns_section();
            // synonyms
            if (lex == LEX_SYNONYMS)
            {
                gl();
                if (lex != LEX_COLON)
                    throw ParseException(1, "':' expected");
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
            while (lex != LEX_SYNONYMS)
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
                throw ParseException(1, "Name expected");
            std::string paramName = m_lexical.name;
            gl();
            if (lex != LEX_EQ)
                throw ParseException(1, "'=' expected");
            gl();

            GVariant paramValue;
            if (lex == LEX_DECIMAL || lex == LEX_FLOAT)
                paramValue = m_lexical.number;
            else if (lex == LEX_NAME)
                paramValue = m_lexical.name;
            else
                throw ParseException(1, "Name or number expected");

            gl();
            if (lex != LEX_SEMI)
                throw ParseException(1, "';' expected");
            gl();

            m_patterns->parameters.add(paramName, paramValue);
        }

        void pattern()
        {
            FL::Pattern *pattern =
                    new FL::Pattern(m_patterns->synonyms, m_patterns->parameters);
            // Description
            std::string description = m_lexical.getRaw("@");
            if (!description.empty())
            {
                Patterns::DescriptionCompiler *compiler =
                        pattern->description()->compiler();
                if ( !compiler->compile(description, *(pattern->description()->structure())) )
                {
                    delete pattern;
                    throw ParseException(1, EInvalidFileFormat);
                }
                pattern->description()->setSourceText(description, false);
            }
            gl();
            if (lex != LEX_AT)
            {
                delete pattern;
                throw ParseException(1, EInvalidFileFormat);
            }

            // Guard
            std::string guard = m_lexical.getRaw(";");
            if (!description.empty())
            {
                Patterns::GuardCompiler *compiler =
                        pattern->guard()->compiler();
                if ( !compiler->compile(guard, *(pattern->guard()->structure())) )
                {
                    delete pattern;
                    throw ParseException(1, EInvalidFileFormat);
                }
                pattern->guard()->setSourceText(description, false);
            }
            gl();
            if (lex != LEX_SEMI)
            {
                delete pattern;
                throw ParseException(1, EInvalidFileFormat);
            }

            // Ready
            m_patterns->push_back(pattern);
        }

        void synonym()
        {
            if (lex != LEX_NAME)
                throw ParseException(1, "Name expected");
            std::string left = m_lexical.name;
            gl();
            if (lex != LEX_EQ)
                throw ParseException(1, "'=' expected");
            gl();

            if (lex != LEX_NAME)
                throw ParseException(1, "Name expected");
            std::string right = m_lexical.name;

            gl();
            if (lex != LEX_SEMI)
                throw ParseException(1, "';' expected");
            gl();

            m_patterns->synonyms.add(left, right);
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

void FilePAT::loadFromString(const std::string &text, PatternCollection &patterns)
{
    patterns.synonyms.clear();
    PATGrammar::m_patterns = &patterns;
    std::string::const_iterator it = text.begin(), end = text.end();
    PATGrammar::SyntaxAnalyser analyser;
    analyser.analyse(it, end);
}


void FilePAT::loadFromFile(const std::string &fileName, PatternCollection &patterns)
{
    std::ifstream in(fileName.c_str(), std::ios_base::in);

    if (!in.is_open())
        return;

    std::string text;
    in.unsetf(std::ios::skipws); // не пропускаем пробелы
    copy(
            std::istream_iterator<char>(in),
            std::istream_iterator<char>(),
            std::back_inserter(text)
            );

    loadFromString(text, patterns);

    in.close();
}

