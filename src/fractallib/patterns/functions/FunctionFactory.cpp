#include "FunctionFactory.h"
#include "Min.h"
#include "Max.h"

using namespace FL::Patterns::Functions;

std::map<std::string, Function*> FunctionFactory::m_functions;

std::string upperCase(const std::string &s)
{
    std::string result;
    for (int i = 0; i < int(s.length()); ++i)
    {
        char c = s[i];
        if (isalpha(c) && islower(c)) c = toupper(c);
        result += c;
    }
    return result;
}


Function* FunctionFactory::get(const std::string &name)
{
    std::map<std::string, Function*>::iterator it;
    it = m_functions.find(upperCase(name));
    if (it == m_functions.end())
        return NULL;
    else
        return (*it).second;
}

void FunctionFactory::registerFunction(Function *f)
{
    if (f != NULL)
    {
        std::string name = upperCase(f->name());
        std::map<std::string, Function*>::iterator it;
        it = m_functions.find(name);
        if (it == m_functions.end())
            m_functions[name] = f;
    }
}

void FunctionFactory::registerAll()
{
    if (get("Max") != NULL) return;
    registerFunction(new Max());
    registerFunction(new Min());
}