#pragma once

#include <glm/glm.hpp>
#include "render/Shader.hpp"
#include "render/Texture.hpp"

class GUIRenderer {
public:
    GUIRenderer();
    ~GUIRenderer();

    void renderRect(float x, float y, float w, float h, const glm::vec3& color, float alpha = 1.0f);
    void renderTexture(Texture* texture, float x, float y, float w, float h);
    void renderNineSlice(Texture* texture, float x, float y, float w, float h, int cornerSize);

private:
    Shader* m_shader;
    unsigned int m_VAO, m_VBO;
    void setupRenderData();
};
