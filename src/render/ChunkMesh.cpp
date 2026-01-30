#include "render/ChunkMesh.hpp"
#include "world/BlockState.hpp"
#include <iostream>

ChunkMesh::ChunkMesh() : m_VAO(0), m_VBO(0), m_vertexCount(0) {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
}

ChunkMesh::~ChunkMesh() {
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
}

void ChunkMesh::generate(const Chunk& chunk, const TextureAtlas& atlas) {
    std::vector<ChunkVertex> vertices;

    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        for (int x = 0; x < CHUNK_WIDTH; ++x) {
            for (int z = 0; z < CHUNK_WIDTH; ++z) {
                Block block = chunk.getBlock(x, y, z);
                if (block.id == Blocks::AIR) continue;

                const BlockState* state = BlockStateManager::getBlockState(block.id);
                if (!state) continue;

                glm::vec3 pos(x, y, z);
                
                // For now, we still use the simplified face adding
                // In a full implementation, we would load the JSON model and iterate its elements
                
                // Down
                if (y > 0 && chunk.getBlock(x, y - 1, z).id == Blocks::AIR)
                    addFace(vertices, pos, 0, atlas.getTextureInfo(state->faceTextures.at(Face::DOWN)));
                else if (y == 0)
                    addFace(vertices, pos, 0, atlas.getTextureInfo(state->faceTextures.at(Face::DOWN)));

                // Up
                if (y < CHUNK_HEIGHT - 1 && chunk.getBlock(x, y + 1, z).id == Blocks::AIR)
                    addFace(vertices, pos, 1, atlas.getTextureInfo(state->faceTextures.at(Face::UP)));
                else if (y == CHUNK_HEIGHT - 1)
                    addFace(vertices, pos, 1, atlas.getTextureInfo(state->faceTextures.at(Face::UP)));

                // North
                if (z > 0 && chunk.getBlock(x, y, z - 1).id == Blocks::AIR)
                    addFace(vertices, pos, 2, atlas.getTextureInfo(state->faceTextures.at(Face::NORTH)));
                else if (z == 0)
                    addFace(vertices, pos, 2, atlas.getTextureInfo(state->faceTextures.at(Face::NORTH)));

                // South
                if (z < CHUNK_WIDTH - 1 && chunk.getBlock(x, y, z + 1).id == Blocks::AIR)
                    addFace(vertices, pos, 3, atlas.getTextureInfo(state->faceTextures.at(Face::SOUTH)));
                else if (z == CHUNK_WIDTH - 1)
                    addFace(vertices, pos, 3, atlas.getTextureInfo(state->faceTextures.at(Face::SOUTH)));

                // West
                if (x > 0 && chunk.getBlock(x - 1, y, z).id == Blocks::AIR)
                    addFace(vertices, pos, 4, atlas.getTextureInfo(state->faceTextures.at(Face::WEST)));
                else if (x == 0)
                    addFace(vertices, pos, 4, atlas.getTextureInfo(state->faceTextures.at(Face::WEST)));

                // East
                if (x < CHUNK_WIDTH - 1 && chunk.getBlock(x + 1, y, z).id == Blocks::AIR)
                    addFace(vertices, pos, 5, atlas.getTextureInfo(state->faceTextures.at(Face::EAST)));
                else if (x == CHUNK_WIDTH - 1)
                    addFace(vertices, pos, 5, atlas.getTextureInfo(state->faceTextures.at(Face::EAST)));
            }
        }
    }

    m_vertexCount = vertices.size();
    if (m_vertexCount == 0) return;

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ChunkVertex), vertices.data(), GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, position));
    // TexCoord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, texCoord));
    // AO
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, ao));

    glBindVertexArray(0);
}

void ChunkMesh::render() {
    if (m_vertexCount == 0) return;
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    glBindVertexArray(0);
}

void ChunkMesh::addFace(std::vector<ChunkVertex>& vertices, glm::vec3 pos, int face, const TextureInfo& tex) {
    float x = pos.x, y = pos.y, z = pos.z;
    float u1 = tex.uvMin.x, v1 = tex.uvMin.y, u2 = tex.uvMax.x, v2 = tex.uvMax.y;

    // Simplified AO calculation based on neighbors (placeholder)
    float ao0 = 1.0f, ao1 = 1.0f, ao2 = 1.0f, ao3 = 1.0f;

    switch (face) {
        case 0: // Down
            vertices.push_back({{x, y, z}, {u1, v1}, ao0});
            vertices.push_back({{x + 1, y, z}, {u2, v1}, ao1});
            vertices.push_back({{x + 1, y, z + 1}, {u2, v2}, ao2});
            vertices.push_back({{x, y, z}, {u1, v1}, ao0});
            vertices.push_back({{x + 1, y, z + 1}, {u2, v2}, ao2});
            vertices.push_back({{x, y, z + 1}, {u1, v2}, ao3});
            break;
        case 1: // Up
            vertices.push_back({{x, y + 1, z}, {u1, v1}, ao0});
            vertices.push_back({{x, y + 1, z + 1}, {u1, v2}, ao1});
            vertices.push_back({{x + 1, y + 1, z + 1}, {u2, v2}, ao2});
            vertices.push_back({{x, y + 1, z}, {u1, v1}, ao0});
            vertices.push_back({{x + 1, y + 1, z + 1}, {u2, v2}, ao2});
            vertices.push_back({{x + 1, y + 1, z}, {u2, v1}, ao3});
            break;
        case 2: // North (Z-)
            vertices.push_back({{x, y, z}, {u2, v1}, ao0});
            vertices.push_back({{x, y + 1, z}, {u2, v2}, ao1});
            vertices.push_back({{x + 1, y + 1, z}, {u1, v2}, ao2});
            vertices.push_back({{x, y, z}, {u2, v1}, ao0});
            vertices.push_back({{x + 1, y + 1, z}, {u1, v2}, ao2});
            vertices.push_back({{x + 1, y, z}, {u1, v1}, ao3});
            break;
        case 3: // South (Z+)
            vertices.push_back({{x, y, z + 1}, {u1, v1}, ao0});
            vertices.push_back({{x + 1, y, z + 1}, {u2, v1}, ao1});
            vertices.push_back({{x + 1, y + 1, z + 1}, {u2, v2}, ao2});
            vertices.push_back({{x, y, z + 1}, {u1, v1}, ao0});
            vertices.push_back({{x + 1, y + 1, z + 1}, {u2, v2}, ao2});
            vertices.push_back({{x, y + 1, z + 1}, {u1, v2}, ao3});
            break;
        case 4: // West (X-)
            vertices.push_back({{x, y, z}, {u1, v1}, ao0});
            vertices.push_back({{x, y, z + 1}, {u2, v1}, ao1});
            vertices.push_back({{x, y + 1, z + 1}, {u2, v2}, ao2});
            vertices.push_back({{x, y, z}, {u1, v1}, ao0});
            vertices.push_back({{x, y + 1, z + 1}, {u2, v2}, ao2});
            vertices.push_back({{x, y + 1, z}, {u1, v2}, ao3});
            break;
        case 5: // East (X+)
            vertices.push_back({{x + 1, y, z}, {u2, v1}, ao0});
            vertices.push_back({{x + 1, y + 1, z}, {u2, v2}, ao1});
            vertices.push_back({{x + 1, y + 1, z + 1}, {u1, v2}, ao2});
            vertices.push_back({{x + 1, y, z}, {u2, v1}, ao0});
            vertices.push_back({{x + 1, y + 1, z + 1}, {u1, v2}, ao2});
            vertices.push_back({{x + 1, y, z + 1}, {u1, v1}, ao3});
            break;
    }
}
