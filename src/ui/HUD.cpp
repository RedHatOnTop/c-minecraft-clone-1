#include <iomanip>
#include <sstream>
#include <cmath>
#include "ui/HUD.hpp"

HUD::HUD(FontRenderer& font, GUIRenderer& gui) : m_font(font), m_gui(gui) {}

HUD::~HUD() {}

void HUD::render(const Player& player, int fps, bool inventoryOpen) {
    if (inventoryOpen) {
        renderInventory(player);
        return;
    }
    renderCrosshair();
    renderHotbar(player);
    renderDebug(player, fps);
}

void HUD::renderCrosshair() {
    float centerX = 800.0f / 2.0f;
    float centerY = 600.0f / 2.0f;
    float size = 10.0f;
    float thickness = 2.0f;

    m_gui.renderRect(centerX - size, centerY - thickness / 2.0f, size * 2, thickness, glm::vec3(1.0f));
    m_gui.renderRect(centerX - thickness / 2.0f, centerY - size, thickness, size * 2, glm::vec3(1.0f));
}

void HUD::renderHotbar(const Player& player) {
    float barWidth = 182 * 2.0f;
    float barHeight = 22 * 2.0f;
    float x = (800.0f - barWidth) / 2.0f;
    float y = 600.0f - barHeight - 2.0f;

    // Background (grayish)
    m_gui.renderRect(x, y, barWidth, barHeight, glm::vec3(0.5f), 0.5f);
    
    // Selection highlight
    float slotSize = 20 * 2.0f;
    float slotX = x + (player.selectedSlot * slotSize) + 2.0f;
    m_gui.renderRect(slotX - 2.0f, y - 2.0f, barHeight + 4.0f, barHeight + 4.0f, glm::vec3(1.0f), 0.5f);
}

void HUD::renderDebug(const Player& player, int fps) {
    std::stringstream ss;
    ss << "Minecraft 1.8.9 Clone (C++)";
    m_font.renderString(ss.str(), 5, 5, 2.0f, glm::vec3(1.0f));

    ss.str("");
    ss << fps << " fps";
    m_font.renderString(ss.str(), 5, 25, 2.0f, glm::vec3(1.0f));

    ss.str("");
    ss << "XYZ: " << std::fixed << std::setprecision(3) << player.position.x << " / " << player.position.y << " / " << player.position.z;
    m_font.renderString(ss.str(), 5, 45, 2.0f, glm::vec3(1.0f));

    ss.str("");
    int blockX = (int)std::floor(player.position.x);
    int blockY = (int)std::floor(player.position.y);
    int blockZ = (int)std::floor(player.position.z);
    ss << "Block: " << blockX << " " << blockY << " " << blockZ;
    m_font.renderString(ss.str(), 5, 65, 2.0f, glm::vec3(1.0f));

    ss.str("");
    int chunkX = (int)std::floor((float)blockX / 16.0f);
    int chunkZ = (int)std::floor((float)blockZ / 16.0f);
    ss << "Chunk: " << (blockX & 15) << " " << (blockY & 15) << " " << (blockZ & 15) << " in " << chunkX << " " << chunkZ;
    m_font.renderString(ss.str(), 5, 85, 2.0f, glm::vec3(1.0f));
}

void HUD::renderInventory(const Player& player) {
    m_gui.renderRect(0, 0, 800, 600, glm::vec3(0.0f), 0.5f);

    float invW = 352.0f;
    float invH = 332.0f;
    float startX = (800.0f - invW) / 2.0f;
    float startY = (600.0f - invH) / 2.0f;

    m_gui.renderRect(startX, startY, invW, invH, glm::vec3(0.2f), 0.85f);

    float slotSize = 32.0f;
    float pad = 6.0f;
    float gridX = startX + 16.0f;
    float gridY = startY + 64.0f;

    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 9; ++col) {
            int slot = row * 9 + col;
            float x = gridX + col * (slotSize + pad);
            float y = gridY + row * (slotSize + pad);
            m_gui.renderRect(x, y, slotSize, slotSize, glm::vec3(0.1f), 0.9f);

            ItemStack stack = player.inventory.getStack(slot);
            if (!stack.isEmpty()) {
                std::stringstream ss;
                ss << stack.count;
                m_font.renderString(ss.str(), x + 18.0f, y + 18.0f, 1.5f, glm::vec3(1.0f));
            }
        }
    }

    m_font.renderString("Inventory", startX + 16.0f, startY + 16.0f, 2.0f, glm::vec3(1.0f));
    m_font.renderString("Press C to craft: 1 log -> 4 planks", startX + 16.0f, startY + invH - 32.0f, 1.5f, glm::vec3(1.0f));
}
