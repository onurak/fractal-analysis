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

#ifndef FILETPL_H
#define FILETPL_H

#include "../Pattern.h"

namespace FL {

/*! \addtogroup FLFiles
  */
/*@{*/

/*! \class FilePAT
  * \brief Loading from PAT-files - files containig patterns of Fractal Library
  *
  * File format:
  * \code
  * file             = patterns_section [synonyms_section]
  * patterns_section = "PATTERNS" ":" pattern_string+
  * pattern          = description @ guard ";" [comment]
  * description      = <depend on used FL::Patterns::Description type>
  * guard            = <depend on used FL::Patterns::Guard type>
  * synonyms_section = "SYNONYMS" ":" synonym+
  * synonym          = pattern_name "=" pattern_name ";" [comment]
  * comment          = "#" (anything - eol)* eol
  * \endcode
  *
  */

class FilePAT
{
public:
    //! Load patterns from file
    /*! \param fileName File name to load
      * \param patterns Patterns collection whereto patterns from file will be added
      * \throws EParse
      */
    bool loadFromFile(const std::string &fileName, PatternCollection &patterns);
    
    //! Load patterns from string
    /*! \param text Text to load
      * \param patterns Patterns collection whereto patterns from file will be added
      * \throws EParse
      */
    bool loadFromString(const std::string &text, PatternCollection &patterns);

    const std::string & lastErrorDescription() const { return m_lastErrorDescription; }
private:
    std::string m_lastErrorDescription;
};

/*@}*/

} // namespace

#endif // FILETPL_H
