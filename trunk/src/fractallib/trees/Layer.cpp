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
