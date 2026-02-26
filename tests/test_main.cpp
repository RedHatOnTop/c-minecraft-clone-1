#include <iostream>
#include <string>
#include <vector>

void test_DataMigration_ChunkLegacyToV2_RoundTrip();
void test_DataMigration_PlayerEntity_RoundTrip();
void test_Inventory_AddItem_NullRegistrySafe();
void test_Entity_MobHealthPersistence();
void test_Raycast_ZeroDirectionComponent_NoCrash();
void test_UI_ViewportResize_ProjectionUpdate();
void test_ChunkMesh_TransparentSort_Stable();

int main() {
    struct TestCase {
        const char* name;
        void (*fn)();
    };

    const std::vector<TestCase> tests = {
        {"DataMigration_ChunkLegacyToV2_RoundTrip", test_DataMigration_ChunkLegacyToV2_RoundTrip},
        {"DataMigration_PlayerEntity_RoundTrip", test_DataMigration_PlayerEntity_RoundTrip},
        {"Inventory_AddItem_NullRegistrySafe", test_Inventory_AddItem_NullRegistrySafe},
        {"Entity_MobHealthPersistence", test_Entity_MobHealthPersistence},
        {"Raycast_ZeroDirectionComponent_NoCrash", test_Raycast_ZeroDirectionComponent_NoCrash},
        {"UI_ViewportResize_ProjectionUpdate", test_UI_ViewportResize_ProjectionUpdate},
        {"ChunkMesh_TransparentSort_Stable", test_ChunkMesh_TransparentSort_Stable},
    };

    int failed = 0;
    for (const auto& test : tests) {
        try {
            test.fn();
            std::cout << "[PASS] " << test.name << "\n";
        } catch (const std::exception& ex) {
            std::cerr << "[FAIL] " << test.name << ": " << ex.what() << "\n";
            failed++;
        }
    }

    if (failed != 0) {
        std::cerr << failed << " test(s) failed.\n";
        return 1;
    }
    std::cout << "All tests passed.\n";
    return 0;
}
