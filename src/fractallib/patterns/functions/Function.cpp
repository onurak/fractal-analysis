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

#include "Function.h"

using namespace FL::Patterns::Functions;

char ErrorDescr[][50] =
{
    "No error",
    "Invalid predicate arguments",
    "Invalid node passed"
};

const char* FL::Patterns::Functions::errorDescription(Error err)
{
    return err >= 0 && err < 3 ? ErrorDescr[(int)err] : NULL;
}
