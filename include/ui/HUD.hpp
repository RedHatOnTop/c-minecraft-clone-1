#pragma once

#include "ui/FontRenderer.hpp"
#include "ui/GUIRenderer.hpp"
#include "physics/Player.hpp"

class HUD {
public:
    HUD(FontRenderer& font, GUIRenderer& gui);
    ~HUD();

    void render(const Player& player, int fps, bool inventoryOpen);

private:
    FontRenderer& m_font;
    GUIRenderer& m_gui;

    void renderCrosshair();
    void renderHotbar(const Player& player);
    void renderDebug(const Player& player, int fps);
    void renderInventory(const Player& player);
};
