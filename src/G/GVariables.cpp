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

#include "GException.h"
#include "GLogger.h"

GLogger logg("./lastrun.log");
GNotifyError GErrorClass::onError;
