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
    virtual ~Forest() { cleanup(); }
    void cleanup() { FL::cleanup(*this); }
};

}} // namespaces

#endif // FOREST_H
