#pragma once

#include <string>
#include <map>
#include <glm/glm.hpp>
#include "render/Texture.hpp"
#include "render/Shader.hpp"

class FontRenderer {
public:
    FontRenderer(const std::string& fontPath);
    ~FontRenderer();

    void renderString(const std::string& text, float x, float y, float scale, const glm::vec3& color, bool shadow = true);
    float getStringWidth(const std::string& text, float scale);

private:
    Texture* m_fontTexture;
    Shader* m_shader;
    unsigned int m_VAO, m_VBO;
    
    // Character width map (standard MC is 8x8 but some are narrower)
    std::map<char, int> m_charWidths;

    void setupRenderData();
};
