#include "Common.h"

using namespace FL;

std::map<std::string, int> IDGenerator::m_map;
std::map<int, std::string> IDGenerator::m_invMap;
std::map< int, std::vector<int> > IDGenerator::m_synonyms;
int IDGenerator::m_id = 0;
std::string EMPTY_STRING = "";

int IDGenerator::idOf(const std::string &name)
{
    std::map<std::string, int>::const_iterator i;
    i = m_map.find(name);
    if (i != m_map.end())
    {
        return (*i).second;
    }
    else
    {
        m_map[name] = ++m_id;
        m_invMap[m_id] = name;
        return m_id;
    }
}

const std::string& IDGenerator::nameOf(int id)
{
    std::map<int, std::string>::const_iterator i;
    i = m_invMap.find(id);
    if (i != m_invMap.end())
        return (*i).second;
    else
        return EMPTY_STRING;
}

void IDGenerator::makeSynonyms(const std::string &name1, const std::string &name2)
{
    int id1 = idOf(name1), id2 = idOf(name2);
    m_synonyms[id1].push_back(id2);
    m_synonyms[id2].push_back(id1);
}

bool IDGenerator::isSynonyms(int id1, int id2)
{
    if (id1 == id2)
        return true;

    std::vector<int> &synonymsOfId1 = m_synonyms[id1];
    return std::find(synonymsOfId1.begin(), synonymsOfId1.end(), id2)
            != synonymsOfId1.end();
}

/*
std::string upperCase(const std::string &s)
{
    std::string result;
    for (int i = 0; i < int(s.length()); ++i)
    {
        char c = s[i];
        if (isalpha(c)) c += 32;
        result += c;
    }
    return result;
}
*/
