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
};

class ChunkMesh {
public:
    ChunkMesh();
    ~ChunkMesh();

    void generate(const Chunk& chunk, const TextureAtlas& atlas);
    void render();

private:
    unsigned int m_VAO, m_VBO;
    int m_vertexCount;

    void addFace(std::vector<ChunkVertex>& vertices, glm::vec3 pos, int face, const TextureInfo& tex);
};
