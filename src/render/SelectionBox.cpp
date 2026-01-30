#include "render/SelectionBox.hpp"
#include <glm/gtc/matrix_transform.hpp>

SelectionBox::SelectionBox() {
    m_shader = new Shader("assets/minecraft/shaders/line.vert", "assets/minecraft/shaders/line.frag");
    setupRenderData();
}

SelectionBox::~SelectionBox() {
    delete m_shader;
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void SelectionBox::setupRenderData() {
    float vertices[] = {
        -0.002f, -0.002f, -0.002f,
         1.002f, -0.002f, -0.002f,
         1.002f,  1.002f, -0.002f,
        -0.002f,  1.002f, -0.002f,
        -0.002f, -0.002f,  1.002f,
         1.002f, -0.002f,  1.002f,
         1.002f,  1.002f,  1.002f,
        -0.002f,  1.002f,  1.002f
    };

    unsigned int indices[] = {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void SelectionBox::render(const glm::ivec3& pos, const glm::mat4& view, const glm::mat4& projection) {
    m_shader->use();
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos));
    m_shader->setMat4("model", model);
    m_shader->setMat4("view", view);
    m_shader->setMat4("projection", projection);

    glLineWidth(2.0f);
    glBindVertexArray(m_VAO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
