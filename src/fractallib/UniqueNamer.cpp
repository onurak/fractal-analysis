#include "UniqueNamer.h"

using namespace FL;

UniqueNamer::IDCollectionType UniqueNamer::m_map;
UniqueNamer::IDCollectionReverseType UniqueNamer::m_revMap;
int UniqueNamer::m_lastID = 0;
const std::string emptyString = "";

const std::string& UniqueNamer::name_of_id(int id)
{
    IDCollectionReverseType::const_iterator i = m_revMap.find(id);

    if (i != m_revMap.end())
        return (*i).second;
    else
        return emptyString;
}

int UniqueNamer::id_of_name(const std::string &name)
{
    if (name == emptyString)
        return INVALID_ID;
    IDCollectionType::const_iterator i = m_map.find(name);
    if (i == m_map.end())
    {
        int id = m_map[name] = ++m_lastID;
        m_revMap[id] = name;
        return id;
    }
    else
        return (*i).second;
}
