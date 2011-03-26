/* Author: Vladimir Belikov
 *
 * This file is part of G Library.
 *
 * G Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * G Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with G Libray.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GDELEGATE_H
#define GDELEGATE_H

// 0 param
#define SUFFIX   0
#define TEMPLATE_PARAMS
#define TEMPLATE_ARGS
#define PARAMS
#define ARGS
#include "GDelegateImplementation.h"
#undef SUFFIX
#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef PARAMS
#undef ARGS



// 1 param
#define SUFFIX   1
#define TEMPLATE_PARAMS \
    , class TP1
#define TEMPLATE_ARGS \
    , TP1
#define PARAMS \
    TP1 p1
#define ARGS \
    p1
#include "GDelegateImplementation.h"
#undef SUFFIX
#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef PARAMS
#undef ARGS


// 2 param
#define SUFFIX   2
#define TEMPLATE_PARAMS \
    , class TP1, class TP2
#define TEMPLATE_ARGS \
    , TP1, TP2
#define PARAMS \
    TP1 p1, TP2 p2
#define ARGS \
    p1, p2
#include "GDelegateImplementation.h"
#undef SUFFIX
#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef PARAMS
#undef ARGS

// 3 param
#define SUFFIX   3
#define TEMPLATE_PARAMS \
    , class TP1, class TP2, class TP3
#define TEMPLATE_ARGS \
    , TP1, TP2, TP3
#define PARAMS \
    TP1 p1, TP2 p2, TP3 p3
#define ARGS \
    p1, p2, p3
#include "GDelegateImplementation.h"
#undef SUFFIX
#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef PARAMS
#undef ARGS


// 4 param
#define SUFFIX   4
#define TEMPLATE_PARAMS \
    , class TP1, class TP2, class TP3, class TP4
#define TEMPLATE_ARGS \
    , TP1, TP2, TP3, TP4
#define PARAMS \
    TP1 p1, TP2 p2, TP3 p3, TP4 p4
#define ARGS \
    p1, p2, p3, p4
#include "GDelegateImplementation.h"
#undef SUFFIX
#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef PARAMS
#undef ARGS

#endif // GDELEGATE_H
