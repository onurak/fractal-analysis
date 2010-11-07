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
#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

/*! \file ProjectSettings.h
  * \brief This file contains library-wide settings (like macro definitions etc.)
    */

/*! \macro FL_ENABLE_PARSER_DEBUG
  * If this macro defined than FL::TSParser will generate debug events if it would
  * be ran in WorkDebug mode. Otherwise there will be no difference
  * between WorkDebug and WorkNormal.
  */
#define FL_ENABLE_PARSER_DEBUG

/*! \macro FL_ENABLE_PARSE_TREE_OPTIMIZATION
  * If this macro defined than FL::ParseTree will be based on some optimized storage argorithm
  * (like FL::HyperTree). This means that it will work faster and take less of memory.
  * Otherwise simple copy algorithm will be used. It could be used in case of errors of hyper tree.
  */
//#define FL_ENABLE_PARSE_TREE_OPTIMIZATION



#endif // PROJECTSETTINGS_H
