#include "app/AppBootstrap.hpp"

#include <algorithm>
#include <filesystem>
#include <initializer_list>
#include <iostream>
#include <memory>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "entity/Mob.hpp"
#include "item/Item.hpp"
#include "util/SoundManager.hpp"
#include "world/BlockState.hpp"

namespace {
void registerBlocks() {
    BlockState stone;
    stone.name = "stone";
    stone.faceTextures = {{Face::UP, "stone"},       {Face::DOWN, "stone"}, {Face::NORTH, "stone"},
                          {Face::SOUTH, "stone"},   {Face::WEST, "stone"},  {Face::EAST, "stone"}};
    BlockStateManager::registerBlockState(Blocks::STONE, stone);

    BlockState dirt;
    dirt.name = "dirt";
    dirt.faceTextures = {{Face::UP, "dirt"},       {Face::DOWN, "dirt"}, {Face::NORTH, "dirt"},
                         {Face::SOUTH, "dirt"},   {Face::WEST, "dirt"},  {Face::EAST, "dirt"}};
    BlockStateManager::registerBlockState(Blocks::DIRT, dirt);

    BlockState grass;
    grass.name = "grass";
    grass.faceTextures = {{Face::UP, "grass_top"},       {Face::DOWN, "dirt"},       {Face::NORTH, "grass_side"},
                          {Face::SOUTH, "grass_side"},   {Face::WEST, "grass_side"}, {Face::EAST, "grass_side"}};
    BlockStateManager::registerBlockState(Blocks::GRASS, grass);

    BlockState log;
    log.name = "log";
    log.faceTextures = {{Face::UP, "log_top"},       {Face::DOWN, "log_top"}, {Face::NORTH, "log_side"},
                        {Face::SOUTH, "log_side"},   {Face::WEST, "log_side"}, {Face::EAST, "log_side"}};
    BlockStateManager::registerBlockState(Blocks::LOG, log);

    BlockState leaves;
    leaves.name = "leaves";
    leaves.faceTextures = {{Face::UP, "leaves"},       {Face::DOWN, "leaves"}, {Face::NORTH, "leaves"},
                           {Face::SOUTH, "leaves"},   {Face::WEST, "leaves"},  {Face::EAST, "leaves"}};
    BlockStateManager::registerBlockState(Blocks::LEAVES, leaves);

    BlockState chest;
    chest.name = "chest";
    chest.faceTextures = {{Face::UP, "chest_top"},       {Face::DOWN, "chest_bottom"}, {Face::NORTH, "chest_front"},
                          {Face::SOUTH, "chest_side"},   {Face::WEST, "chest_side"},    {Face::EAST, "chest_side"}};
    BlockStateManager::registerBlockState(Blocks::CHEST, chest);

    BlockState furnace;
    furnace.name = "furnace";
    furnace.faceTextures = {{Face::UP, "furnace_top"},       {Face::DOWN, "furnace_top"}, {Face::NORTH, "furnace_front"},
                            {Face::SOUTH, "furnace_side"},   {Face::WEST, "furnace_side"}, {Face::EAST, "furnace_side"}};
    BlockStateManager::registerBlockState(Blocks::FURNACE, furnace);
}

void registerItems() {
    ItemRegistry::registerItem(std::make_unique<Item>((ItemID)Blocks::STONE, "stone"));
    ItemRegistry::registerItem(std::make_unique<Item>((ItemID)Blocks::DIRT, "dirt"));
    ItemRegistry::registerItem(std::make_unique<Item>((ItemID)Blocks::GRASS, "grass"));
    ItemRegistry::registerItem(std::make_unique<Item>((ItemID)Blocks::COBBLESTONE, "cobblestone"));
    ItemRegistry::registerItem(std::make_unique<Item>((ItemID)Blocks::PLANKS, "planks"));
    ItemRegistry::registerItem(std::make_unique<Item>((ItemID)Blocks::LOG, "log"));
    ItemRegistry::registerItem(std::make_unique<Item>((ItemID)Blocks::LEAVES, "leaves"));
    ItemRegistry::registerItem(std::make_unique<Item>((ItemID)Blocks::CHEST, "chest"));
    ItemRegistry::registerItem(std::make_unique<Item>((ItemID)Blocks::FURNACE, "furnace"));
}

bool isGameplayInputEnabled(const GameContext& ctx) {
    return ctx.gameMode == GameMode::Playing && !ctx.inventoryOpen && !ctx.openedContainer.active;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    auto* ctx = static_cast<GameContext*>(glfwGetWindowUserPointer(window));
    if (!ctx) return;
    ctx->windowWidth = std::max(1, width);
    ctx->windowHeight = std::max(1, height);
    if (ctx->gui) ctx->gui->setViewport(ctx->windowWidth, ctx->windowHeight);
    if (ctx->font) ctx->font->setViewport(ctx->windowWidth, ctx->windowHeight);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* ctx = static_cast<GameContext*>(glfwGetWindowUserPointer(window));
    if (!ctx || !ctx->player) return;
    if (!isGameplayInputEnabled(*ctx)) return;

    if (ctx->firstMouse) {
        ctx->lastX = (float)xpos;
        ctx->lastY = (float)ypos;
        ctx->firstMouse = false;
    }

    float xoffset = (float)xpos - ctx->lastX;
    float yoffset = ctx->lastY - (float)ypos;
    ctx->lastX = (float)xpos;
    ctx->lastY = (float)ypos;
    ctx->player->camera.ProcessMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)xoffset;
    auto* ctx = static_cast<GameContext*>(glfwGetWindowUserPointer(window));
    if (!ctx || !ctx->player) return;
    if (!isGameplayInputEnabled(*ctx)) return;

