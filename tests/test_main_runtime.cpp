#include <vector>

#include "test_runner.hpp"

void test_Inventory_AddItem_NullRegistrySafe();
void test_Physics_JumpApex_FixedTickProfile();
void test_Runtime_MemoryGrowth_SoakProfile();

int main() {
    const std::vector<TestCase> tests = {
        {"Inventory_AddItem_NullRegistrySafe", test_Inventory_AddItem_NullRegistrySafe},
        {"Physics_JumpApex_FixedTickProfile", test_Physics_JumpApex_FixedTickProfile},
        {"Runtime_MemoryGrowth_SoakProfile", test_Runtime_MemoryGrowth_SoakProfile},
    };
    return runTestSuite("tests_runtime", tests);
}
