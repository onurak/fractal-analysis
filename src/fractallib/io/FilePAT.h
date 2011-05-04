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

#ifndef FILEPAT_H
#define FILEPAT_H

#include "../patterns/Pattern.h"

namespace FL { namespace IO {

/*! @class FilePAT
  * @brief File with patterns data
  *
  * @section File structure
  * Patterns file consists of few sections, there is common structure:
  * @verbatim
  * PATTERNS:
  *     pattern1 = description1 @ guard1;
  *     pattern2 = description2 @ guard2;
  *     ....
  *     patternN = descriptionN @ guardN;
  * @endverbatim
  */
class FilePAT
{
public:
    //! Open file with specified name and load all patterns to @a patterns
    /*! @param fileName Name of file to open
      * @param patterns Output patterns set
      * @return true if file loaded normally, false if there was an errors
      */
    bool open(const std::string &fileName, FL::Patterns::PatternsSet &patterns);

    //! Get last error info
    const Exceptions::EParsing& lastError() const { return m_lastError; }
private:
    //! Last error info
    Exceptions::EParsing m_lastError;
};

}} // namespaces

#endif // FILEPAT_H
