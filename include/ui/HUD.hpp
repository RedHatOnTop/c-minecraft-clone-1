#pragma once

#include "ui/FontRenderer.hpp"
#include "ui/GUIRenderer.hpp"
#include "physics/Player.hpp"
#include "world/ChestBlockEntity.hpp"
#include "world/FurnaceBlockEntity.hpp"

class HUD {
public:
    HUD(FontRenderer& font, GUIRenderer& gui);
    ~HUD();

    static inline glm::vec2 crosshairCenterForViewport(int viewWidth, int viewHeight) {
        glm::ivec2 viewport = GUIRenderer::normalizeViewport(viewWidth, viewHeight);
        return glm::vec2((float)viewport.x * 0.5f, (float)viewport.y * 0.5f);
    }
    static inline glm::vec2 hotbarOriginForViewport(int viewWidth, int viewHeight) {
        glm::ivec2 viewport = GUIRenderer::normalizeViewport(viewWidth, viewHeight);
        float barWidth = 182.0f * 2.0f;
        float barHeight = 22.0f * 2.0f;
        return glm::vec2(((float)viewport.x - barWidth) * 0.5f, (float)viewport.y - barHeight - 2.0f);
    }

    void render(const Player& player, int fps, bool inventoryOpen, const ChestBlockEntity* chest, const FurnaceBlockEntity* furnace, int viewWidth, int viewHeight);

private:
    FontRenderer& m_font;
    GUIRenderer& m_gui;

    void renderCrosshair(int viewWidth, int viewHeight);
    void renderHotbar(const Player& player, int viewWidth, int viewHeight);
    void renderDebug(const Player& player, int fps);
    void renderInventory(const Player& player, int viewWidth, int viewHeight);
    void renderChest(const Player& player, const ChestBlockEntity& chest, int viewWidth, int viewHeight);
    void renderFurnace(const Player& player, const FurnaceBlockEntity& furnace, int viewWidth, int viewHeight);
};
