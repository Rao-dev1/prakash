#ifndef TESTS_H
#define TESTS_H

#include <string>

namespace Tests {
    // Runs all Section 34 verification tests and displays summary
    bool runAllTests(bool verbose = true);

    // Returns JSON representation of test results for dashboard integration
    std::string runAllTestsJSON();
}

#endif // TESTS_H
