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
    int id1 = idOf(name1);
    int id2 = idOf(name2);
    std::map< int, std::vector<int> >::iterator i;
    i = m_synonyms.find(id1);
    if (i != m_synonyms.end())
        i->second.push_back(id2);
    else
    {
        std::vector<int> v;
        v.push_back(id2);
        m_synonyms[id1] = v;
    }

    i = m_synonyms.find(id2);
    if (i != m_synonyms.end())
        i->second.push_back(id1);
    else
    {
        std::vector<int> v;
        v.push_back(id1);
        m_synonyms[id2] = v;
    }
}

void IDGenerator::clearSynonyms()
{
    m_synonyms.clear();
}

bool IDGenerator::isSynonyms(int id1, int id2)
{
    std::map< int, std::vector<int> >::iterator i;
    i = m_synonyms.find(id1);
    if (i == m_synonyms.end())
        return false;
    return std::find(i->second.begin(), i->second.end(), id2) != i->second.end();
}

std::vector<int> IDGenerator::getSynonyms(int id)
{
    std::vector<int> synonyms;
    synonyms.push_back(id);

    std::map< int, std::vector<int> >::iterator i;
    i = m_synonyms.find(id);
    if (i == m_synonyms.end())
        return synonyms;
    synonyms.insert(synonyms.end(), i->second.begin(), i->second.end());
    return synonyms;
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
