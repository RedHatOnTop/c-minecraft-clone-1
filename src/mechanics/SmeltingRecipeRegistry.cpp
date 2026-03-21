#include "mechanics/SmeltingRecipeRegistry.hpp"

// Stub — implementation in task 10.1
void SmeltingRecipeRegistry::registerRecipe(const SmeltingRecipe& recipe) {
    m_recipes.push_back(recipe);
}

void SmeltingRecipeRegistry::registerFuel(const FuelEntry& fuel) {
    m_fuels.push_back(fuel);
}

const SmeltingRecipe* SmeltingRecipeRegistry::findRecipe(ItemID inputID) const {
    for (const auto& r : m_recipes) {
        if (r.inputID == inputID) return &r;
    }
    return nullptr;
}

const FuelEntry* SmeltingRecipeRegistry::findFuel(ItemID fuelID) const {
    for (const auto& f : m_fuels) {
        if (f.fuelID == fuelID) return &f;
    }
    return nullptr;
}

void SmeltingRecipeRegistry::loadDefaults() {
}
