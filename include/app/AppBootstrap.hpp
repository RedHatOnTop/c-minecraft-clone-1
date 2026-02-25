#pragma once

#include "app/GameContext.hpp"

struct GLFWwindow;

namespace app {
void configureWindowCallbacks(GLFWwindow* window, GameContext& ctx);
bool initializeGame(GameContext& ctx, GLFWwindow* window);
}  // namespace app
