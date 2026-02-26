#include <vector>

#include "test_replay.hpp"
#include "test_runner.hpp"

int main() {
    const std::vector<TestCase> tests = {
        {"ReplayHarness_InventoryFlow_Deterministic", test_ReplayHarness_InventoryFlow_Deterministic},
        {"ReplayHarness_BlockRaycastFlow_Deterministic", test_ReplayHarness_BlockRaycastFlow_Deterministic},
        {"ReplayHarness_PersistenceFlow_Deterministic", test_ReplayHarness_PersistenceFlow_Deterministic},
    };
    return runTestSuite("tests_gameplay_smoke", tests);
}
