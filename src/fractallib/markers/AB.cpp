#include "AB.h"

using namespace FL::Markers;
using namespace FL::Exceptions;
using namespace FL::Trees;
using namespace FL::Patterns;

AB::AB()
{
}

AB::~AB()
{

}

FL::ParseResult AB::analyze(
        const TimeSeries &ts, Forest &forest, PatternsSet &patterns)
{
    ParseResult result;

    try
    {
        if (ts.values().size() < 2)
            throw EAnalyze(E_INVALID_INPUT);

        Tree *tree = new Tree();
        const int up   = IDGenerator::idOf("a");
        const int down = IDGenerator::idOf("b");

        for (unsigned int i = 0; i < ts.values().size(); i += 2)
        {
            const int &id =  (ts.values()[i] >= ts.values()[i+1])  ?  up  :  down;
            tree->add(new Node(NULL, id, i, i+1, 0));
        }
        forest.push_back(tree);
        result.treesAdded = 1;
        result.nodesAdded = (ts.values().size() + 1) / 2;
    }
    catch (const EAnalyze &e)
    {

    }
    return result;
}
