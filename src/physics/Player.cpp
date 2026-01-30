#include "physics/Player.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>

Player::Player(glm::vec3 startPos) 
    : position(startPos), velocity(0.0f), camera(startPos), m_onGround(false), m_eyeHeight(1.62f), m_dimensions(0.6f, 1.8f, 0.6f) {
    camera.Position = position + glm::vec3(0, m_eyeHeight, 0);
}

void Player::update(float deltaTime, World& world) {
    applyPhysics(deltaTime, world);
    camera.Position = position + glm::vec3(0, m_eyeHeight, 0);
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

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && m_onGround) {
        velocity.y = 8.0f; // Basic jump
    }
}

void Player::applyPhysics(float deltaTime, World& world) {
    velocity.y -= 24.0f * deltaTime; // Gravity
    
    glm::vec3 nextPos = position + velocity * deltaTime;
    
    // Very simplified collision: just check if the feet/head are in a solid block
    // In a real implementation, we'd check all blocks the AABB intersects
    if (world.getBlock((int)std::floor(nextPos.x), (int)std::floor(nextPos.y), (int)std::floor(nextPos.z)).id != Blocks::AIR) {
        velocity.y = 0;
        m_onGround = true;
    } else {
        m_onGround = false;
        position = nextPos;
    }
}

AABB Player::getAABB() const {
    return AABB(position - glm::vec3(m_dimensions.x/2, 0, m_dimensions.z/2), 
                position + glm::vec3(m_dimensions.x/2, m_dimensions.y, m_dimensions.z/2));
}
