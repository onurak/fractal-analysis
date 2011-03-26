#ifndef TESTING_H
#define TESTING_H

#include <iostream>
#include "../fractallib/FL.h"

using namespace FL::Patterns;
using namespace FL::Exceptions;
using namespace FL::IO;

std::string compilationError(const EParsing &e)
{
    std::string result;
    char buf[100];
    switch (e.id())
    {
        case FL::E_OK:
            result = "OK";
            break;
        case FL::E_EMPTY_INPUT:
            result = "Error: Input is empty";
            break;
        case FL::E_EXPECTED_TOKEN:
            sprintf(buf, "Error at (%d, %d): Expected '%s'",
                    e.line(), e.column(), e.arg().c_str());
            result = buf;
            break;
        case FL::E_UNEXPECTED_TOKEN:
            sprintf(buf, "Error at (%d, %d): Unexpected '%s'",
                    e.line(), e.column(), e.arg().c_str());
            result = buf;
            break;
        case FL::E_SYNTAX_ERROR:
            sprintf(buf, "Error at (%d, %d): Syntax error '%s'",
                    e.line(), e.column(), e.arg().c_str());
            result = buf;
            break;
        case FL::E_UNKNOWN_IDENTIFIER:
            sprintf(buf, "Error at (%d, %d): Unknown identifier '%s'",
                    e.line(), e.column(), e.arg().c_str());
            result = buf;
            break;
    }
    return result;
}

void test_Patterns()
{
    logg.info("TEST: Patterns... ");

    Pattern zigzag(new Standart::EbnfRpnPattern);
    std::string patternString = "zigzag = up_1 down_2 @ min(up_1) > min(down_2);";
    logg << compilationError(zigzag.compile(patternString));
}

void test_Trees()
{
    logg.info("TEST: Trees... ");
    try
    {
        using namespace FL::Trees;
        const int MAX_NODES = 1000;

        Tree tree;
        forrange(i, 1, MAX_NODES)
            tree.add(new Node(NULL, i, i, i+1, 0));

        Layer::ConstIterator node1, node2;
        forboth(node1, tree.leafs(), node2, tree.roots())
            if (*node1 != *node2)
                throw "Invalid tree structure (roots != leafs)";

        logg << "OK";
    }
    catch (const char *s)
    {
        logg << s;
    }

}

void test_TimeSeries()
{
    logg.info("TEST: TimeSeries and FileCSV... ");

    FL::TimeSeries ts, ts1;
    FileCSV file;
    try
    {
        if (!file.open("test.csv"))
            throw "Can't open file test.csv";

        //logg.info("Header: ");
        //FileCSV::Header::const_iterator s;
        //forall(s, file.header())
          //  logg << "'" << *s << "' ";

        if (!file.read(ts, "<CLOSE>", FL::TimeSeries::dtValues))
            throw "Can't read values data from test.csv";
        if (!file.read(ts, "<TIME>", FL::TimeSeries::dtTime))
            throw "Can't read time data from test.csv";
        if (!file.save("test_out.csv", ts))
            throw "Can't save test_out.csv";


        if (!file.open("test_out.csv"))
            throw "Can't open file test_out.csv";
        if (!file.read(ts1, "<CLOSE>", FL::TimeSeries::dtValues))
            throw "Can't read values data from test_out.csv";
        if (!file.read(ts1, "<TIME>", FL::TimeSeries::dtTime))
            throw "Can't read time data from test_out.csv";


        if (ts.time().size() != ts1.time().size() ||
            ts.values().size() != ts1.values().size())
          throw "Invalid time series size";
        FL::TimeSeries::Data::const_iterator i, i1;
        for (i = ts.time().begin(), i1 = ts1.time().begin();
             i != ts.time().end() && i1 != ts1.time().end();
             ++i, ++i1)
        {
            if (*i != *i1)
                throw "Invalid time series time";
        }

        for (i = ts.values().begin(), i1 = ts1.values().begin();
             i != ts.values().end() && i1 != ts1.values().end();
             ++i, ++i1)
        {
            if (*i != *i1)
                throw "Invalid time series time";
        }

        logg << "OK";
    }
    catch (const char * s)
    {
        logg << s;
    }
}

void test_Markers()
{
    logg.info("TEST: Markers");
    try
    {
        FL::TimeSeries ts;
        FileCSV file;
        if (!file.open("test.csv"))
            throw "Can't open file test.csv";
        if (!file.read(ts, "<CLOSE>", FL::TimeSeries::dtValues))
            throw "Can't read values data from test.csv";
        if (!file.read(ts, "<TIME>", FL::TimeSeries::dtTime))
            throw "Can't read time data from test.csv";

        FL::Trees::Forest forest;
        FL::Patterns::PatternsSet patterns;

        FL::Markers::AB marker;
        FL::ParseResult pr = marker.analyze(ts, forest, patterns);
        if (pr.nodesAdded == 0 || pr.treesAdded == 0)
            throw "Nothing was marked";
        logg.info("Begining of marked sequence: ");
        FL::Trees::Tree &tree = *forest[0];
        FL::Trees::Layer::ConstIterator node;
        int i = 0;
        forall(node, tree.leafs())
        {
            if (i++ > 15) break;
            logg << FL::IDGenerator::nameOf((*node)->id()) << " ";
        }

        logg << "...";
        logg.info("OK");
     }
    catch (const char *s)
    {
        logg << s;
    }
}

void test_Parsers()
{
    logg.info("TEST: Parsers");
    try
    {
        // Open time series
        FL::TimeSeries ts;
        FileCSV file;
        if (!file.open("test.csv"))
            throw "Can't open file test.csv";
        if (!file.read(ts, "<CLOSE>", FL::TimeSeries::dtValues))
            throw "Can't read values data from test.csv";
        if (!file.read(ts, "<TIME>", FL::TimeSeries::dtTime))
            throw "Can't read time data from test.csv";

        // Open patterns
        FL::Trees::Forest forest;
        FL::Patterns::PatternsSet patterns;
        FL::IO::FilePAT patFile;
        if (!patFile.open("test.pat", patterns))
            throw
              std::string("Can't load file with patterns test.pat: ")
                + compilationError(patFile.lastError());

        // Markup
        FL::Markers::AB marker;
        FL::ParseResult pr = marker.analyze(ts, forest, patterns);
        if (pr.nodesAdded == 0 || pr.treesAdded == 0)
            throw "Nothing was marked";
        logg.info("OK");

        // Parse with SinglePass
        FL::Parsers::SinglePass singlePassParser;
        do
        {
            pr = singlePassParser.analyze(ts, forest, patterns);
        } while (pr.nodesAdded > 0);

        FL::Trees::Tree &tree = *forest[0];
        FL::Trees::Layer::ConstIterator node;
        logg.info("Begining of tree").endl();
        for (int level = 0; level < tree.levelCount(); ++level)
        {
            int i = 0;
            const FL::Trees::Layer &levelLayer = tree.nodesByLevel(level);
            forall(node, levelLayer)
            {
                if (i++ > 10/(level+1)) break;
                //logg << std::setw(8) << std::right << FL::IDGenerator::nameOf((*node)->id());
                logg << FL::IDGenerator::nameOf((*node)->id())
                     << " (" << (*node)->begin() << ", " << (*node)->end() << ") ";
            }
            logg << "...\n";
        }
     }
    catch (const char *s)
    {
        logg.error(s);
    }
    catch (const std::string &s)
    {
        logg.error(s);
    }
}


#endif // TESTING_H
