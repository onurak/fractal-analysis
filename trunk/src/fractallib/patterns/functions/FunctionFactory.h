#ifndef FUNCTIONFACTORY_H
#define FUNCTIONFACTORY_H

#include <map>
#include <string>
#include "Function.h"

namespace FL { namespace Patterns { namespace Functions {

class FunctionFactory
{
public:
    static Function* get(const std::string &name);
    static void registerFunction(Function *f);
    static void registerAll();
private:
    static std::map<std::string, Function*> m_functions;
};

}}} // namespaces

#endif // FUNCTIONFACTORY_H
