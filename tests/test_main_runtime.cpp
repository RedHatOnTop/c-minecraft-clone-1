#include <vector>

#include "test_runner.hpp"

void test_Inventory_AddItem_NullRegistrySafe();
void test_Physics_JumpApex_FixedTickProfile();
void test_Runtime_MemoryGrowth_SoakProfile();

// Phase 2 test declarations
void test_BreakDuration_StoneIronPickaxe();
void test_BreakDuration_BedrockUnbreakable();
void test_Placement_DirectionalMeta();
void test_SlotAction_LeftClickConservation();
void test_Recipe_ResolveRoundTrip();
void test_Smelting_RecipeRegistryLookup();

int main() {
    const std::vector<TestCase> tests = {
        {"Inventory_AddItem_NullRegistrySafe", test_Inventory_AddItem_NullRegistrySafe},
        {"Physics_JumpApex_FixedTickProfile", test_Physics_JumpApex_FixedTickProfile},
        {"Runtime_MemoryGrowth_SoakProfile", test_Runtime_MemoryGrowth_SoakProfile},
        // Phase 2: Block interaction, inventory, crafting, smelting
        {"BreakDuration_StoneIronPickaxe", test_BreakDuration_StoneIronPickaxe},
        {"BreakDuration_BedrockUnbreakable", test_BreakDuration_BedrockUnbreakable},
        {"Placement_DirectionalMeta", test_Placement_DirectionalMeta},
        {"SlotAction_LeftClickConservation", test_SlotAction_LeftClickConservation},
        {"Recipe_ResolveRoundTrip", test_Recipe_ResolveRoundTrip},
        {"Smelting_RecipeRegistryLookup", test_Smelting_RecipeRegistryLookup},
    };
    return runTestSuite("tests_runtime", tests);
}
