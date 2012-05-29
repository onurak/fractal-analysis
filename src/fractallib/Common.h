#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

namespace FL {

//! Shortcut for iterating over whole collection
#define forall(iterator, collection) \
    for (iterator = (collection).begin(); iterator != (collection).end(); ++iterator)

//! Shortcut for checking if collection's end i reached by iterator
#define isend(iterator, collection) \
    iterator = collection.end()

//! Iterate over two collections simultaneously with two iterators
#define forboth(iterator1, collection1, iterator2, collection2) \
    for (iterator1 = (collection1).begin(), iterator2 = (collection2).begin(); \
         iterator1 != (collection1).end() && iterator2 != (collection2.end()); \
         ++iterator1, ++iterator2)

//! Shortcut for range iterating
#define forrange(iterator, start, end) \
    for (int iterator = start; iterator != end; start <= end ? ++iterator : --iterator)


//! Check if specified item exists in collection and return iterator to this item
template <typename Collection, typename Value>
inline typename Collection::iterator
search(Collection &c, const Value &v)
{
    return std::find(c.begin(), c.end(), v);
}


//! Check if specified item exists in collection and return iterator to this item
template <typename Collection, typename Value>
inline bool
search(Collection &c, const Value &v, typename Collection::iterator &i)
{
    i = std::find(c.begin(), c.end(), v);
    return i != c.end();
}

//! Check if specified item exists in collection
template <typename Collection, typename Value>
inline bool
exists(const Collection &c, const Value &v)
{
    typename Collection::const_iterator i =
            std::find(c.begin(), c.end(), v);
    return i != c.end();
}

//! Call destructor for each item of collection and clear itself
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

/*! \class ParseResult
  * \brief Result of parsing pass
  */
class ParseResult
{
public:
    ParseResult()
    {
        reset();
    }

    inline void reset()
    {
        treesAdded = nodesAdded = 0;
    }

    int treesAdded;
    int nodesAdded;

    bool somethingAdded() const
    {
        return treesAdded > 0 || nodesAdded > 0;
    }

    void add(const ParseResult &pr)
    {
        treesAdded    += pr.treesAdded;
        nodesAdded    += pr.nodesAdded;
    }
};


/*! \class IDGenerator
  * \brief Generator of library-wide unique IDs for strings
  */
class IDGenerator
{
public:
    static const int WILDCARD = -1;
    static const int NONE     = 0;
public:
    static int idOf(const std::string &name);
    static const std::string& nameOf(int id);
    static void makeSynonyms(const std::string &name1, const std::string &name2);
    static bool isSynonyms(int id1, int id2);
    static void clearSynonyms();
    static std::vector<int> getSynonyms(int id);
private:
    static std::map<std::string, int> m_map;
    static std::map<int, std::string> m_invMap;
    static std::map< int, std::vector<int> > m_synonyms;
    static int m_id;
};

inline std::string intToString(int v)
{
    char buf[100];
    //return itoa(v, buf, 10);
    sprintf(buf, "%d", v);
    return std::string(buf);
}

//std::string upperCase(const std::string &s);


} // namespaces

//namespace FL { namespace Patterns {

//}} // namespaces

#endif // COMMON_H
