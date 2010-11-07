#ifndef TREEFILTER_H
#define TREEFILTER_H

#include "../fractallib/fl.h"
#include "../fractallib/groupcheckers/GroupCheckerFactory.h"
#include <QFile>

class ETreeFilter
{
public:
    ETreeFilter(const std::string &msg): msg(msg) {}
    const std::string msg;
};

/*! \class TreeFilter
  * \brief Fractal-based parse tree filter
  */
class TreeFilter
{
public:
    TreeFilter();
    //! Create filter from PAT-file
    /*! File must exists!
      */
    void createFromFile(const std::string &fileName);

    void apply(FL::ParseTree &tree, FL::TimeSeries *ts);
    void apply(FL::ParseTreeSet &set, FL::TimeSeries *ts);
private:
    FL::PatternCollection m_patterns;
};

#endif // TREEFILTER_H
