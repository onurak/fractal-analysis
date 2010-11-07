#ifndef BASETEST_H
#define BASETEST_H

#include "../../src/fractallib/G/GUnitTest.h"
#include "../../src/fractallib/FileCSV.h"
#include "../../src/fractallib/FilePAT.h"

/*! \addtogroup FLTesting
  */
/*@{*/

/*! \file
  * \brief Base tests for most small classes and subsystems
  *
  * Contains unit tests for FL::TimeSeries, FL::Template, FL::Guard, FL::Ebnf,
  * FL::FileCSV, FL::FileTPL
  */


/*! \class TSMarkerTest
  * \brief Unit test for most small clases and subsystems
  *
  * Contains tests for FL::TimeSeries, FL::Template, FL::Guard, FL::Ebnf,
  * FL::FileCSV, FL::FilePAT
  */
class BaseTest : public GUnitTest
{
public:
    BaseTest(): GUnitTest() {}

    void run(GUnitTestResults &res) {
        res.clear();
        string descr;

        // 1. TimeSeries create, load (test of FileCSV), modify, destroy
        GBeginTest
            FL::TimeSeries *ts = new FL::TimeSeries();
            FL::FileCSV file("./test.csv");
            file.loadTimeSeries(ts, 0);
            std::vector<double> v;
            if (!ts->getTSByIndex(v, 0) || v.size() == 0)
                throw (descr = "Invalid time series size after it was loaded");

            ts->add(101010);
            unsigned int sz;
            if (!ts->getTSByIndex(v, 0) || (sz = v.size()) == 0 || v[sz-1] != 101010)
                throw (descr = "Error while tring add value to time series");
            ts->remove(sz-1);
            if (!ts->getTSByIndex(v, 0) || v.size() != sz-1)
                throw (descr = "Error while tring rempve value from time series");
            delete ts;
        GEndTest("TimeSeries create, load (test of FileCSV), modify, destroy", descr, res);

        // 2. Patterns creationg, loading (test of FilePAT), testing most functions, destroing
        GBeginTest
            FL::Patterns patterns;
            FL::FilePAT file;
            if (!file.loadFromFile("test.pat", &patterns))
                throw (descr = "Error loading from test.pat");
            FL::Pattern *pattern = patterns[0];
            patterns.clear();
        GEndTest("Patterns creationg, loading (test of FilePAT), modifing, testing most functions, destroing",
                 descr, res);


    }
};

#endif // BASETEST_H
