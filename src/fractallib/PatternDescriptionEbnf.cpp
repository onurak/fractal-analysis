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

#include "PatternDescriptionEbnf.h"
//#include "tools/spirit.h"
#include "io/AbstractCompiler.h"

using namespace FL;
using namespace FL::Patterns::EBNF;

std::string nodeToString(Node *node, EbnfStructure *ebnf)
{
    if (node == NULL)
        return "(NULL)";
    std::string result = "(";
    switch (node->type)
    {
        case NAME:
            if (ebnf != NULL)
                if (node->concr >= 0 && node->concr < int(ebnf->table.size()))
                    result += ebnf->table[node->concr]->name;
                else
                    result += "err";
            else
                result += "u";
            break;
        case OPERATOR:
            switch (node->concr)
            {
                case opAlternative:
                    result += "|"; break;
                case opConcatenate:
                    result += ","; break;
                case opOptional:
                    result += "-"; break;
                case opRepeat:
                    result += "*"; break;
                default:
                    result += "u"; break;
            }
            break;
        case TRUEC:
            result += "true";
            break;
        case FALSEC:
            result += "false";
            break;
        default:
            result += "U";
            break;
    }
    return result + ")";
}

EbnfStructure::EbnfStructure(Node *newRoot)
    : m_root(newRoot)
{
}

EbnfStructure::~EbnfStructure()
{
    remNode(m_root);
}

std::string EbnfStructure::print(Node *root, int level)
{
    if (root != NULL)
    {
        std::string result = "";
        // первую половину потомков выводим сверху от корня
        for (int i = 0; i < int(root->children.size() / 2); i++)
            result += print(root->children[i], level+1);

        // выводим нужное число отступов и строку узла
        for (int i = 0; i < level; i++)
            result += "\t";
        result += nodeToString(root, this) + "\n";

        // вторую половину потомков выводим снизу от корня
        for (int i = root->children.size() / 2; i < int(root->children.size()); i++)
            result += print(root->children[i], level+1);

        return result;
    }
    return "";
}

int EbnfStructure::maxLength() const
{
    // Fix it!
    // It must be calculated for each pattern, hope no one will use patterns longer
    return 20;
}

/****************** CHECKER ******************/

bool EbnfChecker::check(CheckContext &context)
{
    // Create expression for check
    Expression expr;
    std::string exprString;
    context.lastNode = context.iRoot;
    for (int i = 0;
         i < m_structure->maxLength() && context.lastNode != context.roots->end();
         ++i, ++(context.lastNode))
    {
        expr.items.push_back(*context.lastNode);
        exprString += (*context.lastNode)->name + " ";
    }
    logg.debug("    Expression to check: ") << exprString;
    if (check(&expr, *context.synonyms, coNothing))
    {
        // fill 'last parsed' sequence
        context.lastParsed.clear();
        context.patternSize = expr.pos;
        context.lastNode = context.iRoot;
        while (expr.pos > 1)
        {
            expr.pos--;
            context.lastParsed.push_back(*context.lastNode);
            ++context.lastNode;
        }
        context.lastParsed.push_back(*context.lastNode);
        return true;
    }
    return false;
}

bool EbnfChecker::check(Expression *expr, const SynonymsTable &synonyms, CheckOptions options)
{
    if (m_structure->m_root == NULL || expr == NULL)
        return false;

    m_options = options;

    expr->first();
    expr->next();
    bool r = rcheck(expr, synonyms, m_structure->m_root);

    if (options & coParseWholeExpression)
        r = r && !expr->empty() ? false : r;
    return r;
}

