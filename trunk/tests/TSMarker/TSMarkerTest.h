#ifndef TSMARKERTEST_H
#define TSMARKERTEST_H

#include "../../src/fractallib/G/GUnitTest.h"
#include "../../src/fractallib/TSMarker.h"
#include "../../src/fractallib/TSMarkerSimple.h"
#include "../../src/fractallib/FileCSV.h"
#include "../../src/fractallib/FilePAT.h"

/*! \addtogroup FLTesting
  */
/*@{*/

/*! \file
  * \brief Time Series Marker Unit Test
  *
  * Contains unit tests for FL::TSMarker and his children.
  */


/*! \class TSMarkerTest
  * \brief Unit test for FL::TSMarker and his children
  */
class TSMarkerTest : public GUnitTest
{
public:
    TSMarkerTest(): GUnitTest() {}

    void run(GUnitTestResults &res) {
        res.clear();
        std::string descr;

        GBeginTest
            FL::TSMarker *marker = new FL::TSMarkerSimple();
            FL::ParseTreeSet trees;
            FL::TimeSeries ts;
            FL::FileCSV file("test.csv");
            file.loadTimeSeries(&ts, 0);
            marker->analyse(&ts, trees);

            if (trees.size() == 0)
                throw descr = "No parse trees were created, must be at least 1";

            FL::ParseTree *tree = (FL::ParseTree*)(trees[0]);
            FL::ParseTree::ConstLayer::const_iterator leaf;
            descr = "Markup result: ";
            for_each_(leaf, tree->leafes())
                descr += (*leaf)->name + " ";

        GEndTest("TSMarkerSimple test", descr, res);
    }
};

/*@}*/

#endif // TSMARKERTEST_H
