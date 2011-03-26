#include "Context.h"
#include "../trees/Tree.h"

using namespace FL::Patterns;
using namespace FL::Trees;

Context::Context()
    : m_currentRootPos(0)
{
    m_cache = new Layer();
    m_lastParsed = new Trees::Layer();
}

Context::~Context()
{
    delete m_cache;
    delete m_lastParsed;
}

const Layer& Context::getNodes(int nameId, int no) const
{
    m_cache->clear();
    return *m_cache;
}

Node* Context::getNode(int nameId, int no) const
{
    Node *node = NULL;

    return node;
}

const std::list<Node*>::const_iterator& Context::currentRoot()
{
    return m_currentRoot;
}

void Context::buildLastParsed(const CISequence& seq)
{
    m_lastParsed->clear();
    Trees::Layer::const_iterator node = currentRoot();
    CISequence::const_iterator cinode = seq.begin();

    while (cinode != seq.end() && node != roots().end())
    {
        (*node)->setIndex(cinode->index);
        m_lastParsed->push_back(*node);
        ++node; ++cinode;
    }
}

void Context::anvanceCurrentRoot(int step)
{
    m_currentRoot += step;
    m_currentRootPos += step;
}

void Context::setParseTree(Trees::Tree *tree)
{
    m_parseTree = tree;
    m_roots.assign(tree->roots().begin(), tree->roots().end());
    m_currentRoot = m_roots.begin();
}

//! Analyzing roots of tree
//std::list<Trees::Node*> Context::roots()
//{

//}
