#include "Testing.h"

//GLogger logg("./lastrun.log");

void GlobalErrorHandler(GErrorType type, const std::string& notifier, int errNo, const std::string& description)
{
    throw "Error";
}

int main(int argc, char *argv[])
{
    GErrorClass::onError += delegate(GlobalErrorHandler);

    test_Patterns();
    test_TimeSeries();
    test_Trees();
    test_Markers();
    test_Parsers();

    logg.info("All tests complete");
    logg.info("Press enter...");
    std::cin.ignore();
    return 0;
}
