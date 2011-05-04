/** This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Fractal Library. If not, see <http://www.gnu.org/licenses/>.
 */

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
    void sortByTime();

    //! Sort nodes in layer by level
    void sortByLevel();
};

}} // namespace

inline void operator+= (FL::Trees::Layer::ConstIterator &i, int size)
{
    while (size-- > 0)
        ++i;
}

#endif // LAYER_H
