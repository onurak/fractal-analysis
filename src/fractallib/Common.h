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

    void operator += (const ParseResult &r)
    {
        treesAdded += r.treesAdded;
        treesModified += r.treesModified;
        nodesAdded += r.nodesAdded;
    }

    bool somethingAdded() const
    {
        return treesAdded > 0 || treesModified > 0 || nodesAdded > 0;
    }
};

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
    return itoa(v, buf, 10);
}

//std::string upperCase(const std::string &s);


} // namespaces

//namespace FL { namespace Patterns {

//}} // namespaces

#endif // COMMON_H
