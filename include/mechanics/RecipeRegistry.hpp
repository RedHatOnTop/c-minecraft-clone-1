#pragma once

#include "item/ItemStack.hpp"
#include <vector>

struct RecipeSlot {
    ItemID itemID;  // 0 = empty/wildcard
    int metadata;   // -1 = any metadata
};

struct ShapedRecipe {
    int width, height;
    std::vector<RecipeSlot> pattern;
    ItemStack output;
    int priority;
};

struct ShapelessRecipe {
    std::vector<RecipeSlot> ingredients;
    ItemStack output;
    int priority;
};

struct RecipeMatch {
    bool found;
    ItemStack output;
    int recipeIndex;
};

class RecipeRegistry {
public:
    void registerShaped(const ShapedRecipe& recipe);
    void registerShapeless(const ShapelessRecipe& recipe);
    RecipeMatch resolve(const std::vector<ItemStack>& grid, int gridWidth, int gridHeight) const;
    void consumeInputs(std::vector<ItemStack>& grid, int recipeIndex) const;
    void loadDefaultRecipes();

private:
    std::vector<ShapedRecipe> m_shaped;
    std::vector<ShapelessRecipe> m_shapeless;

    RecipeMatch matchShaped(const std::vector<ItemStack>& grid, int w, int h) const;
    RecipeMatch matchShapeless(const std::vector<ItemStack>& grid) const;
};
