#include <algorithm>
#include <iostream>
#include <memory>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "app/AppBootstrap.hpp"
#include "app/GameContext.hpp"
#include "app/InputController.hpp"
#include "entity/ItemEntity.hpp"
#include "entity/Mob.hpp"
#include "item/ItemStack.hpp"
#include "physics/AABB.hpp"
#include "physics/PhysicsConstants.hpp"
#include "physics/Raycast.hpp"
#include "util/SoundManager.hpp"

static void updateFrameTiming(GameContext& ctx) {
    const float currentFrame = (float)glfwGetTime();
    ctx.deltaTime = currentFrame - ctx.lastFrame;
    ctx.lastFrame = currentFrame;

    ctx.fpsTimer += ctx.deltaTime;
    ctx.frameCount++;
    if (ctx.fpsTimer >= 1.0f) {
        ctx.currentFPS = ctx.frameCount;
        ctx.frameCount = 0;
        ctx.fpsTimer = 0.0f;
    }
}

static void updateGame(GameContext& ctx, GLFWwindow* window) {
    app::ensureOpenedContainerValid(ctx, window);

    if (ctx.gameMode == GameMode::Playing) {
        // Accumulate real time into the tick accumulator
        ctx.tickAccumulator += ctx.deltaTime;

        // Cap accumulator to prevent physics explosion on long frame stalls
        const float maxAccum = PhysicsConstants::MAX_TICKS_PER_FRAME * PhysicsConstants::TICK_DURATION;
        if (ctx.tickAccumulator > maxAccum) {
            ctx.tickAccumulator = maxAccum;
        }

        // Process input once per frame (reads current key state)
        if (!app::isUIOpen(ctx)) {
            ctx.player->handleInput(window, PhysicsConstants::TICK_DURATION);
        }

        // Step physics in fixed 1/20s increments
        while (ctx.tickAccumulator >= PhysicsConstants::TICK_DURATION) {
            ctx.player->update(PhysicsConstants::TICK_DURATION, ctx.world);
            ctx.entityManager.update(PhysicsConstants::TICK_DURATION, ctx.world);

            ctx.tickAccumulator -= PhysicsConstants::TICK_DURATION;
        }

        // Item pickup (runs per frame, not per tick — lightweight check)
        AABB playerBox = ctx.player->getAABB();
        for (auto& e : ctx.entityManager.getEntitiesMutable()) {
            ItemEntity* item = dynamic_cast<ItemEntity*>(e.get());
            if (!item) continue;
            if (playerBox.intersects(item->getAABB())) {
                ItemStack stack = item->stack;
                ctx.player->addToInventory(stack);
                item->stack = stack;
                if (item->stack.isEmpty()) item->markRemoved();
            }
        }

        static float mobSpawnTimer = 0.0f;
        mobSpawnTimer += ctx.deltaTime;
        if (mobSpawnTimer >= 15.0f) {
            int mobCount = 0;
            for (const auto& e : ctx.entityManager.getEntities()) {
                if (dynamic_cast<Mob*>(e.get())) mobCount++;
            }
            if (mobCount < 5) {
                glm::vec3 spawn = ctx.player->position + glm::vec3(6.0f, 0.0f, 6.0f);
                ctx.entityManager.addEntity(std::make_shared<Mob>(spawn, "zombie"));
            }
            mobSpawnTimer = 0.0f;
        }

        static float liquidTimer = 0.0f;
        liquidTimer += ctx.deltaTime;
        if (liquidTimer >= 0.2f) {
            ctx.world.updateLiquids();
            liquidTimer = 0.0f;
        }
    }

    ctx.chunkProvider->update(ctx.player->position, 4);
}

