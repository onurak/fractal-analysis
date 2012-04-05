#include "Context.h"
#include "../trees/Tree.h"
#include "../TimeSeries.h"
#include <assert.h>
#include <algorithm>

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
    m_cache = new Layer();
    m_lastParsed = new Layer();
    m_modification = new Layer();

    setTimeSeries(c.m_ts);
    setParseTree(c.parseTree().copy());
    copyRoots(c.outputTree().copy(), c);
    setCandidateNode(new Node());
}

Context::~Context()
{
    delete m_cache;
    delete m_lastParsed;
    delete m_modification;
}

const Layer& Context::getNodes(int nameId, int index) const
{
    m_cache->clear();

    Trees::Layer::Iterator node;

    if (nameId == FL::IDGenerator::WILDCARD && index != FL::IDGenerator::WILDCARD)
    {
        forall(node, *m_lastParsed)
        {
            if ((*node)->index() == index)
                m_cache->push_back(*node);
        }
    }
    else if (nameId != FL::IDGenerator::WILDCARD && index == FL::IDGenerator::WILDCARD)
    {
        forall(node, *m_lastParsed)
        {
            if ((*node)->id() == nameId)
                m_cache->push_back(*node);
        }
    }
    else
    {
        m_cache->assign(m_lastParsed->begin(), m_lastParsed->end());
    }

    return *m_cache;
}

Node* Context::getNode(int nameId, int index) const
{
    Trees::Layer::Iterator itNode;
    forall(itNode, *m_lastParsed)
    {
        Node *node = *itNode;
        if (node->id() == nameId && node->index() == index)
            return node;
    }
    return NULL;
}

const std::list<Node*>::const_iterator& Context::currentRoot()
{
    return m_currentRoot;
}

bool Context::isAtTime(int time) const
{
    return (*m_currentRoot)->begin() >= time;
}

void Context::buildLastParsed(const CISequence& seq, int nodesCount, const Trees::Layer *baseLayer)
{
    if (nodesCount <= 0 || nodesCount > (int) seq.size())
        nodesCount = seq.size();

    Layer::const_iterator      lanode;
    CISequence::const_iterator cinode = seq.begin();

    // We will use layer if std::list type because roots()
    // is a std::list while baseLayer is Trees::Layer
    const std::list<Trees::Node*> *layer;

    if (baseLayer == NULL)
    {
        layer = &roots();
        lanode = currentRoot();
    }
    else
    {
        layer = baseLayer;
        lanode = baseLayer->begin();
    }

    m_lastParsed->clear();


    for (int i = 0;
         i < nodesCount && lanode != layer->end();
         ++lanode, ++cinode, ++i)
    {
        Node *node = *lanode;
        // Copy nodes to m_lastParsedSequence
        node->setIndex(cinode->index);
        m_lastParsed->push_back(node);

        // Update candidate node data according to last parsed
        if (i > 0)
        {
            if (m_candidateNode->level() <= node->level())
                m_candidateNode->setLevel(node->level() + 1);
            if (m_candidateNode->begin() > node->begin())
                m_candidateNode->setBegin(node->begin());
            if (m_candidateNode->end() < node->end())
                m_candidateNode->setEnd(node->end());
        }
        else
        {
            m_candidateNode->setLevel(node->level() + 1);
            m_candidateNode->setBegin(node->begin());
            m_candidateNode->setEnd(node->end());
        }
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
    if (tree)
        setRoots(tree->roots());
}


struct comp {
    bool operator() (FL::Trees::Node* n1, FL::Trees::Node* n2) {
        return n1->begin() < n2->begin();
    }
} comp_o;

void Context::setRoots(const std::list<Trees::Node*> &layer)
{
    std::vector<Trees::Node*> tmpRoots(layer.begin(),  layer.end());
    std::sort(tmpRoots.begin(), tmpRoots.end(), comp_o);

    m_roots.assign(tmpRoots.begin(), tmpRoots.end());
    m_currentRoot = m_roots.begin();
    m_currentRootPos = 0;
}

void Context::advanceCurrentRootToPos(int pos)
{
    m_currentRoot = m_roots.begin();
    m_currentRootPos = 0;

    while (!isAtEnd() && (*m_currentRoot)->begin() < pos)
        advanceCurrentRoot(1);
}

void Context::copyRoots(Trees::Tree *newOutputTree, const Context& c)
{
    m_outputTree = newOutputTree;
    Layer::ConstIterator node;
    forall(node, c.roots())
    {
         Node *root = m_outputTree->findNode(*node);
         assert(root != NULL);
         m_roots.push_back(root);
    }
    m_currentRoot = m_roots.begin();
    m_currentRootPos = 0;
    advanceCurrentRoot(c.currentRootPos());
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
