#include "TreeMatcher.h"
#include "..\..\Common.h"
#include "..\..\trees\Layer.h"

using namespace FL::Patterns;
using namespace FL::Patterns::Standard;
using namespace FL::Trees;

TreeMatcher::TreeMatcher(PatternsSet &patterns)
    : m_root(NULL)
{
    buildTree(patterns);
}

TreeMatcher::~TreeMatcher()
{
    clearTree(m_root);
}

void TreeMatcher::buildTree(PatternsSet &patterns)
{
    if (m_root == NULL)
        m_root = new TMNode(IDGenerator::NONE);

    m_treeDepth = 0;
    PatternsSet::Iterator pattern;
    forall(pattern, patterns)
    {
        CISet::iterator sequence;
        forall(sequence, (*pattern)->description()->sequences())
        {
            addTreePath(**pattern, *sequence);
            if ((int)sequence->size() > m_treeDepth)
                m_treeDepth = sequence->size();
        }
    }
}

void TreeMatcher::addTreePath(Pattern &pattern, CISequence &seq)
{
    if (seq.size() == 0)
        return;
    addTreePath(pattern, seq, m_root, 0);
}

void TreeMatcher::addTreePath(
    Pattern &pattern,
    CISequence &seq,
    TMNode *node,
    int depth)
{
    // End of recursion? - mark node with pattern
    if (depth == (int) seq.size())
    {
        //node->patterns.push_back(&pattern);
        node->sequences.push_back(
                    CheckInfo::ApplicableSeq(&seq, &pattern));
        return;
    }

    int symbol = seq[depth].id;
    TMNode *nextNode;

    // Find child with same id
    TMChildren::iterator iNode = node->children.find(symbol);
    if (iNode != node->children.end())
        nextNode = iNode->second;
    else
        node->children[symbol] = nextNode = new TMNode(symbol);

    addTreePath(pattern, seq, nextNode, depth+1);
}

void TreeMatcher::clearTree(TreeMatcher::TMNode *root)
{
    if (root != NULL)
    {
        TMChildren::iterator child;
        forall(child, root->children)
            clearTree(child->second);
        delete root;
        if (root == this->m_root)
            m_root = NULL;
    }
}

bool TreeMatcher::match(Context &context, CheckInfo &ci)
{

    Layer::ConstIterator itLayerNode = context.currentRoot();
    TMNode *treeNode = m_root;
    TMChildren::const_iterator itNextNode;

    while (treeNode != NULL &&
           itLayerNode != context.roots().end())
    {
        // Found some patterns? - check guards and add them to result
        std::vector<CheckInfo::ApplicableSeq>::iterator itAs;
        forall(itAs, treeNode->sequences)
        {
            context.buildLastParsed(*itAs->seq);
            if (itAs->pattern->guard()->check(context))
                ci.applicableSequences.push_back(*itAs);
        }

        // If exists edge with specified symbol
        // then get to node pointed by this edge

        itNextNode = treeNode->children.find((*itLayerNode)->id());

        if (itNextNode != treeNode->children.end())
        {
            treeNode = itNextNode->second;
            ++itLayerNode;
        }
        else
            treeNode = NULL;
    }

    // End of root sequence reached, but there are completed pattern
    if (treeNode != NULL)
    {
        std::vector<CheckInfo::ApplicableSeq>::iterator itAs;
        forall(itAs, treeNode->sequences)
        {
            context.buildLastParsed(*itAs->seq);
            if (itAs->pattern->guard()->check(context))
                ci.applicableSequences.push_back(*itAs);
        }
    }

    return ci.applicableSequences.size() > 0;
}


















