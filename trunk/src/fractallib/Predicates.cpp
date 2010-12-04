#include "Predicates.h"

using namespace FL::Predicates;

char ErrorDescr[][50] =
{
    "No error",
    "Invalid predicate arguments",
    "Invalid node passed"
};

const char* FL::Predicates::errorDescription(Error err)
{
    return err >= 0 && err < 3 ? ErrorDescr[(int)err] : NULL;
}
