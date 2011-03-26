#include "Description.h"

using namespace FL::Patterns;
using namespace FL::Exceptions;

Description::Description()
{
    m_id = IDGenerator::WILDCARD;
}

const std::string& Description::name() const
{
    return m_name;
}

void Description::setName(const std::string& newName)
{
    m_name = newName;
    m_id = IDGenerator::idOf(m_name);
}

int Description::id() const
{
    return m_id;
}

EParsing Description::compile(const std::string &s)
{
    std::istringstream stream(s);
    Compilers::Input::Iterator it(stream);
    Compilers::Input input(it);
    return compile(input);
}
