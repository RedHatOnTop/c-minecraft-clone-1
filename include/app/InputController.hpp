#pragma once

#include "app/GameContext.hpp"

struct GLFWwindow;
class ChestBlockEntity;
class FurnaceBlockEntity;

namespace app {
bool isUIOpen(const GameContext& ctx);
void ensureOpenedContainerValid(GameContext& ctx, GLFWwindow* window);
ChestBlockEntity* getOpenChest(GameContext& ctx);
FurnaceBlockEntity* getOpenFurnace(GameContext& ctx);
void processInput(GameContext& ctx, GLFWwindow* window);
void handleBlockInteraction(GameContext& ctx, GLFWwindow* window);
void handleContainerAndCraftShortcuts(GameContext& ctx, GLFWwindow* window);
}  // namespace app
