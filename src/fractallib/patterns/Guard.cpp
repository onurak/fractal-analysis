#include "Guard.h"

using namespace FL::Patterns;
using namespace FL::Exceptions;

Guard::Guard(const FL::Patterns::Description &description)
    : m_description(description)
{
}

EParsing Guard::compile(const std::string &s)
{
    std::istringstream stream(s);
    Compilers::Input::Iterator it(stream);
    Compilers::Input input(it);
    return compile(input);
}
