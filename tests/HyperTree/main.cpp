#include <iostream>
#include "HyperTreeTest.h"

int main(int argc, char *argv[])
{
    GUnitTestResults results;
    HyperTreeTest test;
    test.run(results);
    std::cout << results.toString();
    results.writeToFile("HyperTreeUnitTesting.txt");

    char c;
    std::cin >> c;
    return 0;
}
