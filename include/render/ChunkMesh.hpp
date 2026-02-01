#pragma once

#include <vector>
#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include "world/Chunk.hpp"
#include "render/TextureAtlas.hpp"

struct ChunkVertex {
    glm::vec3 position;
    glm::vec2 texCoord;
    float ao;
    float light;
};

class ChunkMesh {
public:
    ChunkMesh();
    ~ChunkMesh();

    void generate(const Chunk& chunk, const TextureAtlas& atlas);
    void render();
    void renderTransparent();

private:
    unsigned int m_VAO, m_VBO, m_transVAO, m_transVBO;
    int m_vertexCount, m_transVertexCount;

    void addFace(std::vector<ChunkVertex>& vertices, glm::vec3 pos, int face, const TextureInfo& tex, float light);
};
