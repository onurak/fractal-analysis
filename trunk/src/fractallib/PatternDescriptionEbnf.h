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

#ifndef PATTERN_STRUCTURE_EBNF_H
#define PATTERN_STRUCTURE_EBNF_H

#include <vector>
#include "PatternDescription.h"

namespace FL {

namespace Patterns {

//! \namespace EBNF Contains pattern in Extended Backus–Naur Form
namespace EBNF {

typedef FL::ParseTreeNode Elem;

/*! \class Expression
  * \brief Sequence of analizing symbols
  */
struct Expression
{
    std::vector<Elem*> items; //!< Symbols for analysis
    int pos; //!< position in items
    Elem *current; //!< shortcut to current analyzing element, same as (items[pos] : NULL)

    //! Got to the first analyzing element
    void first() { pos = -1; current = NULL; }

    //! Got to the next analyzing element
    void next() { current = ++pos >= int(items.size()) ? NULL : items[pos]; }

    //! Got to the concreet analyzing element
    void restore(int pos) {
        this->pos = pos;
        current = pos >= int(items.size()) || pos < 0 ? NULL : items[pos];
    }

    //! Check if it's the last element
    bool empty() { return pos >= int(items.size()); }
};

// Tree node types
static const int FALSEC = -1; //!< false-node
static const int TRUEC = 0;   //!< true-node
static const int NAME = 1;    //!< name-name
static const int OPERATOR = 2;//!< operator-node

// Operations types
static const int opAlternative = 1; //!< Alternative: |
static const int opConcatenate = 2; //!< Concatenation: ,
static const int opRepeat      = 3; //!< Repeat: *
static const int opOptional    = 4; //!< Option: -

/*! \class Node
  * \brief Node of EBNF tree
  */
struct Node
{
    //! Default constructor
    Node(int type = 0, int concr = 0): type(type), concr(concr), parent(NULL) {}

    int type; //!< Node type
    int concr; //!< Node subtype
    std::vector<Node*> children; //!< Children of node
    Node* parent;  //!< Parent of node
};

/*! \class EbnfStructure
  * \brief Contains EBNF tree, methods for checking if expression holded in tree
  *
  */
class EbnfStructure: public DescriptionStructure
{
public:
    /*! \struct TableElement
      * \brief Element of name table of EBNF tree
      *
      * Contains record about single used name
      */
    struct TableElement
    {
        std::string name; //!< element's name
        int nameId; //!< element name's id
        int no;      //!< element's no
        std::string indexName; //!< element's index name (if any)

        //! Shortcut method
        /*! \param name new name
          * \param no new no
          */
        void set(int nameId, int no)
        {
            this->nameId = nameId;
            this->no = no;
            this->name = UniqueNamer::name_of_id(nameId);
        }
    };

public:
    //! Default constuctor
    /*! \param newRoot Root of tree
      */
    EbnfStructure(Node *newRoot = NULL);

    //! Destructor
    /*! Completely destroy tree
      */
    virtual ~EbnfStructure();

    //! Destroy subtree
    /*! \param node root of subtree to delete
      */
    void remNode(const Node *node)
    {
        if (node != NULL)
        {
            std::vector<Node*>::const_iterator p;
            for (p = node->children.begin(); p != node->children.end(); p++)
                remNode(*p);
            delete node;
        }
    }

    //! Get tree's root
    /*! \return Pointer to root of EBNF tree */
    Node* root() { return m_root; }

    //! Set new tree
    /*! \param newRoot new root of tree
      * \param deleteOldRoot if true old tree will be deleted
      */
    void setRoot(Node* newRoot, bool deleteOldRoot = true)
    {
        if (deleteOldRoot)
            remNode(m_root);
        m_root = newRoot;
    }

    //! Name table of EBNF tree
    std::vector<TableElement*> table;

    //! Get name's index in name table (if it's not exists, it will be created)
    /*! \param name Name to find
      * \param no Index to find
      * \return Index of name
      */
    int getOrCreateName(const std::string &name, int no = -1)
    {
        int nameId = UniqueNamer::id_of_name(name);
        for (int i = 0; i < int(table.size()); i++)
            if (table[i]->nameId == nameId && table[i]->no == no)
                return i;
        TableElement *elem = new TableElement();
        elem->set(nameId, no);
        elem->indexName = "";
        table.push_back(elem);
        return table.size() - 1;
    }

    //! Print EBNF tree
    /*! \param root Root of subtree to print
      * \param level Level of subtree (just leave it 0 for root)
      * \return string with printed tree
      */
    std::string print(Node *root, int level = 0);

    //! Print EBNF tree starting with root
    std::string print()
    {
        return print(m_root, 0);
    }

    //! Maximum length of EBNF
    int maxLength()
    {
        // Fix it!
        // It must be calculated for each pattern, hope no one will use patterns longer than 10
        return 10;
    }

