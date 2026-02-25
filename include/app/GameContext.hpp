#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "entity/EntityManager.hpp"
#include "physics/Player.hpp"
#include "render/Frustum.hpp"
#include "render/SelectionBox.hpp"
#include "render/Shader.hpp"
#include "render/TextureAtlas.hpp"
#include "ui/FontRenderer.hpp"
#include "ui/GUIRenderer.hpp"
#include "ui/HUD.hpp"
#include "world/Block.hpp"
#include "world/ChunkProvider.hpp"
#include "world/World.hpp"
#include "world/WorldGenerator.hpp"

enum class GameMode {
    MainMenu,
    Playing,
    Paused
};

struct OpenedContainerState {
    bool active = false;
    glm::ivec3 blockPos{0};
    BlockID blockId = Blocks::AIR;
};

struct GameContext {
    std::unique_ptr<Player> player;
    bool firstMouse = true;
    float lastX = 400.0f;
    float lastY = 300.0f;
    bool inventoryOpen = false;
    int windowWidth = 800;
    int windowHeight = 600;
    GameMode gameMode = GameMode::MainMenu;
    OpenedContainerState openedContainer;

    std::string worldPath = "saves/world1";
    World world;
    WorldGenerator generator{12345};
    TextureAtlas atlas{16};
    std::unique_ptr<ChunkProvider> chunkProvider;
    EntityManager entityManager;
    SelectionBox selectionBox;
    Frustum frustum;
    std::unique_ptr<Shader> worldShader;
    std::unique_ptr<Shader> worldShaderTrans;
    std::unique_ptr<FontRenderer> font;
    std::unique_ptr<GUIRenderer> gui;
    std::unique_ptr<HUD> hud;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    int frameCount = 0;
    float fpsTimer = 0.0f;
    int currentFPS = 0;

    bool escDown = false;
    bool enterDown = false;
    bool eDown = false;
    bool f5Down = false;
    bool f9Down = false;
    bool f6Down = false;
    bool leftMouseDown = false;
    bool rightMouseDown = false;
    bool cDown = false;
    bool tDown = false;
    bool yDown = false;
    bool uDown = false;
};
