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

#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <strings.h>
#include <string.h>

namespace FL {

#define forall(iterator, collection) \
    for (iterator = (collection).begin(); iterator != (collection).end(); ++iterator)

#define isend(iterator, collection) \
    iterator = collection.end()

#define forboth(iterator1, collection1, iterator2, collection2) \
    for (iterator1 = (collection1).begin(), iterator2 = (collection2).begin(); \
         iterator1 != (collection1).end() && iterator2 != (collection2.end()); \
         ++iterator1, ++iterator2)

#define forrange(iterator, start, end) \
    for (int iterator = start; iterator != end; start <= end ? ++iterator : --iterator)

template <typename Collection, typename Value>
inline typename Collection::iterator
search(Collection &c, const Value &v)
{
    return std::find(c.begin(), c.end(), v);
}

template <typename Collection, typename Value>
inline bool
search(Collection &c, const Value &v, typename Collection::iterator &i)
{
    i = std::find(c.begin(), c.end(), v);
    return i != c.end();
}

template <typename Collection, typename Value>
inline bool
exists(Collection &c, const Value &v)
{
    typename Collection::const_iterator i =
            std::find(c.begin(), c.end(), v);
    return i != c.end();
}


template <typename Collection>
inline void
cleanup(Collection &c)
{
    if (c.size() > 0)
    {
        typename Collection::iterator i;
        forall(i, c)
            delete *i;
        c.clear();
    }
}

class IDGenerator
{
public:
    static const int WILDCARD = -1;
public:
    static int idOf(const std::string &name);
    static const std::string& nameOf(int id);
    static void makeSynonyms(const std::string &name1, const std::string &name2);
    static bool isSynonyms(int id1, int id2);
private:
    static std::map<std::string, int> m_map;
    static std::map<int, std::string> m_invMap;
    static std::map< int, std::vector<int> > m_synonyms;
    static int m_id;
};


inline std::string intToString(int v)
{
    char buf[100];
    sprintf(buf, "%d", v);
    return buf;
}

//std::string upperCase(const std::string &s);


} // namespaces

#endif // COMMON_H
