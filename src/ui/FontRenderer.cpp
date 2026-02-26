#include "ui/FontRenderer.hpp"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

FontRenderer::FontRenderer(const std::string& fontPath) {
    m_fontTexture = new Texture(fontPath.c_str(), false);
    m_viewWidth = 800;
    m_viewHeight = 600;

    m_shader = new Shader("assets/minecraft/shaders/ui.vert", "assets/minecraft/shaders/ui.frag");
    setupRenderData();
}

FontRenderer::~FontRenderer() {
    delete m_fontTexture;
    delete m_shader;
}

void FontRenderer::setupRenderData() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void FontRenderer::setViewport(int width, int height) {
    glm::ivec2 size = normalizeViewport(width, height);
    m_viewWidth = size.x;
    m_viewHeight = size.y;
}

void FontRenderer::renderString(const std::string& text, float x, float y, float scale, const glm::vec3& color, bool shadow) {
    m_shader->use();
    m_shader->setInt("text", 0);
    m_shader->setVec3("textColor", color);
    m_fontTexture->bind(0);
    
    // Standard UI projection (0,0 is top-left)
    glm::mat4 projection = glm::ortho(0.0f, (float)m_viewWidth, (float)m_viewHeight, 0.0f);
    m_shader->setMat4("projection", projection);

    glBindVertexArray(m_VAO);

    float cursorX = x;
    for (char c : text) {
        int charIdx = (int)c;
        float u = (charIdx % 16) / 16.0f;
        float v = (charIdx / 16) / 16.0f;
        float charSize = 1.0f / 16.0f;

        float xpos = cursorX;
        float ypos = y;
        float w = 8 * scale;
        float h = 8 * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   u,            v + charSize },
            { xpos,     ypos,       u,            v },
            { xpos + w, ypos,       u + charSize, v },

            { xpos,     ypos + h,   u,            v + charSize },
            { xpos + w, ypos,       u + charSize, v },
            { xpos + w, ypos + h,   u + charSize, v + charSize }
        };

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        cursorX += 8 * scale;
    }
    glBindVertexArray(0);
}

float FontRenderer::getStringWidth(const std::string& text, float scale) {
    return text.length() * 8 * scale;
}
