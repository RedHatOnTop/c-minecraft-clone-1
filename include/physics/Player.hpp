#pragma once

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "render/Camera.hpp"
#include "entity/Entity.hpp"
#include "item/Inventory.hpp"

class Player : public Entity {
public:
    Camera camera;
    int selectedSlot = 0;
    Inventory inventory;
    
    Player(glm::vec3 startPos);
    
    void update(float deltaTime, World& world) override;
    void handleInput(GLFWwindow* window, float deltaTime);

    bool addToInventory(ItemStack& stack);

private:
    float m_eyeHeight;
};
