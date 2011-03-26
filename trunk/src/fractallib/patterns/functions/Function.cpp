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
