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
#ifndef SPIRIT_H
#define SPIRIT_H

#include <iostream>
#include <string>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

/*! \fn spiritParseString(const std::string&, Grammar &)
  * \brief Parse string with provided grammar
  *
  * \return -1 if success, error position otherwise
  */

template <class Grammar>
inline int spiritParseString(const std::string &text, Grammar &g)
{
    std::string::const_iterator iter = text.begin();
    std::string::const_iterator end = text.end();

    bool r = phrase_parse(iter, end, g, ascii::space);

    if (r && iter == end)
        return -1;
    else
        return end - iter;
}

// Error codes
const int EParserNoError = -1;
const int EParserCantOpenFile = -2;


/*! \fn spiritParseFile(const std::string&, Grammar&)
  * \brief Parse file with provided grammar
  *
  * \return EParserNoError if success, otherwise if result > 0 than it's error position
  *  if less than zero and not -1 than it's error code
  */
template <class Grammar>
inline int spiritParseFile(const std::string &fileName, Grammar &g)
{
    if (fileName.empty())
        return EParserCantOpenFile;

    std::ifstream in(fileName.c_str(), std::ios_base::in);

    if (!in.is_open())
        return EParserCantOpenFile;

    std::string text;
    in.unsetf(std::ios::skipws); // не пропускаем пробелы
    copy(
            std::istream_iterator<char>(in),
            std::istream_iterator<char>(),
            std::back_inserter(text)
         );

    std::string::const_iterator iter = text.begin();
    std::string::const_iterator end = text.end();

    bool r = phrase_parse(iter, end, g, ascii::space);

    in.close();

    if (r && iter == end)
        return EParserNoError;
    else
        return end - iter;
}

#endif // SPIRIT_H
