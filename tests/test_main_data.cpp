#include <vector>

#include "test_runner.hpp"

void test_DataMigration_ChunkLegacyToV2_RoundTrip();
void test_DataMigration_PlayerEntity_RoundTrip();
void test_Entity_MobHealthPersistence();
void test_WorldGen_BiomeDistribution_FunctionalBaseline();
void test_WorldGen_HeightAndDecoration_Profile();

int main() {
    const std::vector<TestCase> tests = {
        {"DataMigration_ChunkLegacyToV2_RoundTrip", test_DataMigration_ChunkLegacyToV2_RoundTrip},
        {"DataMigration_PlayerEntity_RoundTrip", test_DataMigration_PlayerEntity_RoundTrip},
        {"Entity_MobHealthPersistence", test_Entity_MobHealthPersistence},
        {"WorldGen_BiomeDistribution_FunctionalBaseline", test_WorldGen_BiomeDistribution_FunctionalBaseline},
        {"WorldGen_HeightAndDecoration_Profile", test_WorldGen_HeightAndDecoration_Profile},
    };
    return runTestSuite("tests_data", tests);
}
