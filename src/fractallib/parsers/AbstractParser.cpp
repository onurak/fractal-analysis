#include "AbstractParser.h"

using namespace FL::Parsers;

const char *E_ERROR_DESCRIPTION[] =
{
  "Empty time series",
  "Empty pattern set",
  "Empty forest"
};

AbstractParser::AbstractParser()
{
}

AbstractParser::~AbstractParser()
{

}

const char* AbstractParser::descriptionOf(int errNo)
{
    return E_ERROR_DESCRIPTION[errNo];
}
