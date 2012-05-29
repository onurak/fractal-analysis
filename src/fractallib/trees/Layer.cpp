#include "Layer.h"

using namespace FL::Trees;

Layer::Layer()
{
}

Node* Layer::getPrevNearestTo(int position) const
{
    if (position == 0 || size() == 0 || front()->end() > position)
        return NULL;

    const_iterator node = begin(), nextNode = begin();
    for (++nextNode; nextNode != end(); ++node, ++nextNode)
    {
        if ((*node)->end() <= position && (*nextNode)->end() > position)
            return *node;
    }
    return back();
}

Node* Layer::getLastNode() const
{
    if (size() == 0) return NULL;

    Node *node = front();

    Layer::ConstIterator i;
    forall(i, *this)
    {
        if ((*i)->end() > node->end())
            node = *i;
    }

    return node;
}

Node* Layer::firstNode() const
{
    if (size() == 0) return NULL;

    Node *node = front();
    if (node->begin() == 0)
        return node;

    Layer::ConstIterator i;
    forall(i, *this)
    {
        if ((*i)->begin() == 0)
            return *i;

        if ((*i)->end() > node->end())
            node = *i;
    }

    return node;
}
