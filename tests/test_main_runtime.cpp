#include <vector>

#include "test_runner.hpp"

void test_Inventory_AddItem_NullRegistrySafe();

int main() {
    const std::vector<TestCase> tests = {
        {"Inventory_AddItem_NullRegistrySafe", test_Inventory_AddItem_NullRegistrySafe},
    };
    return runTestSuite("tests_runtime", tests);
}
