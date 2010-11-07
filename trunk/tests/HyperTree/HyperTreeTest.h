#ifndef HYPERTREETEST_H
#define HYPERTREETEST_H

#include "../../src/fractallib/G/GUnitTest.h"
#include "../../src/fractallib/HyperTree.h"

/*! \addtogroup FLTesting
  */
/*@{*/

/*! \file
  * \brief Hyper Tree Unit Test
  *
  * Contains unit tests for Hyper Tree (FL::HyperTree).
  */

std::string i2s(int v)
{
    char s[66];
    FL::i2a(v, s);
    return s;
}

class HyperTreeTest: public GUnitTest
{
public:
    HyperTreeTest(): GUnitTest() {}

    //! Do the unit testing
    void run(GUnitTestResults &res) {
        typedef FL::HyperTree<FL::EmptyNode> TestTree;
        typedef FL::HyperTreeSet<FL::EmptyNode> TestTreeSet;
        typedef FL::EmptyNode Node;
        string descr;

        res.clear();

        // 1. Creation/destruction
        GBeginTest
            TestTree *tree = new TestTree();
            delete tree;
        GEndTest("Creation/Destruction", "", res);

        // 2. Create single tree with 1.000.000 nodes, make copy of it,
        //    add nodes to original and copied trees, make one more copy
        //    for this copy and add more 100  nodes to it, HyperTreeSet tests,
        //    NodeGroup statistics tests, memory usage test
        GBeginTest
            TestTreeSet treeSet;
            const int nodesToCreate = 1000000;
            TestTree *tree = new TestTree(&treeSet);
            for (int i = 0; i < nodesToCreate; ++i)
                tree->addNode(new Node());
            TestTree *tree1 = tree->copy();

            for (int i = 0; i < 100; ++i)
                tree->addNode(new Node);
            for (int i = 0; i < 100; ++i)
                tree1->addNode(new Node);

            TestTree *tree2 = tree1->copy();
            for (int i = 0; i < 100; ++i)
                tree2->addNode(new Node);


            unsigned int maxMemUsage = (sizeof(Node) + sizeof(Node*)) * (nodesToCreate + 100*2 + 100);
            unsigned int realMemUsage = treeSet.bytesUsed();
            unsigned int maxPossibleMemUsage = (sizeof(Node) + sizeof(Node*)) * (3*(nodesToCreate + 100) + 100);
            if (realMemUsage > maxMemUsage)
                throw (descr = "Too many memory used");
            else
                descr = "Average memory used: " + i2s(realMemUsage/1024) + " Kb of " +
                        i2s(maxMemUsage/1024) + " Kb limit (without smart coping it would be "
                        + i2s(maxPossibleMemUsage/1024) + " Kb)";
            if (treeSet.nodeGroups().size() != 5)
                throw (descr = "Invalid number of node groups");

            delete tree2;
            delete tree1;
            delete tree;
            if (treeSet.nodeGroups().size() > 0)
                throw (descr = "Invalid node groups usage");
        GEndTest("Create single tree with 1.000.000 nodes, make copy of it, "
                 "add 100 nodes to original and copied trees, make copy of copy and add to "
                 "it 100 nodes. HyperTreeSet tests. "
                 "NodeGroup statistics tests. Memory usage test", descr, res);

    }
};

/*@}*/

#endif // HYPERTREETEST_H
