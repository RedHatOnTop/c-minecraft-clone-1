#pragma once

#include "item/ItemStack.hpp"
#include <vector>

struct SmeltingRecipe {
    ItemID inputID;
    ItemStack output;
    int smeltDuration;  // ticks (default 200)
};

struct FuelEntry {
    ItemID fuelID;
    int burnDuration;   // ticks
};

class SmeltingRecipeRegistry {
public:
    void registerRecipe(const SmeltingRecipe& recipe);
    void registerFuel(const FuelEntry& fuel);
    const SmeltingRecipe* findRecipe(ItemID inputID) const;
    const FuelEntry* findFuel(ItemID fuelID) const;
    void loadDefaults();

private:
    std::vector<SmeltingRecipe> m_recipes;
    std::vector<FuelEntry> m_fuels;
};
