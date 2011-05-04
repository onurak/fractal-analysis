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
