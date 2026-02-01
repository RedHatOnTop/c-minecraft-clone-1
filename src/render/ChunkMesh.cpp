#include "render/ChunkMesh.hpp"
#include "world/BlockState.hpp"
#include <iostream>
#include <algorithm>

ChunkMesh::ChunkMesh() : m_VAO(0), m_VBO(0), m_transVAO(0), m_transVBO(0), m_vertexCount(0), m_transVertexCount(0) {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenVertexArrays(1, &m_transVAO);
    glGenBuffers(1, &m_transVBO);
}

ChunkMesh::~ChunkMesh() {
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_transVBO) glDeleteBuffers(1, &m_transVBO);
    if (m_transVAO) glDeleteVertexArrays(1, &m_transVAO);
}

void ChunkMesh::generate(const Chunk& chunk, const TextureAtlas& atlas) {
    std::vector<ChunkVertex> vertices;
    std::vector<ChunkVertex> transVertices;

    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        for (int x = 0; x < CHUNK_WIDTH; ++x) {
            for (int z = 0; z < CHUNK_WIDTH; ++z) {
                Block block = chunk.getBlock(x, y, z);
                if (block.id == Blocks::AIR) continue;

                const BlockState* state = BlockStateManager::getBlockState(block.id);
                if (!state) continue;

                glm::vec3 pos(x, y, z);
                bool isTrans = (block.id == Blocks::WATER || block.id == Blocks::LEAVES);
                auto& target = isTrans ? transVertices : vertices;
                float light = (float)std::max(getSkyLight(block), getBlockLight(block)) / 15.0f;
                
                // Down
                if (y > 0 && chunk.getBlock(x, y - 1, z).id == Blocks::AIR)
                    addFace(target, pos, 0, atlas.getTextureInfo(state->faceTextures.at(Face::DOWN)), light);
                else if (y == 0)
                    addFace(target, pos, 0, atlas.getTextureInfo(state->faceTextures.at(Face::DOWN)), light);

                // Up
                if (y < CHUNK_HEIGHT - 1 && chunk.getBlock(x, y + 1, z).id == Blocks::AIR)
                    addFace(target, pos, 1, atlas.getTextureInfo(state->faceTextures.at(Face::UP)), light);
                else if (y == CHUNK_HEIGHT - 1)
                    addFace(target, pos, 1, atlas.getTextureInfo(state->faceTextures.at(Face::UP)), light);

                // North
                if (z > 0 && chunk.getBlock(x, y, z - 1).id == Blocks::AIR)
                    addFace(target, pos, 2, atlas.getTextureInfo(state->faceTextures.at(Face::NORTH)), light);
                else if (z == 0)
                    addFace(target, pos, 2, atlas.getTextureInfo(state->faceTextures.at(Face::NORTH)), light);

                // South
                if (z < CHUNK_WIDTH - 1 && chunk.getBlock(x, y, z + 1).id == Blocks::AIR)
                    addFace(target, pos, 3, atlas.getTextureInfo(state->faceTextures.at(Face::SOUTH)), light);
                else if (z == CHUNK_WIDTH - 1)
                    addFace(target, pos, 3, atlas.getTextureInfo(state->faceTextures.at(Face::SOUTH)), light);

                // West
                if (x > 0 && chunk.getBlock(x - 1, y, z).id == Blocks::AIR)
                    addFace(target, pos, 4, atlas.getTextureInfo(state->faceTextures.at(Face::WEST)), light);
                else if (x == 0)
                    addFace(target, pos, 4, atlas.getTextureInfo(state->faceTextures.at(Face::WEST)), light);

                // East
                if (x < CHUNK_WIDTH - 1 && chunk.getBlock(x + 1, y, z).id == Blocks::AIR)
                    addFace(target, pos, 5, atlas.getTextureInfo(state->faceTextures.at(Face::EAST)), light);
                else if (x == CHUNK_WIDTH - 1)
                    addFace(target, pos, 5, atlas.getTextureInfo(state->faceTextures.at(Face::EAST)), light);
            }
        }
    }

    m_vertexCount = vertices.size();
    if (m_vertexCount > 0) {
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ChunkVertex), vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, texCoord));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, ao));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, light));
    }

    m_transVertexCount = transVertices.size();
    if (m_transVertexCount > 0) {
        glBindVertexArray(m_transVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_transVBO);
        glBufferData(GL_ARRAY_BUFFER, transVertices.size() * sizeof(ChunkVertex), transVertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, texCoord));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, ao));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, light));
    }
    glBindVertexArray(0);
}

