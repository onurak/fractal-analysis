#ifndef MULTIPASS_H
#define MULTIPASS_H

#include "AbstractParser.h"

namespace FL { namespace Parsers {

/*! \class MultiPass
  * \brief Multi pass (recursive) parser. Can find all parse trees.
  *
  * It is very slow (about O(n!)).
  */
class MultiPass : public AbstractParser
{
public:
    MultiPass();

    virtual ParseResult analyze(
        const TimeSeries &ts,
        Trees::Forest &forest,
        Patterns::PatternsSet &patterns,
        Patterns::Matcher &matcher,
        Trees::MetricsSet &metrics,
        int begin = 0,
        int end = -1);
protected:
    void newAnalysisBranchForTree(FL::Trees::Tree &tree);

    void newAnalysisBranch(Patterns::Context *context);

    void runBranch(Patterns::Context *context, Patterns::Matcher &matcher);

    bool match(Patterns::Matcher &matcher, Patterns::Context &context);

    void removeSubtrees(FL::Trees::Forest &forest);

    int maxLevel(const FL::Trees::Forest &forest);

    bool isDuplicatingContext(Patterns::Context *context);

    void deleteContext(Patterns::Context *context);

    FL::ParseResult m_result;
    const TimeSeries *m_ts;
    Trees::Forest m_forest;
    Patterns::PatternsSet *m_patterns;
    std::vector<Patterns::Context*> m_branches;
    bool m_interruption;

    std::vector<Patterns::Context*> m_oldBranches;
};

}} // namespaces

#endif // MULTIPASS_H
