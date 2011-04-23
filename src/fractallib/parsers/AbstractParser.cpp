#include "AbstractParser.h"

using namespace FL::Parsers;

const char *E_ERROR_DESCRIPTION[] =
{
  "Empty time series",
  "Empty pattern set",
  "Empty forest",
  "Invalid segment"
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

FL::ParseResult AbstractParser::parseAll(
    const TimeSeries &ts,
    Trees::Forest &forest,
    Patterns::PatternsSet &patterns,
    int begin,
    int end)
{
    FL::ParseResult result, r;
    do
    {
        r.reset();
        r = analyze(ts, forest, patterns, begin, end);
        result += r;
    } while (wasOk() && r.somethingAdded());

    return result;
}
