#include "ui/GUIRenderer.hpp"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

GUIRenderer::GUIRenderer() {
    m_solidShader = new Shader("assets/minecraft/shaders/ui.vert", "assets/minecraft/shaders/ui_solid.frag");
    m_textureShader = new Shader("assets/minecraft/shaders/ui.vert", "assets/minecraft/shaders/ui.frag");
    m_viewWidth = 800;
    m_viewHeight = 600;
    setupRenderData();
}

GUIRenderer::~GUIRenderer() {
    delete m_solidShader;
    delete m_textureShader;
}

void GUIRenderer::setupRenderData() {
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

void GUIRenderer::setViewport(int width, int height) {
    glm::ivec2 size = normalizeViewport(width, height);
    m_viewWidth = size.x;
    m_viewHeight = size.y;
}

void GUIRenderer::renderRect(float x, float y, float w, float h, const glm::vec3& color, float alpha) {
    m_solidShader->use();
    m_solidShader->setVec4("rectColor", glm::vec4(color, alpha));
    
    glm::mat4 projection = glm::ortho(0.0f, (float)m_viewWidth, (float)m_viewHeight, 0.0f);
    m_solidShader->setMat4("projection", projection);

    float vertices[6][4] = {
        { x,     y + h,   0.0f, 1.0f },
        { x,     y,       0.0f, 0.0f },
        { x + w, y,       1.0f, 0.0f },

        { x,     y + h,   0.0f, 1.0f },
        { x + w, y,       1.0f, 0.0f },
        { x + w, y + h,   1.0f, 1.0f }
    };

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void GUIRenderer::renderTexture(Texture* texture, float x, float y, float w, float h) {
    m_textureShader->use();
    m_textureShader->setInt("text", 0);
    m_textureShader->setVec3("textColor", glm::vec3(1.0f));
    glm::mat4 projection = glm::ortho(0.0f, (float)m_viewWidth, (float)m_viewHeight, 0.0f);
    m_textureShader->setMat4("projection", projection);

    texture->bind(0);

    float vertices[6][4] = {
        { x,     y + h,   0.0f, 1.0f },
        { x,     y,       0.0f, 0.0f },
        { x + w, y,       1.0f, 0.0f },

        { x,     y + h,   0.0f, 1.0f },
        { x + w, y,       1.0f, 0.0f },
        { x + w, y + h,   1.0f, 1.0f }
    };

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
