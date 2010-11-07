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

#ifndef FLQT_H
#define FLQT_H

/*! \addtogroup FLUtils
  */
/*@{*/


/*! \file
  * \brief Helpers for FL-Qt integration
  */

#include <QString>
#include <string>
#include <vector>
#include "G/GVariant.h"

//! Convert QString to std::string
inline std::string STR(QString s) { return s.toStdString(); }

//! Convert std::string to QString
inline QString QSTR(std::string s) { return QString().fromStdString(s); }

/*@}*/

#endif // FLQT_H