bool EbnfChecker::rcheck(Expression *expr, const SynonymsTable &synonyms, Node *node)
{
    int pos;
    std::vector<Node*>::iterator p;
    if (node->type == NAME)
    {
        if (node->concr < 0 || node->concr >= int(m_structure->table.size()))
        {
            GError(GCritical,
                   "EbnfChecker",
                   0,
                   EInvalidIndex);
            return false;
        }
        if (expr->current != NULL &&
            synonyms.isSynonyms(
                    expr->current->nameId,
                    m_structure->table[node->concr]->nameId))
        {
            expr->current->no = m_structure->table[node->concr]->no;
            expr->next();
            return true;
        }
        return false;
    }
    else if (node->type == OPERATOR)
    {
        switch (node->concr)
        {
            // '|' - достаточно, чтобы выполнялся хотя бы один из потомков
            case opAlternative:
                pos = expr->pos;
                for (p = node->children.begin(); p != node->children.end(); p++)
                    if (rcheck(expr, synonyms, *p))
                        return true;
                expr->restore(pos);
                return false;
            // ',' - необходимо, чтобы выполнялись все потомки
            case opConcatenate:
                pos = expr->pos;
                for (p = node->children.begin(); p != node->children.end(); p++)
                    if (!rcheck(expr, synonyms, *p))
                    {
                        expr->restore(pos);
                        return false;
                    }
                return true;
            // '{  }' - все равно сколько раз выполняется потомок
            case opRepeat:
                while (rcheck(expr, synonyms, node->children[0]))
                    ;
                return true;
            // '[  ]' - потомок должен выполняться 0 или 1 раз
            case opOptional:
                rcheck(expr, synonyms, node->children[0]);
                return true;
            default:
                //error();
                break;
        }
    }
    else if (node->type == TRUEC)
    {
        return true; // узел truesrc
    }
    else if (node->type == FALSEC)
    {
        return false; // узел false
    } else
        {};//error();

    return false;
}

/*********** EBNF Tree compiler *************/

/* Guard's grammar*/

/*
namespace EBNFGrammar {

    //! Элемент шаблона
    struct Element
    {
        Element(std::string name, int no) : name(name), no(no) {}
        std::string name; //!< Название шаблона
        int no; //!< Номер в последовательности шаблона
    };

    Element m_indexedElem("", -1);
    FL::Patterns::EBNF::EbnfStructure *m_ebnf;
    FL::Patterns::EBNF::Node *m_curNode;

    FL::Patterns::EBNF::Node* newChild()
    {
        FL::Patterns::EBNF::Node* result = new FL::Patterns::EBNF::Node(TRUEC);
        m_curNode->children.push_back(result);
        result->parent = m_curNode;
        return result;
    }

    //   ИНДЕКСИРОВАНЫЕ
    void onIndexedName(std::string name)
    {
        m_indexedElem.name = name;
    }

    void onIndexedNo(unsigned int no)
    {
        m_indexedElem.no = int(no);
    }

    //  ОПЕРАТОРЫ
    void onLevelDown()
    {
        m_curNode = newChild();
    }

    void onLevelUp()
    {
        m_curNode = m_curNode->parent;
    }

    void onAlternative()
    {
        m_curNode->type = OPERATOR;
        m_curNode->concr = opAlternative;
    }

    void onFactor()
    {
        onLevelUp();
        m_curNode->type = OPERATOR;
        m_curNode->concr = opConcatenate;
    }

    void onOptional()
    {
        onLevelUp();
        m_curNode->type = OPERATOR;
        m_curNode->concr = opOptional;
    }

    void onIdent()
    {
        m_curNode->type = NAME;
        m_curNode->concr = m_ebnf->getOrCreateName(m_indexedElem.name,  m_indexedElem.no);
        m_curNode = m_curNode->parent;
        m_curNode = newChild();
    }

    void onTemplateName(std::string name)
    {
        m_ebnf->setName(name);
    }

    //  ГРАММАТИКА

    template <typename Iterator>
    struct Grammar : qi::grammar<Iterator>
    {
        Grammar() : Grammar::base_type(bnf)
        {
            using ascii::char_;
            using ascii::alpha;
            using ascii::alnum;
            using ascii::digit;
            using boost::spirit::lit;
            using qi::uint_;
            using qi::eps;

            name        %= alpha >> *alnum;
            indexed      = name[onIndexedName] >> -('_' >> uint_[onIndexedNo]);
            term_srt     = eps[onLevelDown] >> term[onLevelUp];
            factor_srt   = eps[onLevelDown] >> factor[onLevelUp];

            bnf          = *space >> name[onTemplateName] >> *space >> '=' >> *space >> expr;
            expr         = (term_srt >> *(*space >> "|" >> *space >> term_srt))[onAlternative];     // альтернатива
            term         = (factor_srt >> *factor_srt)[onFactor]; // конкатенация
            factor       = (indexed[onIdent] >> *space) |
                           (*space >> "(" >> *space >> expr >> *space >> ")") |
                           (*space >> char_('[')[onLevelDown] >> expr >> *space >> char_(']')[onOptional]);
                           // | "{" >> expr >> "}";
            space        = char_(' ') | '\t' | '\n';
        }

        qi::rule<Iterator>
            indexed, bnf, expr, factor, term, term_srt, factor_srt, space;
        qi::rule<Iterator, std::string()>
            name;
    };
} // namespace

*/

