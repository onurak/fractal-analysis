#ifndef LAYER_H
#define LAYER_H

#include <list>
#include "Node.h"

namespace FL { namespace Trees {

class Layer : public std::list<Node*>
{
public:
    typedef Layer::iterator Iterator;
    typedef Layer::const_iterator ConstIterator;
public:
    Layer();
};

}} // namespace

inline void operator+= (FL::Trees::Layer::ConstIterator &i, int size)
{
    while (size-- > 0)
        ++i;
}

#endif // LAYER_H
