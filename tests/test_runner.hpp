#pragma once

#include <exception>
#include <iostream>
#include <string>
#include <vector>

struct TestCase {
    const char* name;
    void (*fn)();
};

inline int runTestSuite(const std::string& suiteName, const std::vector<TestCase>& tests) {
    int failed = 0;
    for (const auto& test : tests) {
        try {
            test.fn();
            std::cout << "[" << suiteName << "][PASS] " << test.name << "\n";
        } catch (const std::exception& ex) {
            std::cerr << "[" << suiteName << "][FAIL] " << test.name << ": " << ex.what() << "\n";
            failed++;
        }
    }

    if (failed != 0) {
        std::cerr << "[" << suiteName << "] " << failed << " test(s) failed.\n";
        return 1;
    }

    std::cout << "[" << suiteName << "] All tests passed.\n";
    return 0;
}