namespace EBNFGrammar
{
    using namespace FL::Compilers;

    FL::Patterns::EBNF::EbnfStructure *m_ebnf;
    FL::Patterns::EBNF::Node *m_curNode;

    /* Lexemes */
    const Lexeme LEX_INDEXED_NAME = 1;
    const Lexeme LEX_LPAREN       = 2;
    const Lexeme LEX_RPAREN       = 3;
    const Lexeme LEX_LBRACKET     = 4;
    const Lexeme LEX_RBRACKET     = 5;
    const Lexeme LEX_WILDCARD     = 6;
    const Lexeme LEX_ALTERNATIVE  = 7;
    const Lexeme LEX_EQ           = 8;

    class EBNFLexicalAnalyser : public LexicalAnalyser
    {
    public:
        EBNFLexicalAnalyser()
        {
            m_oneLineComment = '#';
        }

    protected:
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
                    m_number = -1;
                    m_name = m_c;
                    while ( isalnum(gc()) )
                        m_name += m_c;
                    if (m_c == '_')
                    {
                        gc();
                        if (!isdigit(m_c))
                            throw ParseException("Index expected");
                        m_number = 0;
                        do {
                            m_number = m_number * 10 + int(m_c - '0');
                        } while ( isdigit(gc()) );
                        if ( isalpha(m_c) )
                            throw ParseException("Delimeter expected between names");
                        m_lex = LEX_INDEXED_NAME;
                    }
                    else
                        m_lex = LEX_NAME;
                }

                // parenthesis
                else if (m_c == '(')
                {
                    gc();
                    m_lex = LEX_LPAREN;
                }
                else if (m_c == ')')
                {
                    gc();
                    m_lex = LEX_RPAREN;
                }

                // brackets
                else if (m_c == '[')
                {
                    gc();
                    m_lex = LEX_LBRACKET;
                }
                else if (m_c == ']')
                {
                    gc();
                    m_lex = LEX_RBRACKET;
                }

                // wildcard
                else if (m_c == '*')
                {
                    gc();
                    m_lex = LEX_WILDCARD;
                }

                // alternative
                else if (m_c == '|')
                {
                    gc();
                    m_lex = LEX_ALTERNATIVE;
                }

                // eq
                else if (m_c == '=')
                {
                    gc();
                    m_lex = LEX_EQ;
                }

                // unknown
                else
                    m_lex = LEX_UNKNOWN;
            }
            return m_lex;
        }
    };



    class EBNFSyntaxAnalyser : public SyntaxAnalyser
    {
    public:
        EBNFSyntaxAnalyser()
        {
            m_lexical = new EBNFLexicalAnalyser();
        }

    protected:
        FL::Patterns::EBNF::Node* newChild()
        {
            FL::Patterns::EBNF::Node* result = new FL::Patterns::EBNF::Node(TRUEC);
            m_curNode->children.push_back(result);
            result->parent = m_curNode;
            return result;
        }

        void goDown()
        {
            m_curNode = newChild();
        }

        void goUp()
        {
            m_curNode = m_curNode->parent;
        }

        void goNext()
        {
            m_curNode = m_curNode->parent;
            m_curNode = newChild();
        }

    protected:
        virtual void S()
        {
            name();
            if (lex != LEX_EQ)
                throw ParseException("'=' expected");
            gl();
            ebnf();
        }

        void name()
        {
            if (lex != LEX_NAME)
                throw ParseException("Pattern's name expected");
            m_ebnf->setName(m_lexical->name());
            gl();
        }

        void ebnf()
        {
            goDown();
            term();
            while (lex == LEX_ALTERNATIVE)
            {
                gl();
                goDown();
                term();
            }
            m_curNode->type  = OPERATOR;
            m_curNode->concr = opAlternative;
        }

        void term()
        {
            goDown();
            factor();
            while (lex == LEX_NAME     || lex == LEX_INDEXED_NAME ||
                   lex == LEX_LPAREN )//|| lex == LEX_LBRACKET )
            {
                factor();
            }
            goUp();
            m_curNode->type  = OPERATOR;
            m_curNode->concr = opConcatenate;
            goUp();
        }

        void factor()
        {
            if (lex == LEX_NAME || lex == LEX_INDEXED_NAME)
            {
                m_curNode->type = NAME;
                m_curNode->concr = m_ebnf->getOrCreateName(m_lexical->name(),  m_lexical->number());
                gl();
            }
            else if (lex == LEX_LPAREN)
            {
                gl();
                ebnf();
                if (lex != LEX_RPAREN)
                    throw ParseException("')' expected");
                gl();
            }
            else
                throw ParseException(std::string("Unexpected symbol: ") + m_lexical->c());
            goNext();
        }

    };


} // namespace

