#pragma once

#include <algorithm>
#include <glm/glm.hpp>
#include "render/Shader.hpp"
#include "render/Texture.hpp"

class GUIRenderer {
public:
    GUIRenderer();
    ~GUIRenderer();

    static inline glm::ivec2 normalizeViewport(int width, int height) {
        return glm::ivec2(std::max(1, width), std::max(1, height));
    }
    void setViewport(int width, int height);
    void renderRect(float x, float y, float w, float h, const glm::vec3& color, float alpha = 1.0f);
    void renderTexture(Texture* texture, float x, float y, float w, float h);
    void renderNineSlice(Texture* texture, float x, float y, float w, float h, int cornerSize);

private:
    Shader* m_solidShader;
    Shader* m_textureShader;
    unsigned int m_VAO, m_VBO;
    int m_viewWidth;
    int m_viewHeight;
    void setupRenderData();
};
