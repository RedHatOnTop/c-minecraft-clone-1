#pragma once

#include <stdexcept>
#include <string>

#define TEST_ASSERT(cond)                                                                                               \
    do {                                                                                                                \
        if (!(cond)) {                                                                                                  \
            throw std::runtime_error(std::string("Assertion failed: ") + #cond);                                       \
        }                                                                                                               \
    } while (0)
