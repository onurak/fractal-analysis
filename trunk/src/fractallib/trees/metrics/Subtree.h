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

#ifndef SUBTREE_H
#define SUBTREE_H

#include "../Metrics.h"

namespace FL { namespace Trees { namespace Metrics {

/*! \class Subtree
  * \brief Subtree metric checks if specified tree is subtree of another tree
  *
  * It return 1 if it's tree or 0 otherwise.
  * So if limit is 0 then subtrees not acceptable for metrics and 1 otherwise
  */
class Subtree : public Metric
{
public:
    Subtree(double limit = 0) : Metric(limit)
    {
        m_name = "Subtree";
    }

    virtual double apply(const Tree &tree, const Forest &forest)
    {
        Forest::ConstIterator itTree;

        for (itTree = forest.begin(); itTree != forest.end(); ++itTree)
        {
            if (*itTree == &tree)
                continue;
            TreeCompareResult tcr = (*itTree)->compare(tree);
            if (tcr.isSecondSubtreeOfFirst())
                return 1;
        }

        return 0;
    }

    virtual double setLimit(const double &value)
    {
        return m_limit = value == 0 ? 0 : 1;
    }
};


}}} // namespaces


#endif // SUBTREE_H
