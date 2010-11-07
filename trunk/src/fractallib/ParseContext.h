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

#ifndef PARSECONTEXT_H
#define PARSECONTEXT_H

#include "ParseTree.h"
#include "TimeSeries.h"
#include "SynonymsTable.h"
#include "ParseResult.h"
#include "Pattern.h"

namespace FL {

class ParseTree;
class PatternCollection;

/*! \addtogroup FLAlgorithm
  */
/*@{*/

/*! \class ParseContext
  * \brief Contains all information about current state of analyse process
  */
class ParseContext
{
public:    
    //! Default constructor
    /*! \param treeSet Tree set currently used in parsing
      * \param tree Current parse tree (this tree is now modifing)
      * \param ts Time series wich is under analysis
      * \param patterns Set of patterns used in p[arsing
      * \param roots Current layer of roots
      * \param synonyms Synonyms table
      * \param parameters Parameters set
      * \param result Current results of parse process iteration
      */
    ParseContext(ParseTreeSet *treeSet = NULL, ParseTree *tree = NULL, TimeSeries *ts = NULL,
                 PatternCollection *patterns = NULL, ParseTree::ConstLayer *roots = NULL,
                 SynonymsTable *synonyms = NULL, Patterns::ParameterSet *parameters = NULL,
                 ParseResult *result = NULL)
        : treeSet(treeSet), tree(tree), ts(ts), roots(roots),
          synonyms(synonyms), parameters(parameters), patterns(patterns),
          result(result)
    {
        m_ownRoots = false;
        if (roots)
            iRoot = roots->begin();
        // create check context
        cc = new FL::Patterns::CheckContext(
                roots, iRoot, modification, lastParsed, synonyms, parameters, ts);
    }

    //! Copy constructor
    /*! Tree set, synonyms table and time series are copied as pointers.
      * All other fields (current parse tree, roots, pointer to current node)
      * are deeply copied.
      *
      * \param c Context to copy
      */
    ParseContext(ParseContext &c);

    //! Destructor
    /*! if roots are owned by this context they will be destroyed
      */
    ~ParseContext()
    {
        if (m_ownRoots && roots)
            delete roots;
        if (cc)
            delete cc;
    }

    //! Current tree set
    ParseTreeSet *treeSet;

    //! Current analyzed parse tree
    ParseTree *tree;

    //! Current analyzed time series
    TimeSeries *ts;

    //! Current analyzed layer of nodes
    ParseTree::ConstLayer *roots;

    //! Pointer to current analyzed element of roots
    ParseTree::ConstLayer::const_iterator iRoot;

    //! Last recognized sequence. Need by guards
    ParseTree::Layer lastParsed;

    //! Synonyms table
    SynonymsTable *synonyms;

    //! Parameters set
    Patterns::ParameterSet *parameters;

    //! Fractal patterns set
    PatternCollection *patterns;

    //! Result of parsing
    ParseResult *result;

    //! Parsed nodes sequence (means new founded patterns)
    ParseTree::Layer modification;

    //! Properly customed CheckContext
    FL::Patterns::CheckContext *cc;
    
    //! Return all elements from lastParsed with specified parameteres
    inline std::vector<ParseTreeNode*>& getNodes(const std::string &name, int no) const
    {
        m_cashedNodes.clear();
        ParseTree::Layer::const_iterator i;
        if (name != "*")
        {
            for (i = lastParsed.begin(); i != lastParsed.end(); ++i)
                if ((*i)->no == no && (*i)->name == name)
                    m_cashedNodes.push_back(*i);
        }
        else
        {
            for (i = lastParsed.begin(); i != lastParsed.end(); ++i)
                if ((*i)->no == no)
                    m_cashedNodes.push_back(*i);
        }
        return m_cashedNodes;
    }

    //! Return first element from lastParsed with specified parameteres
    inline ParseTreeNode* getNode(const std::string &name, int no) const
    {
        ParseTree::Layer::const_iterator i;
        for (i = lastParsed.begin(); i != lastParsed.end(); ++i)
            if ((*i)->no == no && (*i)->name == name)
                return *i;
        return NULL;
    }
private:
    //! Temporary result, used by getNode and getNodes
    mutable std::vector<ParseTreeNode*> m_cashedNodes;

    //! Show if roots is owned by instance and must be destroyed with it
    bool m_ownRoots;
};

/*@}*/

} // namespace

#endif // PARSECONTEXT_H
