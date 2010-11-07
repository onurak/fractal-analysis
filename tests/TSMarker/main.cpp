#include "TSMarkerTest.h"

int main(int argc, char *argv[])
{
    TSMarkerTest test;
    GUnitTestResults result;
    test.run(result);

    result.writeToFile("Result.txt");
    std::cout << result.toString();
    char c;
    std::cin >> c;

    return 0;
}
