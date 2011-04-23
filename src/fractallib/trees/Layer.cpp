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

bool nodeLessTimeCompare(const Node* node1, const Node *node2)
{
    return node1->begin() < node2->begin();
}

void Layer::sort()
{
    // Sort needs random access iterator, while Layer is the std::list
    std::vector<Node*> vec(begin(), end());
    std::sort(vec.begin(), vec.end(), nodeLessTimeCompare);
    assign(vec.begin(), vec.end());
}
