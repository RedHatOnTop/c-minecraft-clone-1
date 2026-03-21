#include "test_common.hpp"
#include "mechanics/SmeltingRecipeRegistry.hpp"

// Stub — property and unit tests added in task 10.4
void test_Smelting_RecipeRegistryLookup() {
    SmeltingRecipeRegistry registry;
    registry.loadDefaults();
    auto* recipe = registry.findRecipe(0);
    (void)recipe;
}
