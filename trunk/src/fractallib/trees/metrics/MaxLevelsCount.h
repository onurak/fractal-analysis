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

#ifndef MAXLEVELSCOUNT_H
#define MAXLEVELSCOUNT_H

#include "../Metrics.h"

namespace FL { namespace Trees { namespace Metrics {

class MaxLevelsCount : public Metric
{
public:
    MaxLevelsCount()
    {
        setLimit(5);
        m_name = "Max levels count";
    }

    virtual bool filter(Tree &tree, Forest &forest)
    {
        return tree.levelCount() <= (int) m_limit;
    }

    virtual void setLimit(const GVariant &value)
    {
        if (value.canCastTo(G_VAR_INT))
        {
            m_limit = (int) value;
            if ((int) m_limit <= 0)
                m_limit = 1;
        }
    }
};


}}} // namespaces

#endif // MAXLEVELSCOUNT_H
