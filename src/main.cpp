#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "render/Shader.hpp"
#include "render/TextureAtlas.hpp"
#include "world/World.hpp"
#include "world/WorldGenerator.hpp"
#include "world/ChunkProvider.hpp"
#include "physics/Player.hpp"
#include "physics/Raycast.hpp"
#include "render/SelectionBox.hpp"
#include "ui/FontRenderer.hpp"
#include "ui/GUIRenderer.hpp"
#include "ui/HUD.hpp"

#include "world/BlockState.hpp"

void registerBlocks() {
    BlockState stone;
    stone.name = "stone";
    stone.faceTextures = {
        {Face::UP, "stone"}, {Face::DOWN, "stone"},
        {Face::NORTH, "stone"}, {Face::SOUTH, "stone"},
        {Face::WEST, "stone"}, {Face::EAST, "stone"}
    };
    BlockStateManager::registerBlockState(Blocks::STONE, stone);

    BlockState dirt;
    dirt.name = "dirt";
    dirt.faceTextures = {
        {Face::UP, "dirt"}, {Face::DOWN, "dirt"},
        {Face::NORTH, "dirt"}, {Face::SOUTH, "dirt"},
        {Face::WEST, "dirt"}, {Face::EAST, "dirt"}
    };
    BlockStateManager::registerBlockState(Blocks::DIRT, dirt);

    BlockState grass;
    grass.name = "grass";
    grass.faceTextures = {
        {Face::UP, "grass_top"}, {Face::DOWN, "dirt"},
        {Face::NORTH, "grass_side"}, {Face::SOUTH, "grass_side"},
        {Face::WEST, "grass_side"}, {Face::EAST, "grass_side"}
    };
    BlockStateManager::registerBlockState(Blocks::GRASS, grass);
}

// Global state for simplicity in main.cpp
Player* g_player;
bool g_firstMouse = true;
float g_lastX = 400, g_lastY = 300;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (g_firstMouse) {
        g_lastX = xpos;
        g_lastY = ypos;
        g_firstMouse = false;
    }

    float xoffset = xpos - g_lastX;
    float yoffset = g_lastY - ypos; // reversed since y-coordinates go from bottom to top
    g_lastX = xpos;
    g_lastY = ypos;

    g_player->camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    g_player->selectedSlot -= (int)yoffset;
    if (g_player->selectedSlot < 0) g_player->selectedSlot = 8;
    if (g_player->selectedSlot > 8) g_player->selectedSlot = 0;
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Minecraft 1.8.9 Clone", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize gl3w
    if (gl3wInit()) {
        std::cerr << "Failed to initialize gl3w" << std::endl;
        return -1;
    }

    // Systems initialization
    registerBlocks();
    Shader worldShader("assets/minecraft/shaders/basic.vert", "assets/minecraft/shaders/basic.frag");
    TextureAtlas atlas(16);
    atlas.addTexture("stone", "assets/minecraft/textures/blocks/stone.png");
    atlas.addTexture("dirt", "assets/minecraft/textures/blocks/dirt.png");
    atlas.addTexture("grass_top", "assets/minecraft/textures/blocks/grass_top.png");
    atlas.addTexture("grass_side", "assets/minecraft/textures/blocks/grass_side.png");
    atlas.generate();

    World world;
    WorldGenerator generator(12345);
    ChunkProvider chunkProvider(world, generator, atlas);
    g_player = new Player(glm::vec3(0, 80, 0));

    SelectionBox selectionBox;

    // UI initialization
    // For now we use placeholders for the font texture
    FontRenderer font("assets/minecraft/font/ascii.png");
    GUIRenderer gui;
    HUD hud(font, gui);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    int frameCount = 0;
    float fpsTimer = 0.0f;
    int currentFPS = 0;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        fpsTimer += deltaTime;
        frameCount++;
        if (fpsTimer >= 1.0f) {
            currentFPS = frameCount;
            frameCount = 0;
            fpsTimer = 0.0f;
        }

        // Input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        // Block Interaction
        static bool leftMouseDown = false;
        static bool rightMouseDown = false;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (!leftMouseDown) {
                RaycastResult res = Raycaster::raycast(g_player->camera.Position, g_player->camera.Front, 5.0f, world);
                if (res.hit) {
                    world.setBlock(res.blockPos.x, res.blockPos.y, res.blockPos.z, Block(Blocks::AIR));
                }
                leftMouseDown = true;
            }
        } else {
            leftMouseDown = false;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            if (!rightMouseDown) {
                RaycastResult res = Raycaster::raycast(g_player->camera.Position, g_player->camera.Front, 5.0f, world);
                if (res.hit) {
                    glm::ivec3 placePos = res.blockPos + res.faceNormal;
                    // Ensure we don't place a block inside the player
                    AABB playerAABB = g_player->getAABB();
                    AABB blockAABB(glm::vec3(placePos), glm::vec3(placePos) + glm::vec3(1.0f));
                    if (!playerAABB.intersects(blockAABB)) {
                        world.setBlock(placePos.x, placePos.y, placePos.z, Block(Blocks::STONE));
                    }
                }
                rightMouseDown = true;
            }
        } else {
            rightMouseDown = false;
        }
        
        g_player->handleInput(window, deltaTime);
        g_player->update(deltaTime, world);

        // Update world loading
        chunkProvider.update(g_player->position, 4);

        // Render
        glClearColor(0.529f, 0.808f, 0.922f, 1.0f); // Sky blue
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 3D Rendering
        worldShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(g_player->camera.Zoom), 800.0f / 600.0f, 0.1f, 1000.0f);
        glm::mat4 view = g_player->camera.GetViewMatrix();
        worldShader.setMat4("projection", projection);
        worldShader.setMat4("view", view);

        chunkProvider.render(worldShader);

        // Selection Box
        RaycastResult res = Raycaster::raycast(g_player->camera.Position, g_player->camera.Front, 5.0f, world);
        if (res.hit) {
            selectionBox.render(res.blockPos, view, projection);
        }

        // 2D Rendering
        glDisable(GL_DEPTH_TEST);
        hud.render(*g_player, currentFPS);
        glEnable(GL_DEPTH_TEST);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete g_player;
    glfwTerminate();
    return 0;
}
