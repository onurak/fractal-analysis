#ifndef FOREST_H
#define FOREST_H

#include "Tree.h"
#include <vector>

namespace FL { namespace Trees {

/*! \class Forest
  * \brief Colelction of trees
  */
class Forest : public std::vector<Tree*>
{
public:
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
public:
    Forest();
    virtual ~Forest() { cleanup(); }
    void cleanup() { FL::cleanup(*this); }

    int maxLevelCount() const
    {
        int result = 0;
        ConstIterator tree;
        forall(tree, *this)
        {
            int level = (*tree)->levelCount();
            if (level > result)
                result = level;
        }
        return result;
    }
};

}} // namespaces

#endif // FOREST_H
