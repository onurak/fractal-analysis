#ifndef UNIQUENAMER_H
#define UNIQUENAMER_H

#include <string>
#include <map>

namespace FL {

class UniqueNamer
{
public:
    typedef std::map<std::string, int> IDCollectionType;
    typedef std::map<int, std::string> IDCollectionReverseType;
public:
    static const int INVALID_ID = -1000;
    static const int WILDCARD = -1;
public:
    static const std::string& name_of_id(int id);
    static int id_of_name(const std::string &name);
private:
    static IDCollectionType m_map;
    static IDCollectionReverseType m_revMap;
    static int m_lastID;
};

} // namespace

#endif // UNIQUENAMER_H
