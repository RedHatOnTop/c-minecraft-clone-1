#include <iostream>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "render/Shader.hpp"
#include "render/TextureAtlas.hpp"
#include "world/World.hpp"
#include "world/WorldGenerator.hpp"
#include "world/ChunkProvider.hpp"
#include "world/WorldPersistence.hpp"
#include "entity/EntityManager.hpp"
#include "entity/Mob.hpp"
#include "entity/ItemEntity.hpp"
#include "physics/Player.hpp"
#include "physics/Raycast.hpp"
#include "render/SelectionBox.hpp"
#include "ui/FontRenderer.hpp"
#include "ui/GUIRenderer.hpp"
#include "ui/HUD.hpp"
#include "util/SoundManager.hpp"

#include "world/BlockState.hpp"
#include "item/Item.hpp"

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

    BlockState log;
    log.name = "log";
    log.faceTextures = {
        {Face::UP, "log_top"}, {Face::DOWN, "log_top"},
        {Face::NORTH, "log_side"}, {Face::SOUTH, "log_side"},
        {Face::WEST, "log_side"}, {Face::EAST, "log_side"}
    };
    BlockStateManager::registerBlockState(Blocks::LOG, log);

    BlockState leaves;
    leaves.name = "leaves";
    leaves.faceTextures = {
        {Face::UP, "leaves"}, {Face::DOWN, "leaves"},
        {Face::NORTH, "leaves"}, {Face::SOUTH, "leaves"},
        {Face::WEST, "leaves"}, {Face::EAST, "leaves"}
    };
    BlockStateManager::registerBlockState(Blocks::LEAVES, leaves);
}

void registerItems() {
    ItemRegistry::registerItem(std::make_unique<Item>((ItemID)Blocks::STONE, "stone"));
    ItemRegistry::registerItem(std::make_unique<Item>((ItemID)Blocks::DIRT, "dirt"));
    ItemRegistry::registerItem(std::make_unique<Item>((ItemID)Blocks::GRASS, "grass"));
    ItemRegistry::registerItem(std::make_unique<Item>((ItemID)Blocks::PLANKS, "planks"));
    ItemRegistry::registerItem(std::make_unique<Item>((ItemID)Blocks::LOG, "log"));
    ItemRegistry::registerItem(std::make_unique<Item>((ItemID)Blocks::LEAVES, "leaves"));
}

