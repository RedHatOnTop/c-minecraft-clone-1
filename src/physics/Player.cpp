#include "physics/Player.hpp"
#include "util/SoundManager.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>

Player::Player(glm::vec3 startPos) 
    : Entity(startPos, glm::vec3(0.6f, 1.8f, 0.6f)), camera(startPos), inventory(36), m_eyeHeight(1.62f) {
    camera.Position = position + glm::vec3(0, m_eyeHeight, 0);
}

void Player::update(float deltaTime, World& world) {
    glm::vec3 oldPos = position;
    applyPhysics(deltaTime, world);
    camera.Position = position + glm::vec3(0, m_eyeHeight, 0);

    // Footstep sounds
    if (onGround && (std::abs(velocity.x) > 0.1f || std::abs(velocity.z) > 0.1f)) {
        static float stepTimer = 0;
        stepTimer += deltaTime;
        if (stepTimer > 0.35f) {
            Block b = world.getBlock((int)std::floor(position.x), (int)std::floor(position.y - 0.1f), (int)std::floor(position.z));
            if (b.id == Blocks::GRASS) SoundManager::playSound("step_grass");
            else if (b.id == Blocks::DIRT) SoundManager::playSound("step_dirt");
            else SoundManager::playSound("step_stone");
            stepTimer = 0;
        }
    }
}

void Player::handleInput(GLFWwindow* window, float deltaTime) {
    // Hotbar selection
    for (int i = 0; i < 9; ++i) {
        if (glfwGetKey(window, GLFW_KEY_1 + i) == GLFW_PRESS) {
            selectedSlot = i;
        }
    }

    float speed = 4.317f; // MC walking speed m/s
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) speed *= 1.3f; // Sprint

    glm::vec3 direction(0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) direction += camera.Front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) direction -= camera.Front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) direction -= camera.Right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) direction += camera.Right;

    direction.y = 0; // Keep movement horizontal
    if (glm::length(direction) > 0) {
        direction = glm::normalize(direction);
        velocity.x = direction.x * speed;
        velocity.z = direction.z * speed;
    } else {
        velocity.x = 0;
        velocity.z = 0;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && onGround) {
        velocity.y = 8.0f; // Basic jump
    }
}

bool Player::addToInventory(ItemStack& stack) {
    return inventory.addItem(stack);
}
