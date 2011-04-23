#include "Function.h"

using namespace FL::Patterns::Functions;

char ErrorDescr[][50] =
{
    "No error",
    "Invalid predicate arguments",
    "Invalid node passed"
};

const char* FL::Patterns::Functions::errorDescription(Error err)
{
    return err >= 0 && err < 3 ? ErrorDescr[(int)err] : NULL;
}

void Function::defineArg(const std::string &argName, bool defaultValue)
{
    m_argsData[argName]    = defaultValue;
    m_options[argName]     = defaultValue;
}

void Function::parseArgs(FunctionArgs& args)
{
    std::map<std::string, bool>::iterator i;

    // Init with default values
    forall(i, m_argsData)
        m_options[i->first] = i->second;

    FunctionArgs::const_iterator arg;
    forall(arg, args)
    {
        GVariant &v = **arg;
        if (v.type() != G_VAR_PCHAR)
            continue;

        bool isEnabled = true;
        const std::string argName = v;

        i = m_argsData.find(argName);
        if (i == m_argsData.end())
        {
            std::string noArgName = "no";
            noArgName += argName;
            i = m_argsData.find(noArgName);
            if (i == m_argsData.end())
                throw FL::Exceptions::EArguments(m_name, argName, arg - args.begin());
            isEnabled = false;
        }

        m_options[argName] = isEnabled;
    }
}
