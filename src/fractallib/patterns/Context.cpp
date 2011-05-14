#include "Context.h"
#include "../trees/Tree.h"
#include "../TimeSeries.h"

using namespace FL::Patterns;
using namespace FL::Trees;

Context::Context()
    : m_currentRootPos(0)
{
    m_cache = new Layer();
    m_lastParsed = new Layer();
    m_modification = new Layer();
}

Context::Context(const Context &c)
{
    setTimeSeries(c.m_ts);
    setParseTree(c.parseTree().copy());
    setOutputTree(c.parseTree().copy());
    setCandidateNode(new Node());
}

Context::~Context()
{
    delete m_cache;
    delete m_lastParsed;
    delete m_modification;
}

const Layer& Context::getNodes(int nameId, int no) const
{
    m_cache->clear();
    return *m_cache;
}

Node* Context::getNode(int nameId, int no) const
{
    Trees::Layer::Iterator itNode;
    forall(itNode, *m_lastParsed)
    {
        Node *node = *itNode;
        if (node->id() == nameId && node->index() == no)
            return node;
    }
    return NULL;
}

const std::list<Node*>::const_iterator& Context::currentRoot()
{
    return m_currentRoot;
}

void Context::buildLastParsed(const CISequence& seq)
{
    m_lastParsed->clear();
    Layer::const_iterator      lanode = currentRoot();
    CISequence::const_iterator cinode = seq.begin();

    while (cinode != seq.end() && lanode != roots().end())
    {
        Node *node = *lanode;
        // Copy nodes to m_lastParsedSequence
        node->setIndex(cinode->index);
        m_lastParsed->push_back(node);

        // Update candidate node data according to last parsed
        if (m_candidateNode->level() <= node->level())
            m_candidateNode->setLevel(node->level() + 1);
        if (m_candidateNode->begin() > node->begin() || m_candidateNode->begin() == -1)
            m_candidateNode->setBegin(node->begin());
        if (m_candidateNode->end() < node->end() || m_candidateNode->end() == -1)
            m_candidateNode->setEnd(node->end());

        ++lanode; ++cinode;
    }
}

void Context::advanceCurrentRoot(int step)
{
    m_currentRoot += step;
    m_currentRootPos += step;
}

void Context::setParseTree(Trees::Tree *tree)
{
    m_parseTree = tree;
}

void Context::setOutputTree(Trees::Tree *tree)
{
    m_outputTree = tree;
    m_roots.assign(tree->roots().begin(), tree->roots().end());
    m_currentRoot = m_roots.begin();
    m_currentRootPos = 0;
}

const FL::TimeSeries* Context::timeSeries()
{
    return m_ts;
}

void Context::setTimeSeries(const FL::TimeSeries* ts)
{
    m_ts = ts;
}

FL::Trees::Layer& Context::modification()
{
    return *m_modification;
}
