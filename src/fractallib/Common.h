#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

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




class ParseResult
{
public:
    ParseResult()
    {
        reset();
    }

    inline void reset()
    {
        treesAdded = treesModified = nodesAdded = 0;
    }

    int treesAdded;
    int treesModified;
    int nodesAdded;
};

class IDGenerator
{
public:
    static const int WILDCARD = -1;
public:
    static int idOf(const std::string &name);
    static const std::string& nameOf(int id);
private:
    static std::map<std::string, int> m_map;
    static std::map<int, std::string> m_invMap;
    static int m_id;
};

inline std::string intToString(int v)
{
    char buf[100];
    return itoa(v, buf, 10);
}

//std::string upperCase(const std::string &s);


} // namespaces

//namespace FL { namespace Patterns {

//}} // namespaces

#endif // COMMON_H
