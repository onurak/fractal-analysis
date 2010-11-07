/* Author: Vladimir Belikov
 *
 * This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Fractal Libray.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PARSETREEMETRIC_H
#define PARSETREEMETRIC_H

#include "ParseTree.h"
#include "Exceptions.h"

namespace FL {

    /*! \namespace Metrics
      * \brief Includes different parse tree metrics
      */
    namespace Metrics {



/*! \class ParseTreeMetric
  * \brief ParseTree metric class
  */
class ParseTreeMetric
{
public:
    //! Default constructor
    /*! Children should assign values to m_min and m_max in constructor.
      */
    ParseTreeMetric()
    {
        m_min = -INF;
        m_max = +INF;
    }

    //! Destructor
    /*! Do nothing. Just to be polymorphic
      */
    virtual ~ParseTreeMetric() {}

    //! Main metric function
    /*! Concrete metric function must reintroduce this method.
      */
    virtual double measure(FL::ParseTree &tree) = 0;


    //! Overloaded metric function
    /*! Equivalent to measure(*tree). If tree is empty EMetric will be thrown.
      * \throws EMetric
      */
    double measure(FL::ParseTree *tree)
    {
        if (tree == NULL)
        {
            GError(GCritical, "ParseTreeMetric", 0, "Parse tree not set");
            return 0;
        }
        return measure(*tree);
    }

    //! Function operator
    /*! Equivalent to measure(tree)
      */
    double operator()(FL::ParseTree &tree)
    {
        return measure(tree);
    }

    //! Function operator
    /*! Equivalent to measure(tree)
      */
    double operator()(FL::ParseTree *tree)
    {
        return measure(tree);
    }

    //! Get maximum possible metric value
    /*! If metric unlimited than +INF will be return.
      */
    double max()
    {
        return m_max;
    }

    //! Get minimum possible metric value
    /*! If metric unlimited than -INF will be return.
      */
    double min()
    {
        return m_max;
    }
protected:
    //! Maximal metric value
    double m_max;

    //! Minimal metric value
    double m_min;
};

}} // namespaces

#endif // PARSETREEMETRIC_H
