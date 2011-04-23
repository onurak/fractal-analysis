#ifndef SINGLEPASS_H
#define SINGLEPASS_H

#include "AbstractParser.h"

namespace FL { namespace Parsers {

/*! \class SinglePass
  * \brief Single pass parser. Find only one parse tree.
  *
  * It always applies petterns in direct order.
  */
class SinglePass : public AbstractParser
{
public:
    SinglePass();

    virtual ParseResult analyze(
        const TimeSeries &ts,
        Trees::Forest &forest,
        Patterns::PatternsSet &patterns,
        int begin = 0,
        int end = -1);
protected:
    void analyzeTree(const TimeSeries &ts, Trees::Tree &tree, Patterns::PatternsSet &patterns);

    bool applyPattern(Patterns::Pattern &pattern, Patterns::Context &context);

    int m_begin, m_end;
    FL::ParseResult m_result;
};

}} // namespaces

#endif // SINGLEPASS_H
