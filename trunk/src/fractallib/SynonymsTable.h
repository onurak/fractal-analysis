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

#ifndef SYNONYMSTABLE_H
#define SYNONYMSTABLE_H

#include <map>
#include <string>
#include "UniqueNamer.h"

namespace FL {

/*! \class SynonymsTable
  * \brief Table of synonym names
  *
  * Searching takes same time as searching in std::multimap.
  */
class SynonymsTable
{
public:
    //! Clear the table
    inline void clear()
    {
        m_synonyms.clear();
    }

    //! Add pair (name1 == name2)
    inline void add(const std::string &name1, const std::string &name2)
    {
        add(UniqueNamer::id_of_name(name1), UniqueNamer::id_of_name(name2));
    }

    //! Add pair (name1 == name2)
    inline void add(int name1, int name2)
    {
        // make it double for faster search
        m_synonyms.insert(std::make_pair(name1, name2));
        m_synonyms.insert(std::make_pair(name2, name1));
    }

    //! Check if names are synonyms
    inline bool isSynonyms(const std::string &name1, const std::string &name2) const
    {
        return isSynonyms(UniqueNamer::id_of_name(name1), UniqueNamer::id_of_name(name2));
    }

    //! Check if name's id are synonyms
    inline bool isSynonyms(int name1, int name2) const
    {
        if (name1 == name2)
            return true;
        std::pair<Container::const_iterator, Container::const_iterator> i;

        i = m_synonyms.equal_range(name1);

        while (i.first != i.second)
        {
            if ((*(i.first)).second == name2)
                return true;
            ++(i.first);
        }
        return false;
    }
private:
    typedef std::multimap<int, int> Container;
    //! Real tables of synonyms
    Container m_synonyms;
};

} // namespace

#endif // SYNONYMSTABLE_H
