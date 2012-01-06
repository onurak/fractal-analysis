#include "DescriptionCompilerEbnf.h"
#include "../Context.h"
#include "../../compilers/AbstractCompiler.h"
#include "../../trees/Tree.h"

using namespace FL::Patterns::Standard;
using namespace FL::Exceptions;
using namespace FL::Compilers;

namespace FL { namespace Patterns { namespace Standard { namespace Internal {

    /////////////////////////////////////////////////////////////
    // SECTION: EBNF COMPILER
    /////////////////////////////////////////////////////////////


    /*! \class EbnfCompiler
      * \brief Actual EBNF compiler
      *
      * Grammar:
      * \code
      * name         = alpha {alpha | digit};
      * indexed      = name ["_" uint];
      * ebnf         = name '=' expr;
      * expr         = term  {"|" term};
      * term         = factor {factor};
      * factor       = name | indexed | "(" expr ")" | "[" expr "]";
      * \endcode
      */
    class EbnfCompiler : public AbstractCompiler
    {
    public:
        static const int LEX_ALTERNATIVE       = 5;
        static const int LEX_LOPTION           = 6;
        static const int LEX_ROPTION           = 7;
        static const int LEX_INDEXED           = 8;
    private:
        Description *m_description;
    public:
        EbnfCompiler(Description *description)
        {
            m_description = description;
            filterLexeme(LEX_ASSIGN,    true);
            filterLexeme(LEX_LPAREN,    true);
            filterLexeme(LEX_RPAREN,    true);
            filterLexeme(LEX_INTEGER,   true);
            filterLexeme(LEX_AT,        true);
        }

    protected:
        virtual void vgl()
        {
            while (m_l == LEX_UNKNOWN)
            {
                if (isspace(c()))
                {
                    gc();
                }
                else if (isalpha(c()))
                {
                    std::string name;
                    name = c();
                    while (isalnum(gc()))
                        name += c();
                    if (c() == '_')
                    {
                        m_l = LEX_INDEXED;
                        gc();
                    }
                    else
                        m_l = LEX_NAME;
                    m_lex.index = addSymbol(name);
                }
                else if (c() == '|')
                {
                    gc();
                    m_l = LEX_ALTERNATIVE;
                }
                else if (c() == '[')
                {
                    gc();
                    m_l = LEX_LOPTION;
                }
                else if (c() == ']')
                {
                    gc();
                    m_l = LEX_ROPTION;
                }
                else
                    AbstractCompiler::vgl();
            }
        }

    protected:
        virtual void S()
        {
            m_description->sequences() = ebnf();
            makeSet(m_description->sequences());
        }

        CISet ebnf()
        {
            if (m_l != LEX_NAME)
                error(E_EXPECTED_TOKEN, "Pattern name");
            m_description->setName(m_symbolsTable[m_lex.index]);
            gl();
            if (m_l != LEX_ASSIGN)
                error(E_EXPECTED_TOKEN, "=");
            gl();
            CISet set;
            set = expr(set);
            if (m_l != LEX_AT)
                error(E_EXPECTED_TOKEN, "@");
            return set;
        }

        CISet expr(CISet set)
        {
            CISet result = term(set);
            while (m_l == LEX_ALTERNATIVE)
            {
                gl();
                CISet r1 = term(set);
                result.insert(result.end(), r1.begin(), r1.end());
            }
            return result;
        }

        CISet term(CISet set)
        {
            CISet result = factor(set);
            while (m_l == LEX_NAME     || m_l == LEX_INDEXED ||
                   m_l == LEX_LPAREN   || m_l == LEX_LOPTION)
            {
                CISet r1 = factor(set);
                result = closure(result, r1);
            }
            return result;
        }

        CISet factor(CISet set)
        {
            switch (m_l)
            {
                case LEX_NAME:
                {
                    CINode node;
                    node.id    = IDGenerator::idOf((char*)m_symbolsTable[m_lex.index]);
                    node.index = -1;
                    append(set, node);
                    gl();
                    break;
                }

                case LEX_INDEXED:
                {
                    CINode node;
                    node.id = IDGenerator::idOf((char*)m_symbolsTable[m_lex.index]);
                    gl();
                    if (m_l != LEX_INTEGER)
                        error(E_EXPECTED_TOKEN, "Index");
                    node.index = m_symbolsTable[m_lex.index];
                    append(set, node);
                    gl();
                    break;
                }

                case LEX_LPAREN:
                {
                    gl();
                    set = expr(set);
                    if (m_l != LEX_RPAREN)
                        error(E_EXPECTED_TOKEN, ")");
                    gl();
                    break;
                }

                case LEX_LOPTION:
                {
                    gl();
                    CISet opt = expr(set);
                    if (m_l != LEX_ROPTION)
                        error(E_EXPECTED_TOKEN, "]");

                    CISet::iterator i, j;
                    if (set.size() > 0)
                    {
                        forall(i, set)
                        {
                            forall(j, opt)
                            {
                                CISequence seq = *i;
                                seq.insert(seq.end(), j->begin(), j->end());
                                set.push_back(seq);
                            }
                        }
                    }
                    else
                    {
                        CISequence empty;
                        set.push_back(empty);
                        set.insert(set.end(), opt.begin(), opt.end());
                    }
                    gl();
                    break;
                }

                default:
                {
                    error(E_UNEXPECTED_TOKEN, psymbol());
                }
            }
            return set;
        }

    protected:
        CISet closure(CISet &set1, CISet &set2)
        {
            CISet result;
            CISet::iterator i, j;
            forall(i, set1)
                forall(j, set2)
                {
                    CISequence s;
                    s.assign(i->begin(), i->end());
                    s.insert(s.end(), j->begin(), j->end());
                    result.insert(result.end(), s);
                }
            return result;
        }

        void append(CISet &set, CINode &node)
        {
            if (set.size() > 0)
            {
                CISet::iterator i;
                forall(i, set)
                    i->push_back(node);
            }
            else
            {
                CISequence s;
                s.push_back(node);
                set.push_back(s);
            }
        }

        //! Insure that set contains om;y unique items
        void makeSet(CISet &set)
        {
            CISet::iterator i;
            for(i = set.begin(); i != set.end(); )
            {
                if (std::find(i+1, set.end(), *i) != set.end())
                    i = set.erase(i);
                else
                    ++i;
            }
        }
    };

}}}} // namespaces


/////////////////////////////////////////////////////
// SECTION: DescriptionEbnf
/////////////////////////////////////////////////////

EParsing DescriptionCompilerEbnf::compile(Compilers::Input &input, Description *description)
{
    Internal::EbnfCompiler compiler(description);
    if (!compiler.compile(&input))
        return compiler.lastError();
    else
        return EParsing(E_OK);
}
