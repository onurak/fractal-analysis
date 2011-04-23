#ifndef LAYER_H
#define LAYER_H

#include <list>
#include <algorithm>
#include "Node.h"

namespace FL { namespace Trees {

/*! \class Layer
  * \brief Ordered by position in time series sequence of nodes
  */
class Layer : public std::list<Node*>
{
public:
    typedef Layer::iterator Iterator;
    typedef Layer::const_iterator ConstIterator;
public:
    Layer();

    //! Get node nearest previous to specified position in time series
    /*! End position of returned node less than specified position.
      * It used by Context class.
      */
    Node* getPrevNearestTo(int position) const;

    //! Check that layer is valid
    /*! Debug mostly
      */
    void checkValidness()
    {

    }

    //! Sort nodes in layer by time
    void sort();
};

}} // namespace

inline void operator+= (FL::Trees::Layer::ConstIterator &i, int size)
{
    while (size-- > 0)
        ++i;
}

#endif // LAYER_H
