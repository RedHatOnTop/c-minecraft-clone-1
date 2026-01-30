#pragma once

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include "render/Shader.hpp"

class SelectionBox {
public:
    SelectionBox();
    ~SelectionBox();

    void render(const glm::ivec3& pos, const glm::mat4& view, const glm::mat4& projection);

private:
    unsigned int m_VAO, m_VBO, m_EBO;
    Shader* m_shader;

    void setupRenderData();
};
