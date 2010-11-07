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

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <string>
#include <algorithm>
#include "G/GLogger.h"
#include "G/GException.h"


namespace FL {

/*! \addtogroup FLExceptions
  */
/*@{*/

inline std::string atPos(int stringNo, int stringPos = -1);

#define for_each_(iter, collection) \
    for (iter = (collection).begin(); iter != (collection).end(); ++iter)

#define find_(value, collection) \
   (std::find((collection).begin(), (collection).end(), value) != (collection).end())

//! Resource string
const std::string ECantOpenFile = "Can't open file: ";
//! Resource string
const std::string EInvalidFileFormat = "Invalide file format: ";
//! Resource string
const std::string EInvalidIndex = "Invalide index";
//! Resource string
const std::string EUnknowFileFormat = "Unknown file format";
//! Resource string
const std::string EFileNotLoaded = "File not loaded";
//! Resource string
const std::string ETermNotExists = "Term not exists";
//! Resource string
const std::string EGuardAndTemplateIncompatible = "Guard and template are incompatible";
//! Resource string
const std::string EInvalidProgram = "Invalid program";
//! Resource string
const std::string EInvalidTermArgs = "Invalid term arguments";
//! Resource string
const std::string ETreeModification = "Parse tree modification error";

//! Some problem occures in Linux connected with itoa; this is simple replacement
inline void i2a(int v, char *result)
{
    if (v == 0)
    {
        result[0] = '0';
        result[1] = '\0';
        return;
    }

    int len = -1;

    if (v < 0)
    {
        result[0] = '-';
        v = -v;
        ++len;
    }

    for (int t = v; t != 0; t = t/10)
        ++len;
    int i;
    for (i = 0; v != 0; i++)
    {
        result[len - i] = char(v % 10 + '0');
        v = v / 10;
    }
    result[i] = '\0';
}

/*! \fn atPos(int, int)
  * \brief Helper function, generate string like " at pos (stringNo, stringPos)",
  *   or " at string stringNo" if stringPos is -1
  */
inline std::string atPos(int stringNo, int stringPos)
{
    char buf1[65], buf2[65];
    i2a(stringNo, buf1);
    if (stringPos != -1)
    {
        i2a(stringPos, buf2);
        return " at pos (" + std::string(buf1) + ", " + std::string(buf2) + ")";
    } else
        return " at pos " + std::string(buf1);
}

struct FilePos
{
    FilePos(int line = -1, int column = -1) : line(line), column(column) {}
    int line, column;
};

/*! \fn posInFile
  * \brief Calculate two-dimensional position in file (line and column)
  *   from single-dimensional (character no)
  */
inline FilePos posInFile(std::string fileName, int pos)
{
    FilePos result(-1, -1);
    std::ifstream file(fileName.c_str());
    if (!file)
        return result;
    std::string buf;
    while (true)
    {
        // Get next line and its length
        std::getline(file, buf);

        // If it's last line to get - break
        if (int(buf.length()) < pos)
        {
            result.line++;
            pos -= int(buf.length());
        }
        else
            break;
    }
    file.close();
    result.column = pos;
    return result;
}

/*@}*/

} // namespace

#endif // EXCEPTIONS_H
