#pragma once

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "render/Camera.hpp"
#include "physics/AABB.hpp"
#include "world/World.hpp"

class Player {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    Camera camera;
    
    Player(glm::vec3 startPos);
    
    int selectedSlot = 0;

    void update(float deltaTime, World& world);
    void handleInput(GLFWwindow* window, float deltaTime);
    
    AABB getAABB() const;

private:
    bool m_onGround;
    float m_eyeHeight;
    glm::vec3 m_dimensions;

    void applyPhysics(float deltaTime, World& world);
};
