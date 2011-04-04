#ifndef SINGLEPASS_H
#define SINGLEPASS_H

#include "AbstractParser.h"

namespace FL { namespace Parsers {

class SinglePass : public AbstractParser
{
public:
    SinglePass();

    virtual ParseResult analyze(
        const TimeSeries &ts,
        Trees::Forest &forest,
        Patterns::PatternsSet &patterns);
protected:
    FL::Trees::Tree* analyzeTree(const TimeSeries &ts, Trees::Tree &tree, Patterns::PatternsSet &patterns);

    bool applyPattern(Patterns::Pattern &pattern, Patterns::Context &context);

    FL::ParseResult m_result;
};

}} // namespaces

#endif // SINGLEPASS_H