static void renderOverlay(GameContext& ctx) {
    ctx.gui->setViewport(ctx.windowWidth, ctx.windowHeight);
    ctx.font->setViewport(ctx.windowWidth, ctx.windowHeight);

    if (ctx.gameMode == GameMode::MainMenu) {
        ctx.gui->renderRect(0, 0, (float)ctx.windowWidth, (float)ctx.windowHeight, glm::vec3(0.0f), 0.6f);
        float centerX = (float)ctx.windowWidth * 0.5f;
        float centerY = (float)ctx.windowHeight * 0.5f;
        ctx.font->renderString("Minecraft 1.8.9 Clone", centerX - 180.0f, centerY - 120.0f, 3.0f, glm::vec3(1.0f));
        ctx.font->renderString("Press ENTER to Play", centerX - 130.0f, centerY - 40.0f, 2.0f, glm::vec3(1.0f));
        ctx.font->renderString("Press ESC to Quit", centerX - 120.0f, centerY - 10.0f, 2.0f, glm::vec3(1.0f));
    } else if (ctx.gameMode == GameMode::Paused) {
        ctx.gui->renderRect(0, 0, (float)ctx.windowWidth, (float)ctx.windowHeight, glm::vec3(0.0f), 0.45f);
        float centerX = (float)ctx.windowWidth * 0.5f;
        float centerY = (float)ctx.windowHeight * 0.5f;
        ctx.font->renderString("Paused", centerX - 55.0f, centerY - 80.0f, 3.0f, glm::vec3(1.0f));
        ctx.font->renderString("Press ENTER to Resume", centerX - 150.0f, centerY - 20.0f, 2.0f, glm::vec3(1.0f));
        ctx.font->renderString("Press ESC for Menu/Quit", centerX - 170.0f, centerY + 10.0f, 2.0f, glm::vec3(1.0f));
    }
}

static void renderFrame(GameContext& ctx) {
    glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ctx.worldShader->use();
    ctx.worldShader->setInt("texture_atlas", 0);
    float aspect = (float)ctx.windowWidth / (float)std::max(1, ctx.windowHeight);
    glm::mat4 projection = glm::perspective(glm::radians(ctx.player->camera.Zoom), aspect, 0.1f, 1000.0f);
    glm::mat4 view = ctx.player->camera.GetViewMatrix();
    ctx.worldShader->setMat4("projection", projection);
    ctx.worldShader->setMat4("view", view);
    ctx.worldShader->setVec3("fogColor", glm::vec3(0.529f, 0.808f, 0.922f));
    ctx.worldShader->setFloat("fogNear", 30.0f);
    ctx.worldShader->setFloat("fogFar", 80.0f);

    ctx.frustum.update(projection * view);
    ctx.chunkProvider->render(*ctx.worldShader, ctx.frustum);

    ctx.worldShaderTrans->use();
    ctx.worldShaderTrans->setInt("texture_atlas", 0);
    ctx.worldShaderTrans->setMat4("projection", projection);
    ctx.worldShaderTrans->setMat4("view", view);
    ctx.worldShaderTrans->setVec3("fogColor", glm::vec3(0.529f, 0.808f, 0.922f));
    ctx.worldShaderTrans->setFloat("fogNear", 30.0f);
    ctx.worldShaderTrans->setFloat("fogFar", 80.0f);
    ctx.chunkProvider->renderTransparent(*ctx.worldShaderTrans, ctx.frustum, ctx.player->camera.Position);

    RaycastResult res = Raycaster::raycast(ctx.player->camera.Position, ctx.player->camera.Front, 5.0f, ctx.world);
    if (res.hit) {
        ctx.selectionBox.render(res.blockPos, view, projection);
    }

    glDisable(GL_DEPTH_TEST);
    if (ctx.gameMode == GameMode::Playing) {
        ctx.hud->render(
            *ctx.player, ctx.currentFPS, ctx.inventoryOpen, app::getOpenChest(ctx), app::getOpenFurnace(ctx), ctx.windowWidth,
            ctx.windowHeight);
    }
    if (ctx.gameMode != GameMode::Playing) {
        renderOverlay(ctx);
    }
    glEnable(GL_DEPTH_TEST);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Minecraft 1.8.9 Clone", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    GameContext ctx;
    app::configureWindowCallbacks(window, ctx);

    if (gl3wInit()) {
        std::cerr << "Failed to initialize gl3w\n";
        glfwTerminate();
        return -1;
    }

    if (!app::initializeGame(ctx, window)) {
        glfwTerminate();
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        updateFrameTiming(ctx);
        app::processInput(ctx, window);
        app::handleBlockInteraction(ctx, window);
        app::handleContainerAndCraftShortcuts(ctx, window);
        updateGame(ctx, window);
        renderFrame(ctx);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    SoundManager::cleanup();
    glfwTerminate();
    return 0;
}
