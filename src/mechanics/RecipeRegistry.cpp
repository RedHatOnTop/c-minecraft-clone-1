#include "mechanics/RecipeRegistry.hpp"

// Stub — implementation in task 8.1
void RecipeRegistry::registerShaped(const ShapedRecipe& recipe) {
    m_shaped.push_back(recipe);
}

void RecipeRegistry::registerShapeless(const ShapelessRecipe& recipe) {
    m_shapeless.push_back(recipe);
}

RecipeMatch RecipeRegistry::resolve(const std::vector<ItemStack>& /*grid*/, int /*gridWidth*/, int /*gridHeight*/) const {
    return {false, {}, -1};
}

void RecipeRegistry::consumeInputs(std::vector<ItemStack>& /*grid*/, int /*recipeIndex*/) const {
}

void RecipeRegistry::loadDefaultRecipes() {
}

RecipeMatch RecipeRegistry::matchShaped(const std::vector<ItemStack>& /*grid*/, int /*w*/, int /*h*/) const {
    return {false, {}, -1};
}

RecipeMatch RecipeRegistry::matchShapeless(const std::vector<ItemStack>& /*grid*/) const {
    return {false, {}, -1};
}
