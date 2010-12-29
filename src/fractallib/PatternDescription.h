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
#ifndef PATTERNSTRUCTURE_H
#define PATTERNSTRUCTURE_H

#include "ParseTree.h"
#include "SynonymsTable.h"
#include "TimeSeries.h"
#include "PatternParameter.h"
#include "UniqueNamer.h"
#include "io/AbstractCompiler.h"

/*! \addtogroup FLAlgorithm
  */
/*@{*/

namespace FL {
    //! \namespace Patterns Contains definitions of pattern's structure and guards
    namespace Patterns {

/*! \class CheckContext
  * \brief Information about extracted structure
  */
class CheckContext
{
public:
    //! Default constructor
    CheckContext(ParseTree *tree,
                 ParseTree::ConstLayer *roots,
                 ParseTree::ConstLayer::const_iterator &iRoot,
                 ParseTree::Layer &modification,
                 ParseTreeNode *&candidateNode,
                 ParseTree::Layer &lastParsed,
                 SynonymsTable *synonyms,
                 Patterns::ParameterSet *parameters,
                 TimeSeries *ts)
        : tree(tree), roots(roots), iRoot(iRoot), modification(modification),
          candidateNode(candidateNode), lastParsed(lastParsed),
          synonyms(synonyms), parameters(parameters),  ts(ts)
    {
    }

    //! Parse tree
    ParseTree *tree;

    //! Analysing level
    ParseTree::ConstLayer *roots;

    //! Analysing layer position
    ParseTree::ConstLayer::const_iterator &iRoot;

    //! Parsed nodes sequence (means new founded patterns)
    ParseTree::Layer &modification;

    //! Currently checking node
    ParseTreeNode *&candidateNode;

    //! Last extracted nodes sequence
    ParseTree::Layer &lastParsed;

    //! Size of extracted sequence
    int patternSize;

    //! Iterator pointed to last node of extracted sequence
    ParseTree::Layer::const_iterator lastNode;

    //! Table of synonyms
    SynonymsTable *synonyms;

    //! Set of parameters
    Patterns::ParameterSet *parameters;

    //! Analysing time series
    TimeSeries *ts;

    //! Return all nodes from vector with specified parameteres
    std::vector<ParseTreeNode*>& getNodes(int nameId, int no, ParseTree::Layer &nodes) const;

    //! Return all nodes from lastParsed with specified parameteres
    std::vector<ParseTreeNode*>& getNodes(int nameId, int no) const;

    //! Return first node from nodes with specified parameteres
    ParseTreeNode* getNode(int nameId, int no, ParseTree::Layer &nodes) const;

    //! Return first node from lastParsed with specified parameteres
    ParseTreeNode* getNode(int nameId, int no) const;
private:
    //! Temporary result, used by getNode and getNodes
    mutable std::vector<ParseTreeNode*> m_cashedNodes;
};

/*! \class DescriptionStructure
  * \brief Abstract description for pattern's structure
  */
class DescriptionStructure
{
protected:
    std::string m_name;
    int m_id;
public:
    //! Destructor
    /*! Destroy nothing. Just to be polymorphic.
      */
    virtual ~DescriptionStructure() {}

    //! Get pattern's name
    const std::string& name() const { return m_name; }

    //! Set pattern's name. It also set ID
    void setName(const std::string &value)
    {
        m_name = value;
        m_id = FL::UniqueNamer::id_of_name(value);
    }

    //! Get pattern's ID
    int id() const { return m_id; }
};

/*! \class DescriptionChecker
  * \brief Abstract checker for Description's structure
  */
class DescriptionChecker
{
public:
    //! Constructor
    /*! Throught it good idea to pass structure to check in check(CheckContext &context)
      * method its passed to constructor only once to not make dynamic cast every time
      * check() is called.
      * So children HAVE to call setStructure() in constructor.
      * \param structure Structure that checker will check
      */
    DescriptionChecker(DescriptionStructure *structure)
    {
        structure = NULL;
    }

    //! Set the structure to check
    /*! Inherited class have to check type of structure, cast it and remember it.
      */
    virtual void setStructure(DescriptionStructure *structure) = 0;

    //! Get the structure that is chacking in checker
    /*! Inherited class have to return cashed value.
      */
    virtual DescriptionStructure* structure() = 0;

    //! Virtual destructor
    virtual ~DescriptionChecker() {}

    //! Check if proper structure can be extracted in current point of parse context
    /*! \param context current parsing context
      * \return true if structure can be extracted, false otherwise.
      */
    virtual bool check(CheckContext &context) = 0;
};

/*! \class DescriptionCompiler
  * \brief Abstract compiler of pattern's structure
  */
class DescriptionCompiler
{
public:
    virtual ~DescriptionCompiler() {}

    //! Compile source text to description structure
    virtual bool compile(const std::string &text, DescriptionStructure& description) = 0;

    FL::Compilers::Position errorPos;
    std::string errorDescription;
};


/*! \class Description
  * \brief Describe symbolic structure of pattern
  */
class Description
{
private:
    DescriptionStructure *m_structure;
    DescriptionChecker *m_checker;
    DescriptionCompiler *m_compiler;
    std::string m_sourceText;
public:
    //! Default constructor
    /*! \param structure Structure of description, child of FL::Patterns::DescriptionStructure class
      * \param checker Checker of description, child of FL::Patterns::DescriptionChecker class
      * \param compiler Compiler of description, child of FL::Patterns::DescriptionCompiler class
      */
    Description(DescriptionStructure *structure = NULL,
                DescriptionChecker *checker = NULL,
                DescriptionCompiler *compiler = NULL)
    {
        m_structure = structure;
        m_checker = checker;
        m_compiler = compiler;
    }

    //! Destructor
    virtual ~Description()
    {
        if (m_structure)
            delete m_structure;
        if (m_checker)
            delete m_checker;
        if (m_compiler)
            delete m_compiler;
    }

    //! Get description's structure
    DescriptionStructure* structure() { return m_structure; }

    //! Get checker for this structure class
    DescriptionChecker* checker() { return m_checker; }

    //! Get compiler for this structure class
    DescriptionCompiler* compiler() { return m_compiler; }

    //! Get human-readable text of structure
    std::string sourceText() { return m_sourceText; }

    //! Set human-readable text of structure
    /*! \param value New source text
      * \param compile If true than new text will be compiled to this description (only if compiler set)
      * \return True if (compile is false) or (compile is true and compilation compilted successful)
      */
    bool setSourceText(const std::string &value, bool compile = false)
    {
        m_sourceText = value;
        if (compile && m_compiler != NULL)
            return m_compiler->compile(value, *m_structure);
        return true;
    }

    //! Shortcut to checker()->check()
    /*! If checker not set, FL::EAnalyze will be thrown
      */
    bool check(CheckContext &context)
    {
        if (m_checker != NULL)
            return m_checker->check(context);
        else {
            GError(GCritical, "PatterDescription", 0, "Description checker not set");
            return false;
        }
    }

    //! Getter of structure's name
    std::string name() { return m_structure->name(); }

    //! Setter for structure's name
    void setName(std::string value) { m_structure->setName(value); }

    //! Get pattern's ID
    int id() const { return m_structure->id(); }
};



    } // namespace Patterns
} // namespace FL

/*@}*/

#endif // PATTERNSTRUCTURE_H
