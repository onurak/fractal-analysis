#include "TreeFilter.h"

TreeFilter::TreeFilter()
{
}

void TreeFilter::createFromFile(const std::string &fileName)
{
    FL::FilePAT file;
    file.loadFromFile(fileName, m_patterns);
}

void TreeFilter::apply(FL::ParseTree &tree, FL::TimeSeries *ts)
{
    // create singlepass parser
    FL::TSParser *parser = NULL;
    if (!FL::TSParserFactory::create("simple", &parser, false))
        throw ETreeFilter("Can't create simple parser");

    // create universal groupchecker
    FL::Patterns::GroupChecker *groupChecker = NULL;
    if (!FL::Patterns::GroupCheckerFactory::create("universal", &groupChecker, false))
        throw ETreeFilter("Can't create universal parser");

    // add tree to temporary tree set
    FL::ParseTreeSet tmpTreeSet;
    tmpTreeSet.add(&tree);

    // analyse
    while (parser->parse(tmpTreeSet, m_patterns, ts, *groupChecker).parsedCount > 0)
        ;

    // erase tree from tree set
    std::vector<FL::ParseTree*>::iterator iter;
    for (iter = tmpTreeSet.trees.begin(); iter != tmpTreeSet.trees.end(); ++iter)
        if (*iter == &tree)
            break;
    tmpTreeSet.trees.erase(iter);
}

void TreeFilter::apply(FL::ParseTreeSet &set, FL::TimeSeries *ts)
{
    FL::ParseTreeSet::Iterator tree;
    for_each_(tree, set.trees)
        apply(**tree, ts);
}