bool EbnfCompiler::compile(const std::string &text,
                           DescriptionStructure& description)
{
    errorPos.set(0, 0, 0);
    EbnfStructure *ebnf = dynamic_cast<EbnfStructure*>(&description);
    if (ebnf == NULL)
        return false;


    // Prepare
    ebnf->remNode(ebnf->root());
    ebnf->setRoot(new Node(0, 0));
    EBNFGrammar::m_ebnf = ebnf;
    EBNFGrammar::m_curNode = ebnf->root();

    // Parse
    /*
    EBNFGrammar::Grammar<std::string::const_iterator> g;
    errorPos = spiritParseString(text, g);
    */
    EBNFGrammar::EBNFSyntaxAnalyser analyser;
    if (!analyser.analyse(text.begin(), text.end()))
    {
        ebnf->remNode(ebnf->root());
        char err[200];
        sprintf(err, "Error at pos (%d, %d): %s",
                analyser.pos().line,
                analyser.pos().column,
                analyser.lastErrorDescription().c_str());
        GError(GCritical, "EBNF compiler", 1, err);
        errorPos = analyser.pos();
        errorDescription = analyser.lastErrorDescription();
        return false;
    }

    // Optimze
    optimize(ebnf->root(), ebnf);

    logg.debug("Pattern compiled:\n") << ebnf->print();
    errorPos.set(-1, -1, -1);
    return true;
}

inline void removeChild(Node *parent, Node *child)
{
    std::vector<Node*>::iterator p;
    for (p = parent->children.begin(); p != parent->children.end(); )
        if (*p == child)
            p = parent->children.erase(p);
        else
            ++p;
}

void EbnfCompiler::optimize(Node *node, EbnfStructure *ebnf)
{
    if (node == NULL)
        return;
    bool toDeleteNode = false;

    // Сначала оптимизируем поддеревья (иначе потом мы их не достанем)
    for (int i = 0; i < int(node->children.size()); i++)
        optimize(node->children[i], ebnf);

    // * Заменяем конкатенации, всегда возвращающие false, на обычные false-узлы:
    // * false,a -> false или a,false -> false
    // ....

    // * Удаляем лишние true-узлы в конкатенациях:
    // * true,a -> a или a,true -> a
    if (node->type == OPERATOR && node->concr == opConcatenate && node->children.size() > 1)
    {
        for (int i = 0; i < int(node->children.size()); )
        {
            Node *child = node->children[i];
            if (child->type == TRUEC)
            {
                removeChild(node, child);
                child->parent = NULL;
                delete child;
            } else
                i++;
        }
    }

    // * Удаляем альтернативы и конкатенации с одним потомком:
    // * |a -> a или ,a -> a
    if (node->type == OPERATOR &&
        (node->concr == opAlternative || node->concr == opConcatenate) &&
        node->children.size() == 1)
    {
        if (node == ebnf->root())
            ebnf->setRoot(node->children[0], false);
        node->children[0]->parent = node->parent;
        removeChild(node, node->children[0]);
        if (node->parent)
        {
            node->parent->children.push_back(node->children[0]);
            removeChild(node->parent, node);
        }
        toDeleteNode = true;
    }

    // * Убираем вложенные альтернативы:
    // * a|(b|c) -> a|b|c
    if (node->type == OPERATOR && node->concr == opAlternative &&
        node->parent != NULL &&
        node->parent->type == OPERATOR && node->parent->concr == opAlternative)
    {
        removeChild(node->parent, node);
        for (int i = 0; i < int(node->children.size()); i++)
            node->parent->children.push_back(node->children[i]);
        toDeleteNode = true;
    }


    // * Удаляем повторяющиеся узлы, заменяем их на единый:
    // * уменьшает расход памяти, убирает избыточные узлы
    // ....

    // Повторно оптимизируем поддеревья (иначе встречается баг, который я никак не выловлю:
    // некоторые true-узлы в конкатенациях остаются)
    for (int i = 0; i < int(node->children.size()); i++)
        optimize(node->children[i], ebnf);

    // Удаляем ненужный узел
    if (toDeleteNode)
        delete node;
}


