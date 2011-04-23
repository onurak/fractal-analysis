#ifndef LEVELSCOUNT_H
#define LEVELSCOUNT_H

#include "../Metrics.h"

namespace FL { namespace Trees { namespace Metrics {

class LevelsCount
{
public:
    virtual double apply(const Tree &tree)
    {
        return tree.levelCount();
    }
};


}}} // namespaces

#endif // LEVELSCOUNT_H
