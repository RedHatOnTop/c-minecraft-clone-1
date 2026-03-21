#include <vector>

#include "test_replay.hpp"
#include "test_runner.hpp"

// Phase 2 test declarations
void test_PhysicsParity_WalkSpeedConvergence();
void test_PhysicsParity_SprintSpeedConvergence();
void test_SurvivalScenario_30MinRun();

int main() {
    const std::vector<TestCase> tests = {
        {"ReplayHarness_InventoryFlow_Deterministic", test_ReplayHarness_InventoryFlow_Deterministic},
        {"ReplayHarness_BlockRaycastFlow_Deterministic", test_ReplayHarness_BlockRaycastFlow_Deterministic},
        {"ReplayHarness_PersistenceFlow_Deterministic", test_ReplayHarness_PersistenceFlow_Deterministic},
        // Phase 2: Physics parity and survival scenario
        {"PhysicsParity_WalkSpeedConvergence", test_PhysicsParity_WalkSpeedConvergence},
        {"PhysicsParity_SprintSpeedConvergence", test_PhysicsParity_SprintSpeedConvergence},
        {"SurvivalScenario_30MinRun", test_SurvivalScenario_30MinRun},
    };
    return runTestSuite("tests_gameplay_smoke", tests);
}
