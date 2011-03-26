#ifndef FOREST_H
#define FOREST_H

#include "Tree.h"
#include <vector>

namespace FL { namespace Trees {

class Forest : public std::vector<Tree*>
{
public:
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
public:
    Forest();
};

}} // namespaces

#endif // FOREST_H
