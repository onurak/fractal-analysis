#include "DescriptionCompilerEbnf.h"
#include "../Context.h"
#include "../../compilers/AbstractCompiler.h"
#include "../../trees/Tree.h"

using namespace FL::Patterns::Standard;
using namespace FL::Exceptions;
using namespace FL::Compilers;

namespace FL { namespace Patterns { namespace Standard { namespace Internal {

    /////////////////////////////////////////////////////////////
    // SECTION: EBNF TREE STRUCTURE
    /////////////////////////////////////////////////////////////

//    typedef FL::Trees::Node Elem;

//    /*! \class Expression
//      * \brief Sequence of analizing symbols
//      */
//    struct Expression
//    {
//        std::vector<Elem*> items; //!< Symbols for analysis
//        int pos; //!< position in items
//        Elem *current; //!< shortcut to current analyzing element, same as (items[pos] : NULL)

//        //! Got to the first analyzing element
//        void first() { pos = -1; current = NULL; }

//        //! Got to the next analyzing element
//        void next() { current = ++pos >= int(items.size()) ? NULL : items[pos]; }

//        //! Got to the concreet analyzing element
//        void restore(int pos)
//        {
//            this->pos = pos;
//            current = pos >= int(items.size()) || pos < 0 ? NULL : items[pos];
//        }

//        //! Check if it's the last element
//        bool empty() { return pos >= int(items.size()); }
//    };

//    class EbnfTreeNode;
//    class EbnfTree;
//    std::string nodeToString(EbnfTreeNode *node, EbnfTree *ebnf);

//    // Tree node types
//    const int FALSEC = -1;  //!< false-node
//    const int TRUEC = 0;    //!< true-node
//    const int NAME = 1;     //!< name-name
//    const int OPERATOR = 2; //!< operator-node

//    // Operations types
//    const int opAlternative = 1; //!< Alternative: |
//    const int opConcatenate = 2; //!< Concatenation: ,
//    const int opRepeat      = 3; //!< Repeat: *
//    const int opOptional    = 4; //!< Option: -

//    /*! \class CINode
//      * \brief Node of EBNF tree
//      */
//    struct EbnfTreeNode
//    {
//        //! Default constructor
//        EbnfTreeNode(int type = 0, int concr = 0): type(type), concr(concr), parent(NULL) {}
//        int type;                        //!< Node type
//        int concr;                       //!< Node subtype
//        std::vector<EbnfTreeNode*> children; //!< Children of node
//        EbnfTreeNode* parent;                //!< Parent of node
//    };


//    /*! \class EbnfTree
//      * \brief Contains EBNF tree, methods for checking if expression holded in tree
//      */
//    class EbnfTree
//    {
//    public:
//        /*! \struct TableElement
//          * \brief Element of name table of EBNF tree
//          *
//          * Contains record about single used name
//          */
//        struct TableElement
//        {
//            std::string name;      //!< element's name
//            int no;                //!< element's no
//            std::string indexName; //!< element's index name (if any)

//            //! Shortcut method
//            /*! \param name new name
//              * \param no new no
//              */
//            void set(const std::string &name, int no)
//            {
//                this->name = name;
//                this->no = no;
//            }
//        };

//    public:
//        //! Default constuctor
//        /*! \param newRoot Root of tree
//          */
//        EbnfTree(EbnfTreeNode *newRoot = NULL)
//            : m_root(newRoot)
//        {

//        }

//        //! Destructor
//        /*! Completely destroy tree
//          */
//        virtual ~EbnfTree()
//        {
//            remNode(m_root);
//        }

//        //! Destroy subtree
//        /*! \param node root of subtree to delete
//          */
//        void remNode(const EbnfTreeNode *node)
//        {
//            if (node != NULL)
//            {
//                std::vector<EbnfTreeNode*>::const_iterator p;
//                forall(p, node->children)
//                    remNode(*p);
//                delete node;
//            }
//        }

//        //! Get tree's root
//        /*! \return Pointer to root of EBNF tree */
//        EbnfTreeNode* root()
//        {
//            return m_root;
//        }

//        //! Set new tree
//        /*! \param newRoot new root of tree
//          * \param deleteOldRoot if true old tree will be deleted
//          */
//        void setRoot(EbnfTreeNode* newRoot, bool deleteOldRoot = true)
//        {
//            if (deleteOldRoot)
//                remNode(m_root);
//            m_root = newRoot;
//        }

//        //! Name table of EBNF tree
//        std::vector<TableElement*> table;

//        //! Get name's index in name table (if it's not exists, it will be created)
//        /*! \param name Name to find
//          * \param no Index to find
//          * \return Index of name
//          */
//        int getOrCreateName(const std::string &name, int no = -1)
//        {
//            for (int i = 0; i < int(table.size()); i++)
//                if (table[i]->name == name && table[i]->no == no)
//                    return i;
//            TableElement *elem = new TableElement();
//            elem->set(name, no);
//            elem->indexName = "";
//            table.push_back(elem);
//            return table.size() - 1;
//        }

//        //! Print EBNF tree
//        /*! \param root Root of subtree to print
//          * \param level Level of subtree (just leave it 0 for root)
//          * \return string with printed tree
//          */
//        std::string print(EbnfTreeNode *root, int level = 0)
//        {
//            if (root != NULL)
//            {
//                std::string result = "";
//                // left children
//                for (int i = 0; i < int(root->children.size() / 2); i++)
//                    result += print(root->children[i], level+1);

//                // spaces and self
//                for (int i = 0; i < level; i++)
//                    result += "\t";
//                result += nodeToString(root, this) + "\n";

//                // right children
//                for (int i = root->children.size() / 2; i < int(root->children.size()); i++)
//                    result += print(root->children[i], level+1);

//                return result;
//            }
//            return "";
//        }

//        //! Print EBNF tree starting with root
//        std::string print()
//        {
//            return print(m_root, 0);
//        }

//        //! Maximum length of EBNF
//        int maxLength()
//        {
//            // Fix it!
//            // It must be calculated for each pattern, hope no one will use patterns longer than 10
//            return 10;
//        }

//        friend class EbnfChecker;
//    private:
//        //! Tree root
//        EbnfTreeNode *m_root;
//    };

//    std::string nodeToString(EbnfTreeNode *node, EbnfTree *ebnf)
//    {
//        if (node == NULL)
//            return "(NULL)";
//        std::string result = "(";
//        switch (node->type)
//        {
//            case NAME:
//                if (ebnf != NULL)
//                    if (node->concr >= 0 && node->concr < int(ebnf->table.size()))
//                        result += ebnf->table[node->concr]->name;
//                    else
//                        result += "err";
//                else
//                    result += "u";
//                break;
//            case OPERATOR:
//                switch (node->concr)
//                {
//                    case opAlternative:
//                        result += "|"; break;
//                    case opConcatenate:
//                        result += ","; break;
//                    case opOptional:
//                        result += "-"; break;
//                    case opRepeat:
//                        result += "*"; break;
//                    default:
//                        result += "u"; break;
//                }
//                break;
//            case TRUEC:
//                result += "true";
//                break;
//            case FALSEC:
//                result += "false";
//                break;
//            default:
//                result += "U";
//                break;
//        }
//        return result + ")";
//    }


//    /*! \class EbnfChecker
//      * \brief Checker for EbnfStructure
//      *
//      * Algorithm:
//      * \code
//      * bool rcheck(Expression *expr, Node *node)
//      * {
//      *     if (node is leaf) {
//      *         if (expr->next_name == node->name_in_node)
//      *             return true;
//      *         return false;
//      *     } else if (node is operation) {
//      *         switch (node->operation_type) {
//      *             case CONCATENATION:
//      *                 save_current_position_in_expr();
//      *                 for each child in node {
//      *                     if (!rcheck(expr, node)) {
//      *                         restore_saved_position_in_expr();
//      *                         return false;
//      *                     }
//      *                 }
//      *                 return true;
//      *             case ALTERNATIVE:
//      *                 save_current_position_in_expr();
//      *                 for each child in node
//      *                     if (check(expr, node))
//      *                         return true;
//      *                 restore_saved_position_in_expr();
//      *                 return false;
//      *             etc.
//      *     }
//      * }
//      * \endcode
//      * Than checking of expr will be successful if rcheck will return true and expr will be at end.
//      */
//    class EbnfTreeChecker
//    {
//    public:
//        /*! \enum CheckOptions
//          * \brief Check option
//          */
//        enum CheckOptions
//        {
//            //! Reset flags
//            coNothing              = 0x0,

//            //! If this flag is on, than check result will be successfull only if whole expression
//            //! will be parsed, otherwise only first part can be parsed
//            coParseWholeExpression = 0x1
//        };
//    public:
//        //! Constructor
//        EbnfTreeChecker()
//        {
//        }

//        //! Set structure to check
//        /*! Structure must have type EbnfStructure
//          */
//        void setTree(EbnfTree *tree)
//        {
//            m_tree = tree;
//        }

//        //! Main checking function
//        bool check(FL::Patterns::Context &context)
//        {
//            // Create expression for check
//            Expression expr;
//            std::string exprString;
//            context.lastNode = context.iRoot;
//            for (int i = 0;
//                 i < m_structure->maxLength() && context.lastNode != context.roots->end();
//                 ++i, ++(context.lastNode))
//            {
//                expr.items.push_back(*context.lastNode);
//                exprString += (*context.lastNode)->name + " ";
//            }
//            logg.debug("    Expression to check: ") << exprString;
//            if (check(&expr, *context.synonyms, coNothing))
//            {
//                // fill 'last parsed' sequence
//                context.lastParsed.clear();
//                context.patternSize = expr.pos;
//                context.lastNode = context.iRoot;
//                while (expr.pos > 1)
//                {
//                    expr.pos--;
//                    context.lastParsed.push_back(*context.lastNode);
//                    ++context.lastNode;
//                }
//                context.lastParsed.push_back(*context.lastNode);
//                return true;
//            }
//            return false;
//        }

//        //! Check if Expression satisfy EBNF tree
//        /*! \param expr Expression to check
//          * \param synonyms Synonims table
//          * \param options Check options
//          * \return true if expr staify EBNF tree, false otherwise
//          */
//        bool check(Expression *expr, const SynonymsTable &synonyms,
//                   CheckOptions options = coParseWholeExpression)
//        {
//            if (m_structure->m_root == NULL || expr == NULL)
//                return false;

//            m_options = options;

//            expr->first();
//            expr->next();
//            bool r = rcheck(expr, synonyms, m_structure->m_root);

//            if (options & coParseWholeExpression)
//                r = r && !expr->empty() ? false : r;
//            return r;
//        }

//    protected:
//        //! Рекурсивная функция проверки принадлежности выражения поддереву
//        /*! \param expr выражение, которое необходимо проверить на принадлежность (принадлежать может лишь его часть)
//          * \param synonyms таблица синонимов
//          * \param node узел, начиная с которого необходимо проверить проверку
//          * \return если хотя бы чать выражения приналежит EBNF-дереву, то true, иначе false
//          */
//        bool rcheck(Expression *expr, const SynonymsTable &synonyms, CINode *node)
//        {
//            int pos;
//            std::vector<Node*>::iterator p;
//            if (node->type == NAME)
//            {
//                if (node->concr < 0 || node->concr >= int(m_structure->table.size()))
//                {
//                    GError(GCritical,
//                           "EbnfChecker",
//                           0,
//                           EInvalidIndex);
//                    return false;
//                }
//                if (expr->current != NULL &&
//                    synonyms.isSynonyms(expr->current->name, m_structure->table[node->concr]->name))
//                {
//                    expr->current->no = m_structure->table[node->concr]->no;
//                    expr->next();
//                    return true;
//                }
//                return false;
//            }
//            else if (node->type == OPERATOR)
//            {
//                switch (node->concr)
//                {
//                    // '|' - достаточно, чтобы выполнялся хотя бы один из потомков
//                    case opAlternative:
//                        pos = expr->pos;
//                        forall(p, node->children)
//                            if (rcheck(expr, synonyms, *p))
//                                return true;
//                        expr->restore(pos);
//                        return false;
//                    // ',' - необходимо, чтобы выполнялись все потомки
//                    case opConcatenate:
//                        pos = expr->pos;
//                        for (p = node->children.begin(); p != node->children.end(); p++)
//                            if (!rcheck(expr, synonyms, *p))
//                            {
//                                expr->restore(pos);
//                                return false;
//                            }
//                        return true;
//                    // '{  }' - все равно сколько раз выполняется потомок
//                    case opRepeat:
//                        while (rcheck(expr, synonyms, node->children[0]))
//                            ;
//                        return true;
//                    // '[  ]' - потомок должен выполняться 0 или 1 раз
//                    case opOptional:
//                        rcheck(expr, synonyms, node->children[0]);
//                        return true;
//                    default:
//                        //error();
//                        break;
//                }
//            }
//            else if (node->type == TRUEC)
//            {
//                return true; // узел true
//            }
//            else if (node->type == FALSEC)
//            {
//                return false; // узел false
//            } else
//                {};//error();

//            return false;
//        }
//    private:
//        //! Stored description structure
//        EbnfStructure *m_structure;

//        //! Current check options
//        CheckOptions m_options;
//    };

