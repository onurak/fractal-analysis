#include <iostream>
#include "BaseTest.h"

int main(int argc, char *argv[])
{
    BaseTest test;
    GUnitTestResults result;
    test.run(result);

    result.writeToFile("BaseTestResult.txt");
    std::cout << result.toString();
    char c;
    std::cin >> c;
    return 0;
}