void ChunkMesh::render() {
    if (m_vertexCount == 0) return;
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    glBindVertexArray(0);
}

void ChunkMesh::renderTransparent() {
    if (m_transVertexCount == 0) return;
    glBindVertexArray(m_transVAO);
    glDrawArrays(GL_TRIANGLES, 0, m_transVertexCount);
    glBindVertexArray(0);
}

void ChunkMesh::addFace(std::vector<ChunkVertex>& vertices, glm::vec3 pos, int face, const TextureInfo& tex, float light) {
    float x = pos.x, y = pos.y, z = pos.z;
    float u1 = tex.uvMin.x, v1 = tex.uvMin.y, u2 = tex.uvMax.x, v2 = tex.uvMax.y;

    float ao0 = 1.0f, ao1 = 1.0f, ao2 = 1.0f, ao3 = 1.0f;

    switch (face) {
        case 0: // Down
            vertices.push_back({{x, y, z}, {u1, v1}, ao0, light});
            vertices.push_back({{x + 1, y, z}, {u2, v1}, ao1, light});
            vertices.push_back({{x + 1, y, z + 1}, {u2, v2}, ao2, light});
            vertices.push_back({{x, y, z}, {u1, v1}, ao0, light});
            vertices.push_back({{x + 1, y, z + 1}, {u2, v2}, ao2, light});
            vertices.push_back({{x, y, z + 1}, {u1, v2}, ao3, light});
            break;
        case 1: // Up
            vertices.push_back({{x, y + 1, z}, {u1, v1}, ao0, light});
            vertices.push_back({{x, y + 1, z + 1}, {u1, v2}, ao1, light});
            vertices.push_back({{x + 1, y + 1, z + 1}, {u2, v2}, ao2, light});
            vertices.push_back({{x, y + 1, z}, {u1, v1}, ao0, light});
            vertices.push_back({{x + 1, y + 1, z + 1}, {u2, v2}, ao2, light});
            vertices.push_back({{x + 1, y + 1, z}, {u2, v1}, ao3, light});
            break;
        case 2: // North (Z-)
            vertices.push_back({{x, y, z}, {u2, v1}, ao0, light});
            vertices.push_back({{x, y + 1, z}, {u2, v2}, ao1, light});
            vertices.push_back({{x + 1, y + 1, z}, {u1, v2}, ao2, light});
            vertices.push_back({{x, y, z}, {u2, v1}, ao0, light});
            vertices.push_back({{x + 1, y + 1, z}, {u1, v2}, ao2, light});
            vertices.push_back({{x + 1, y, z}, {u1, v1}, ao3, light});
            break;
        case 3: // South (Z+)
            vertices.push_back({{x, y, z + 1}, {u1, v1}, ao0, light});
            vertices.push_back({{x + 1, y, z + 1}, {u2, v1}, ao1, light});
            vertices.push_back({{x + 1, y + 1, z + 1}, {u2, v2}, ao2, light});
            vertices.push_back({{x, y, z + 1}, {u1, v1}, ao0, light});
            vertices.push_back({{x + 1, y + 1, z + 1}, {u2, v2}, ao2, light});
            vertices.push_back({{x, y + 1, z + 1}, {u1, v2}, ao3, light});
            break;
        case 4: // West (X-)
            vertices.push_back({{x, y, z}, {u1, v1}, ao0, light});
            vertices.push_back({{x, y, z + 1}, {u2, v1}, ao1, light});
            vertices.push_back({{x, y + 1, z + 1}, {u2, v2}, ao2, light});
            vertices.push_back({{x, y, z}, {u1, v1}, ao0, light});
            vertices.push_back({{x, y + 1, z + 1}, {u2, v2}, ao2, light});
            vertices.push_back({{x, y + 1, z}, {u1, v2}, ao3, light});
            break;
        case 5: // East (X+)
            vertices.push_back({{x + 1, y, z}, {u2, v1}, ao0, light});
            vertices.push_back({{x + 1, y + 1, z}, {u2, v2}, ao1, light});
            vertices.push_back({{x + 1, y + 1, z + 1}, {u1, v2}, ao2, light});
            vertices.push_back({{x + 1, y, z}, {u2, v1}, ao0, light});
            vertices.push_back({{x + 1, y + 1, z + 1}, {u1, v2}, ao2, light});
            vertices.push_back({{x + 1, y, z + 1}, {u1, v1}, ao3, light});
            break;
    }
}