    /////////////////////////////////////////////////////////////
    // SECTION: EBNF COMPILER
    /////////////////////////////////////////////////////////////


    /*! \class EbnfCompiler
      * \brief Actual EBNF compiler
      *
      * Grammar:
      * \code
      * name         = alpha {alpha | digit};
      * indexed      = name "_" [uint];
      * ebnf         = name '=' expr;
      * expr         = term  {"|" term};
      * term         = factor {factor};
      * factor       = indexed | "(" expr ")" | "[" expr "]";
      * \endcode
      */
    class EbnfCompiler : public AbstractCompiler
    {
    public:
        static const int LEX_ALTERNATIVE = 5;
        static const int LEX_LOPTION     = 6;
        static const int LEX_ROPTION     = 7;
        static const int LEX_INDEXED     = 8;
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
            while (m_l == LEX_NAME   || m_l == LEX_INDEXED ||
                   m_l == LEX_LPAREN || m_l == LEX_LOPTION)
            {
                CISet r1 = factor(set);
                result = closure(result, r1);
            }
            return result;
        }

        CISet factor(CISet set)
        {
            if (m_l == LEX_NAME)
            {
                CINode node;
                node.id    = IDGenerator::idOf((char*)m_symbolsTable[m_lex.index]);
                node.index = -1;
                append(set, node);
                gl();
            }
            else if (m_l == LEX_INDEXED)
            {
                CINode node;
                node.id    = IDGenerator::idOf((char*)m_symbolsTable[m_lex.index]);
                gl();
                if (m_l != LEX_INTEGER)
                    error(E_EXPECTED_TOKEN, "Index");
                node.index = m_symbolsTable[m_lex.index];
                append(set, node);
                gl();
            }
            else if (m_l == LEX_LPAREN)
            {
                gl();
                set = expr(set);
                if (m_l != LEX_RPAREN)
                    error(E_EXPECTED_TOKEN, ")");
                gl();
            }
            else if (m_l == LEX_LOPTION)
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
            }
            else
                error(E_UNEXPECTED_TOKEN, psymbol());
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

//bool DescriptionEbnf::check(Context &c, CheckInfo &info)
//{
//    info.applicableSequences.clear();
//    CISet::iterator seq;

//    int stillSize = c.roots().size() - c.currentRootPos();


//    // Check all sequences
//    forall(seq, m_ebnfSet)
//    {
//        if ((int)(*seq).size() > stillSize)
//            continue;

//        bool found = true;
//        Trees::Layer::ConstIterator node = c.currentRoot();
//        CISequence::iterator cinode = seq->begin();

//        // Check current sequence
//        while (node   != c.parseTree().roots().end() &&
//               cinode != seq->end())
//        {
//            if ((*node)->id() != cinode->id)
//            {
//                found = false;
//                break;
//            }
//            ++node;
//            ++cinode;
//        }

//        // If it's applicable
//        if (found)
//        {
//            info.applicableSequences.push_back(&(*seq));
//        }
//    }

//    return info.applicableSequences.size() > 0;
//}
