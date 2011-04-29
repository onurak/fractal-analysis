#include "Node.h"
#include "Layer.h"

using namespace FL::Trees;

Node::Node(Node *parent, int id, int begin, int end, int level, NodeStatus ns)
    : m_id(id), m_index(-1), m_begin(begin), m_end(end), m_level(level),
      m_parent(parent), m_status(ns), m_relativeNode(NULL),
      m_internalParent(NULL), m_origSequence(NULL), m_origPattern(NULL)
{
    m_children = new Layer();
}

Node::Node(const Node &node)
{
    m_children = new Layer();
    m_id = node.id();
    m_index = node.index();
    m_level = node.level();
    m_begin = node.begin();
    m_end = node.end();
    m_parent = NULL;
    m_relativeNode = NULL;
    m_internalParent = NULL;
    m_origSequence = NULL;
    m_origPattern = NULL;
    m_status = FL::nsFixed;
}

Node::~Node()
{
    setParent(NULL);
    while (m_children->size() > 0)
        m_children->front()->setParent(NULL);
    delete m_children;
}

void Node::setParent(Node* value)
{
    if (m_parent)
        m_parent->children().remove(this);

    m_parent = value;
    if (m_parent)
    {
        // Reset internal parent
        if (m_internalParent)
        {
            m_internalParent->children().remove(this);
            m_internalParent = NULL;
        }

        m_parent->children().push_back(this);

        // Adjust position of parent (level and time series indices)
        if (m_parent->level() <= m_level)
            m_parent->setLevel(m_level + 1);
        if (m_parent->begin() > m_begin || m_parent->begin() == -1)
            m_parent->setBegin(m_begin);
        if (m_parent->end() < m_end)
            m_parent->setEnd(m_end);
    }
}
