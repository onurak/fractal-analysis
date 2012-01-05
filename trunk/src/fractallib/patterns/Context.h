#ifndef CONTEXT_H
#define CONTEXT_H

#include <vector>
#include <list>
#include "CheckInfo.h"

////////////////////////////////////////////////////////////
// SECTION: FORWARDS
////////////////////////////////////////////////////////////

namespace FL {
    class TimeSeries;

    namespace Trees {
        class Node;
        class Tree;
        class Layer;
        class Forest;
    }
}

////////////////////////////////////////////////////////////
// SECTION: CONTEXT
////////////////////////////////////////////////////////////

namespace FL { namespace Patterns {


class Context
{
public:
    //! Default constructor
    Context();

    //! Copy constructor. Create a deep copy
    Context(const Context &c);

    //! Destructor
    ~Context();

    //! Last parsed sequence of nodes
    /*! It's filled by description checking algorithm after successfull checking.
      * It mostly used in guards with getNode() and getNodes() functions for
      * speed reasons.
      */
    inline Trees::Layer& lastParsed() { return *m_lastParsed; }

    //! Fill lastParsed from sequence of nodes
    /*! \param seq Sequence that must be used to build lastParsed
      * \param nodesCount if greater then zero then only first nodesCount
      *        will be used
      */
    void buildLastParsed(const CISequence& seq, int nodesCount = 0);

    //! Get first node from last parsed sequence with specified parameters
    Trees::Node* getNode(int nameId, int index) const;

    //! Get all nodes from last parsed sequence with specified parameters
    const Trees::Layer& getNodes(int nameId, int index) const;


    //! Get current position at the analyzing sequence of roots
    const std::list<Trees::Node*>::const_iterator& currentRoot();

    //! Change position of current root
    void advanceCurrentRoot(int step);

    //! Get position to the end of roots()  (same as currentRoot() - roots().begin(),
    //! but operator- not defined for std::list::const_iterator).
    inline int currentRootPos() const { return m_currentRootPos; }


    //! Get analyzing tree
    Trees::Tree& parseTree() const { return *m_parseTree; }

    //! Set analyzing tree
    void setParseTree(Trees::Tree *tree);


    //! Get output tree where stored results of analysis
    Trees::Tree& outputTree() { return *m_outputTree; }
    const Trees::Tree& outputTree() const { return *m_outputTree; }

    //! Set output tree where stored results of analysis
    void setOutputTree(Trees::Tree *tree);


    //! Get currently checking node that passed description check and going to check in guard
    Trees::Node* candidateNode() { return m_candidateNode; }

    //! Get current checking subnode of condidateNode.
    //! Need by It() guard function.
    Trees::Node* currentItNode() { return m_currentItNode; }
    void setCurrentItNode(Trees::Node* node) { m_currentItNode = node; }

    //! Set currently checking node that passed description check and going to check in guard
    void setCandidateNode(Trees::Node* newNode) { m_candidateNode = newNode; }


    //! Analyzing roots of tree
    inline const std::list<Trees::Node*>& roots()
    {
        return m_roots;
    }

    //! Analyzing roots of tree
    inline const std::list<Trees::Node*>& roots() const
    {
        return m_roots;
    }

    //! Check if currentRoot() == roots().end()
    inline bool isAtEnd() const
    {
        return m_currentRootPos >= (int)m_roots.size();
    }

    //! Check if current root on specified time position
    bool isAtTime(int time) const;

    //! Get analyzing time series
    const FL::TimeSeries* timeSeries();

    //! Set analyzing time series
    void setTimeSeries(const FL::TimeSeries* ts);

    //! Change analysing roots
    void setRoots(const std::list<Trees::Node*> &layer);

    //! Advance root interator to specified position
    void advanceCurrentRootToPos(int pos);

    //! Nodes, added during analysis
    FL::Trees::Layer& modification();
private:
    void copyRoots(Trees::Tree *newOutputTree, const Context& c);

    //! Current forest
    Trees::Forest *m_forest;

    //! Currently analyzing parse tree
    Trees::Tree *m_parseTree;

    //! Tree that hold results of analysis of m_parseTree
    Trees::Tree *m_outputTree;

    //! Currently checking node that passed description check and going to
    //! check in guard
    Trees::Node *m_candidateNode;

    //! Current checking subnode of m_candidateNode
    Trees::Node *m_currentItNode;

    //! Analyzing roots of tree
    std::list<Trees::Node*> m_roots;

    //! Current analyzing root (from m_roots)
    std::list<Trees::Node*>::const_iterator m_currentRoot;

    //! Internal cache used by getNode() and getNodes() methods
    mutable Trees::Layer *m_cache;

    //! Last parsed sequence (\sa lastParsed())
    Trees::Layer *m_lastParsed;

    //! For fast calculation of position of current root in list
    int m_currentRootPos;

    //! Analyzing time series
    const FL::TimeSeries *m_ts;

    //! Nodes, added during analysis
    FL::Trees::Layer *m_modification;
};

}} // namespaces

#endif // CONTEXT_H
