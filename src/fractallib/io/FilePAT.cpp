#include "FilePAT.h"
#include "../compilers/AbstractCompiler.h"
#include "../patterns/standart/EbnfRpnPattern.h"

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

public:
    PatCompiler(PatternsSet *patterns)
        : m_patterns(patterns)
    {
        m_reservedWords["PATTERNS"] = LEX_SECTION_PATTERNS;
    }
protected:
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

                do { s += c(); gc(); }
                while (!isEoi() && c() != ':' && c() != ';');

                m_l = LEX_RAW;
                m_lex.index = addSymbol(s);
            }
            else if (c() == ':')
            {
                m_l = LEX_COLON;
                gc();
            }
            else if (c() == ';')
            {
                m_l = LEX_SEMICOLON;
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
    }

    void section_patterns()
    {
        if (m_l != LEX_SECTION_PATTERNS)
            error(E_EXPECTED_TOKEN, "PATTERNS");
        gl();
        if (m_l != LEX_COLON)
            error(E_EXPECTED_TOKEN, ":");
        gl();
        if (m_l != LEX_RAW)
            error(E_EXPECTED_TOKEN, "Pattern");

        pattern();
        while (m_l != LEX_EOI && m_l == LEX_RAW)
            pattern();
    }

    void pattern()
    {
        Standart::EbnfRpnPattern pc;
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

    PatternsSet *m_patterns;
};

}}} // namespace


bool FilePAT::open(const std::string &fileName, FL::Patterns::PatternsSet &set)
{
    Internal::PatCompiler compiler(&set);

    std::ifstream file(fileName.c_str());
    file >> std::noskipws;
    FL::Compilers::Input::Iterator it(file);
    FL::Compilers::Input input(it);
    bool result = compiler.compile(&input);
    m_lastError = compiler.lastError();
    return result;
}
