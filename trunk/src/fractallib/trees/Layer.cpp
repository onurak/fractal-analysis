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

bool nodeLessLevelCompare(const Node* node1, const Node *node2)
{
    return node1->level() < node2->level();
}

void Layer::sortByTime()
{
    // Sort needs random access iterator, while Layer is the std::list
    std::vector<Node*> vec(begin(), end());
    std::sort(vec.begin(), vec.end(), nodeLessTimeCompare);
    assign(vec.begin(), vec.end());
}

void Layer::sortByLevel()
{
    // Sort needs random access iterator, while Layer is the std::list
    std::vector<Node*> vec(begin(), end());
    std::sort(vec.begin(), vec.end(), nodeLessLevelCompare);
    assign(vec.begin(), vec.end());
}