    friend class EbnfChecker;
private:
    //! Tree root
    Node *m_root;
};

/*! \class EbnfChecker
  * \brief Checker for EbnfStructure
  *
  * Algorithm:
  * \code
  * bool rcheck(Expression *expr, Node *node)
  * {
  *     if (node is leaf) {
  *         if (expr->next_name == node->name_in_node)
  *             return true;
  *         return false;
  *     } else if (node is operation) {
  *         switch (node->operation_type) {
  *             case CONCATENATION:
  *                 save_current_position_in_expr();
  *                 for each child in node {
  *                     if (!rcheck(expr, node)) {
  *                         restore_saved_position_in_expr();
  *                         return false;
  *                     }
  *                 }
  *                 return true;
  *             case ALTERNATIVE:
  *                 save_current_position_in_expr();
  *                 for each child in node
  *                     if (check(expr, node))
  *                         return true;
  *                 restore_saved_position_in_expr();
  *                 return false;
  *             и т.д.
  *     }
  * }
  * \endcode
  * Than checking of expr will be successful if rcheck will return true and expr will be at end.
  */
class EbnfChecker: public DescriptionChecker
{
public:
    /*! \enum CheckOptions
      * \brief Check option
      */
    enum CheckOptions {
        //! Reset flags
        coNothing              = 0x0,

        //! If this flag is on, than check result will be successfull only if whole expression
        //! will be parsed, otherwise only first part can be parsed
        coParseWholeExpression = 0x1
    };
public:
    //! Constructor
    EbnfChecker(DescriptionStructure *structure)
        : DescriptionChecker(structure)
    {
        setStructure(structure);
    }

    //! Set structure to check
    /*! Structure must have type EbnfStructure
      */
    virtual void setStructure(DescriptionStructure *structure)
    {
        m_structure = dynamic_cast<EbnfStructure*>(structure);
        if (m_structure == NULL)
            GError(GCritical, "EbnfChecker", 0, "Invalid structure passed");
    }

    //! Get checking structure
    virtual DescriptionStructure* structure()
    {
        return m_structure;
    }

    //! Main checking function
    /*! \copydoc FL::Patterns::DescriptionChecker::check(CheckContext &context)
      */
    virtual bool check(CheckContext &context);

    //! Check if Expression satisfy EBNF tree
    /*! \param expr Expression to check
      * \param synonyms Synonims table
      * \param options Check options
      * \return true if expr staify EBNF tree, false otherwise
      */
    bool check(Expression *expr, const SynonymsTable &synonyms, CheckOptions options = coParseWholeExpression);

protected:
    //! Рекурсивная функция проверки принадлежности выражения поддереву
    /*! \param expr выражение, которое необходимо проверить на принадлежность (принадлежать может лишь его часть)
      * \param synonyms таблица синонимов
      * \param node узел, начиная с которого необходимо проверить проверку
      * \return если хотя бы чать выражения приналежит EBNF-дереву, то true, иначе false
      */
    bool rcheck(Expression *expr, const SynonymsTable &synonyms, Node *node);
private:
    //! Stored description structure
    EbnfStructure *m_structure;

    //! Current check options
    CheckOptions m_options;
};

/*! \class EbnfCompiler
  * \brief Compiler of EBNF expressions
  *
  * Grammar:
  * \code
  * имя          = символ {символ | цифра};
  * идк_имя      = имя "_" [число];
  * бнф          = имя '=' выражение;
  * выражение    = терм  {"|" терм};
  * терм         = фактор >> {фактор};
  * фактор       = идк_имя | "(" выражение ")" | "[" выражение "]";
  * \endcode
  * Операция повторения {} отсутствует в грамматике т.к. она не используется в теоретическом описании.
  *
  * После компиляции дерево оптимизируется. Подробнее об оптимизации можно почитать в optimize(Node*, Ebnf*).
  */
class EbnfCompiler: public DescriptionCompiler
{
public:
    //! Compile string to EBNF tree
    /*! \copydoc FL::Patterns::DescriptionCompiler::compile(const std::string &text, DescriptionStructure& description)
      */
    virtual bool compile(const std::string &text,
                         DescriptionStructure& description);

    //! Optimize EBNF tree
    /*! На данный момент оптимизируются следующие случаи:
      *   - удаляются лишние true-узлы в конкатенациях: true,a -> a или a,true -> a
      *   - удаляются альтернативы и конкатенации с одним потомком: |a -> a или ,a -> a
      *   - убираются вложенные альтернативы: a|(b|c) -> a|b|c
      * \param node узел в дереве, начиная с которого необходимо выполнить оптимизацию
      * \param ebnf которому принадлежит узел
      */
    void optimize(Node *node, EbnfStructure *ebnf);
};

} // namespace
} // namespace
} // namespace

#endif // PATTERN_STRUCTURE_EBNF_H