    ctx->player->selectedSlot -= (int)yoffset;
    if (ctx->player->selectedSlot < 0) ctx->player->selectedSlot = 8;
    if (ctx->player->selectedSlot > 8) ctx->player->selectedSlot = 0;
}

std::string resolveTexturePath(std::initializer_list<const char*> filenames) {
    const std::filesystem::path roots[] = {"assets/minecraft/textures/blocks", "assets/minecraft/textures/block",
                                           "assets/sample-texturepack/assets/minecraft/textures/blocks",
                                           "assets/sample-texturepack/assets/minecraft/textures/block",
                                           "assets/simple-texturepack/assets/minecraft/textures/blocks",
                                           "assets/simple-texturepack/assets/minecraft/textures/block"};

    for (const auto& root : roots) {
        for (const char* filename : filenames) {
            std::filesystem::path p = root / filename;
            if (std::filesystem::exists(p)) return p.string();
        }
    }
    return (roots[0] / *filenames.begin()).string();
}
}  // namespace

namespace app {
void configureWindowCallbacks(GLFWwindow* window, GameContext& ctx) {
    glfwSetWindowUserPointer(window, &ctx);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool initializeGame(GameContext& ctx, GLFWwindow* window) {
    registerBlocks();
    registerItems();

    if (!SoundManager::init()) {
        std::cerr << "Warning: Sound system failed to initialize.\n";
    }
    SoundManager::loadSound("break", "assets/minecraft/sounds/dig/stone1.ogg");
    SoundManager::loadSound("place", "assets/minecraft/sounds/dig/stone1.ogg");
    SoundManager::loadSound("step_stone", "assets/minecraft/sounds/step/stone1.ogg");
    SoundManager::loadSound("step_grass", "assets/minecraft/sounds/step/grass1.ogg");
    SoundManager::loadSound("step_dirt", "assets/minecraft/sounds/step/dirt1.ogg");

    ctx.worldShader = std::make_unique<Shader>("assets/minecraft/shaders/basic.vert", "assets/minecraft/shaders/basic_opaque.frag");
    ctx.worldShaderTrans = std::make_unique<Shader>("assets/minecraft/shaders/basic.vert", "assets/minecraft/shaders/basic.frag");

    ctx.atlas.addTexture("stone", resolveTexturePath({"stone.png"}));
    ctx.atlas.addTexture("dirt", resolveTexturePath({"dirt.png"}));
    ctx.atlas.addTexture("grass_top", resolveTexturePath({"grass_top.png", "grass_block_top.png"}));
    ctx.atlas.addTexture("grass_side", resolveTexturePath({"grass_side.png", "grass_block_side.png"}));
    ctx.atlas.addTexture("log_top", resolveTexturePath({"log_top.png", "oak_log_top.png", "log_oak_top.png"}));
    ctx.atlas.addTexture("log_side", resolveTexturePath({"log_side.png", "oak_log.png", "log_oak.png"}));
    ctx.atlas.addTexture("leaves", resolveTexturePath({"leaves.png", "oak_leaves.png"}));
    ctx.atlas.addTexture("chest", resolveTexturePath({"chest.png"}));
    ctx.atlas.addTexture("chest_top", resolveTexturePath({"chest_top.png", "chest.png"}));
    ctx.atlas.addTexture("chest_bottom", resolveTexturePath({"chest_bottom.png", "chest_top.png", "chest.png"}));
    ctx.atlas.addTexture("chest_side", resolveTexturePath({"chest_side.png", "chest.png"}));
    ctx.atlas.addTexture("chest_front", resolveTexturePath({"chest_front.png", "chest.png"}));
    ctx.atlas.addTexture("furnace", resolveTexturePath({"furnace.png"}));
    ctx.atlas.addTexture("furnace_top", resolveTexturePath({"furnace_top.png", "furnace.png"}));
    ctx.atlas.addTexture("furnace_side", resolveTexturePath({"furnace_side.png", "furnace.png"}));
    ctx.atlas.addTexture("furnace_front", resolveTexturePath({"furnace_front.png", "furnace.png"}));
    ctx.atlas.generate();

    ctx.chunkProvider = std::make_unique<ChunkProvider>(ctx.world, ctx.generator, ctx.atlas, ctx.worldPath);
    ctx.player = std::make_unique<Player>(glm::vec3(0, 80, 0));
    ctx.entityManager.addEntity(std::make_shared<Mob>(glm::vec3(5, 80, 5), "zombie"));

    ctx.font = std::make_unique<FontRenderer>("assets/minecraft/font/ascii.png");
    ctx.gui = std::make_unique<GUIRenderer>();
    ctx.hud = std::make_unique<HUD>(*ctx.font, *ctx.gui);

    int fbW = 0;
    int fbH = 0;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    framebufferSizeCallback(window, fbW, fbH);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return true;
}
}  // namespace app
