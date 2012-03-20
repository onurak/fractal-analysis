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

#ifndef GRESOURCES_H
#define GRESOURCES_H

#include <string>
#include <strings.h>

/*! \addtogroup GLib
  */
/*@{*/

const std::string GIndexOutOfBounds = "Index out of bounds";
const std::string GVariantName = "GVariant";
const std::string GVariantNotEnoughtMemory = "Can't create variant: not enought memory";
const std::string GVariantCantCastNullToSomething = "Can't cast NULL variant to something";
const std::string GVariantCantCast = "Variant cast error:";
const std::string GVariantUndefinedOperation = "Undefined variant operation";
const std::string GLoggerInfo = "Info: ";
const std::string GLoggerWarning = "Warning: ";
const std::string GLoggerError = "Error: ";
const std::string GLoggerDebug = "Debug: ";
const std::string GDataSetFieldAlreadyExists = "Field already exists";
const std::string GDataSetFieldNotExists = "Field not exists";


const int G_UNDEFINED_OPERATION   = 10;
const int G_INVALID_TYPECAST      = 11;
const int G_INVALID_VARIANT_INIT  = 12;

const int G_INDEX_OUT_OF_BOUNDS = 100;

//! Convert string to lower case
inline std::string lowerCase(const std::string &s)
{

    std::string result = s;
    for (unsigned int i = 0; i < result.size(); i++)
        result[i] = tolower(result[i]);
    return result;
}

/*@}*/

#endif // GRESOURCES_H
