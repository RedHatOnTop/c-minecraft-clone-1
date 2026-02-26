#include <vector>

#include "test_runner.hpp"

void test_Raycast_ZeroDirectionComponent_NoCrash();
void test_UI_ViewportResize_ProjectionUpdate();
void test_ChunkMesh_TransparentSort_Stable();

int main() {
    const std::vector<TestCase> tests = {
        {"Raycast_ZeroDirectionComponent_NoCrash", test_Raycast_ZeroDirectionComponent_NoCrash},
        {"UI_ViewportResize_ProjectionUpdate", test_UI_ViewportResize_ProjectionUpdate},
        {"ChunkMesh_TransparentSort_Stable", test_ChunkMesh_TransparentSort_Stable},
    };
    return runTestSuite("tests_render", tests);
}