// Global state for simplicity in main.cpp
Player* g_player;
bool g_firstMouse = true;
float g_lastX = 400, g_lastY = 300;
bool g_inventoryOpen = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (g_inventoryOpen) return;
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
    if (g_inventoryOpen) return;
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
    registerItems();
    if (!SoundManager::init()) {
        std::cerr << "Warning: Sound system failed to initialize." << std::endl;
    }
    SoundManager::loadSound("break", "assets/minecraft/sounds/dig/stone1.ogg");
    SoundManager::loadSound("place", "assets/minecraft/sounds/dig/stone1.ogg");
    SoundManager::loadSound("step_stone", "assets/minecraft/sounds/step/stone1.ogg");
    SoundManager::loadSound("step_grass", "assets/minecraft/sounds/step/grass1.ogg");
    SoundManager::loadSound("step_dirt", "assets/minecraft/sounds/step/dirt1.ogg");

    Shader worldShader("assets/minecraft/shaders/basic.vert", "assets/minecraft/shaders/basic.frag");
    TextureAtlas atlas(16);
    atlas.addTexture("stone", "assets/minecraft/textures/blocks/stone.png");
    atlas.addTexture("dirt", "assets/minecraft/textures/blocks/dirt.png");
    atlas.addTexture("grass_top", "assets/minecraft/textures/blocks/grass_top.png");
    atlas.addTexture("grass_side", "assets/minecraft/textures/blocks/grass_side.png");
    atlas.addTexture("log_top", "assets/minecraft/textures/blocks/log_top.png");
    atlas.addTexture("log_side", "assets/minecraft/textures/blocks/log_side.png");
    atlas.addTexture("leaves", "assets/minecraft/textures/blocks/leaves.png");
    atlas.generate();

    World world;
    WorldGenerator generator(12345);
    ChunkProvider chunkProvider(world, generator, atlas);
    g_player = new Player(glm::vec3(0, 80, 0));

    EntityManager entityManager;
    // Spawn a test mob
    entityManager.addEntity(std::make_shared<Mob>(glm::vec3(5, 80, 5), "zombie"));

    SelectionBox selectionBox;
    Frustum frustum;

    // UI initialization
    // For now we use placeholders for the font texture
    FontRenderer font("assets/minecraft/font/ascii.png");
    GUIRenderer gui;
    HUD hud(font, gui);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

        static bool eDown = false;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            if (!eDown) {
                g_inventoryOpen = !g_inventoryOpen;
                if (g_inventoryOpen) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                } else {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    g_firstMouse = true;
                }
                eDown = true;
            }
        } else {
            eDown = false;
        }

        static bool f5Down = false;
        if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
            if (!f5Down) {
                std::string worldPath = "saves/world1";
                std::filesystem::create_directories(worldPath);
                for (auto const& [pos, chunk] : world.getChunks()) {
                    WorldPersistence::saveChunk(worldPath, *chunk);
                }
                WorldPersistence::savePlayerData(worldPath, *g_player);
                WorldPersistence::saveEntities(worldPath, entityManager);
                f5Down = true;
            }
        } else {
            f5Down = false;
        }

        static bool f9Down = false;
        if (glfwGetKey(window, GLFW_KEY_F9) == GLFW_PRESS) {
            if (!f9Down) {
                std::string worldPath = "saves/world1";
                WorldPersistence::loadPlayerData(worldPath, *g_player);
                entityManager.clear();
                WorldPersistence::loadEntities(worldPath, entityManager);
                f9Down = true;
            }
        } else {
            f9Down = false;
        }
        
        // Block Interaction
        static bool leftMouseDown = false;
        static bool rightMouseDown = false;
        auto getDropItem = [](BlockID id) -> ItemID {
            switch (id) {
                case Blocks::GRASS: return (ItemID)Blocks::DIRT;
                case Blocks::STONE: return (ItemID)Blocks::STONE;
                case Blocks::DIRT: return (ItemID)Blocks::DIRT;
                case Blocks::LOG: return (ItemID)Blocks::LOG;
                case Blocks::LEAVES: return (ItemID)Blocks::LEAVES;
                default: return 0;
            }
        };

        if (!g_inventoryOpen && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (!leftMouseDown) {
                auto rayIntersectsAABB = [](const glm::vec3& ro, const glm::vec3& rd, const AABB& box, float& tOut) -> bool {
                    float tmin = 0.0f;
                    float tmax = 5.0f;
                    for (int axis = 0; axis < 3; ++axis) {
                        float origin = ro[axis];
                        float dir = rd[axis];
                        float minB = box.min[axis];
                        float maxB = box.max[axis];
                        if (std::abs(dir) < 1e-6f) {
                            if (origin < minB || origin > maxB) return false;
                        } else {
                            float ood = 1.0f / dir;
                            float t1 = (minB - origin) * ood;
                            float t2 = (maxB - origin) * ood;
                            if (t1 > t2) std::swap(t1, t2);
                            tmin = std::max(tmin, t1);
                            tmax = std::min(tmax, t2);
                            if (tmin > tmax) return false;
                        }
                    }
                    tOut = tmin;
                    return true;
                };

                float closestT = 9999.0f;
                std::shared_ptr<Entity> closestEntity;
                for (auto& e : entityManager.getEntities()) {
                    if (e->removed) continue;
                    Mob* mob = dynamic_cast<Mob*>(e.get());
                    if (!mob) continue;
                    float tHit = 0.0f;
                    if (rayIntersectsAABB(g_player->camera.Position, glm::normalize(g_player->camera.Front), mob->getAABB(), tHit)) {
                        if (tHit < closestT) {
                            closestT = tHit;
                            closestEntity = e;
                        }
                    }
                }

                if (closestEntity) {
                    closestEntity->health -= 4.0f;
                    if (closestEntity->health <= 0.0f) {
                        closestEntity->markRemoved();
                    }
                } else {
                    RaycastResult res = Raycaster::raycast(g_player->camera.Position, g_player->camera.Front, 5.0f, world);
                    if (res.hit) {
                        Block hitBlock = world.getBlock(res.blockPos.x, res.blockPos.y, res.blockPos.z);
                        ItemID drop = getDropItem(hitBlock.id);
                        if (drop != 0) {
                            entityManager.addEntity(std::make_shared<ItemEntity>(
                                glm::vec3(res.blockPos) + glm::vec3(0.5f, 0.5f, 0.5f),
                                ItemStack(drop, 1, 0)));
                        }
                        world.setBlock(res.blockPos.x, res.blockPos.y, res.blockPos.z, Block(Blocks::AIR));
                        SoundManager::playSound("break");
                    }
                }
                leftMouseDown = true;
            }
        } else {
            leftMouseDown = false;
        }

        if (!g_inventoryOpen && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            if (!rightMouseDown) {
                RaycastResult res = Raycaster::raycast(g_player->camera.Position, g_player->camera.Front, 5.0f, world);
                if (res.hit) {
                    glm::ivec3 placePos = res.blockPos + res.faceNormal;
                    // Ensure we don't place a block inside the player
                    AABB playerAABB = g_player->getAABB();
                    AABB blockAABB(glm::vec3(placePos), glm::vec3(placePos) + glm::vec3(1.0f));
                    if (!playerAABB.intersects(blockAABB)) {
                        world.setBlock(placePos.x, placePos.y, placePos.z, Block(Blocks::STONE));
                        SoundManager::playSound("place");
                    }
                }
                rightMouseDown = true;
            }
        } else {
            rightMouseDown = false;
        }

        static bool cDown = false;
        if (g_inventoryOpen && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            if (!cDown) {
                ItemStack in = g_player->inventory.getStack(0);
                if (!in.isEmpty() && in.itemID == (ItemID)Blocks::LOG && in.count >= 1) {
                    g_player->inventory.removeStack(0, 1);
                    ItemStack out((ItemID)Blocks::PLANKS, 4, 0);
                    g_player->addToInventory(out);
                }
                cDown = true;
            }
        } else {
            cDown = false;
        }

        if (!g_inventoryOpen) {
            g_player->handleInput(window, deltaTime);
        }
        g_player->update(deltaTime, world);
        entityManager.update(deltaTime, world);

        AABB playerBox = g_player->getAABB();
        for (auto& e : entityManager.getEntitiesMutable()) {
            ItemEntity* item = dynamic_cast<ItemEntity*>(e.get());
            if (!item) continue;
            if (playerBox.intersects(item->getAABB())) {
                ItemStack stack = item->stack;
                g_player->addToInventory(stack);
                item->stack = stack;
                if (item->stack.isEmpty()) {
                    item->markRemoved();
                }
            }
        }

        static float mobSpawnTimer = 0.0f;
        mobSpawnTimer += deltaTime;
        if (mobSpawnTimer >= 15.0f) {
            int mobCount = 0;
            for (auto& e : entityManager.getEntities()) {
                if (dynamic_cast<Mob*>(e.get())) mobCount++;
            }
            if (mobCount < 5) {
                glm::vec3 spawn = g_player->position + glm::vec3(6.0f, 0.0f, 6.0f);
                entityManager.addEntity(std::make_shared<Mob>(spawn, "zombie"));
            }
            mobSpawnTimer = 0.0f;
        }

        // Update world loading
        chunkProvider.update(g_player->position, 4);

        // Update Liquids (every 0.2s)
        static float liquidTimer = 0;
        liquidTimer += deltaTime;
        if (liquidTimer >= 0.2f) {
            world.updateLiquids();
            liquidTimer = 0;
        }

        // Render
        glClearColor(0.529f, 0.808f, 0.922f, 1.0f); // Sky blue
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 3D Rendering
        worldShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(g_player->camera.Zoom), 800.0f / 600.0f, 0.1f, 1000.0f);
        glm::mat4 view = g_player->camera.GetViewMatrix();
        worldShader.setMat4("projection", projection);
        worldShader.setMat4("view", view);

        // Fog uniforms
        worldShader.setVec3("fogColor", glm::vec3(0.529f, 0.808f, 0.922f));
        worldShader.setFloat("fogNear", 30.0f);
        worldShader.setFloat("fogFar", 80.0f);

        frustum.update(projection * view);

        chunkProvider.render(worldShader, frustum);

        // Transparent Rendering
        chunkProvider.renderTransparent(worldShader, frustum);

        // Selection Box
        RaycastResult res = Raycaster::raycast(g_player->camera.Position, g_player->camera.Front, 5.0f, world);
        if (res.hit) {
            selectionBox.render(res.blockPos, view, projection);
        }

        // 2D Rendering
        glDisable(GL_DEPTH_TEST);
        hud.render(*g_player, currentFPS, g_inventoryOpen);
        glEnable(GL_DEPTH_TEST);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete g_player;
    glfwTerminate();
    return 0;
}
