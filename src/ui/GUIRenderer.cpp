#include "ui/GUIRenderer.hpp"
#include <glm/gtc/matrix_transform.hpp>

GUIRenderer::GUIRenderer() {
    // Reuse the UI shaders created in FontRenderer
    m_shader = new Shader("assets/minecraft/shaders/ui.vert", "assets/minecraft/shaders/ui.frag");
    setupRenderData();
}

GUIRenderer::~GUIRenderer() {
    delete m_shader;
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

void GUIRenderer::renderRect(float x, float y, float w, float h, const glm::vec3& color, float alpha) {
    m_shader->use();
    m_shader->setVec3("textColor", color); // Reusing variable name from UI shader
    
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f);
    m_shader->setMat4("projection", projection);

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
    texture->bind(0);
    renderRect(x, y, w, h, glm::vec3(1.0f));
}
