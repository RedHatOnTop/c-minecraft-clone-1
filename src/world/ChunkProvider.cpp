#include "world/ChunkProvider.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

ChunkProvider::ChunkProvider(World& world, WorldGenerator& generator, TextureAtlas& atlas)
    : m_world(world), m_generator(generator), m_atlas(atlas) {}

ChunkProvider::~ChunkProvider() {}

void ChunkProvider::update(glm::vec3 playerPos, int renderDistance) {
    int px = std::floor(playerPos.x / CHUNK_WIDTH);
    int pz = std::floor(playerPos.z / CHUNK_WIDTH);

    loadAround(px, pz, renderDistance);
    unloadFar(px, pz, renderDistance);
    processMeshQueue(1);
}

void ChunkProvider::render(Shader& shader, const Frustum& frustum) {
    m_atlas.bind(0);
    for (auto const& [pos, mesh] : m_meshes) {
        glm::vec3 min(pos.x * CHUNK_WIDTH, 0, pos.y * CHUNK_WIDTH);
        glm::vec3 max((pos.x + 1) * CHUNK_WIDTH, CHUNK_HEIGHT, (pos.y + 1) * CHUNK_WIDTH);
        
        if (frustum.isBoxInFrustum(min, max)) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), min);
            shader.setMat4("model", model);
            mesh->render();
        }
    }
}

void ChunkProvider::renderTransparent(Shader& shader, const Frustum& frustum) {
    m_atlas.bind(0);
    for (auto const& [pos, mesh] : m_meshes) {
        glm::vec3 min(pos.x * CHUNK_WIDTH, 0, pos.y * CHUNK_WIDTH);
        glm::vec3 max((pos.x + 1) * CHUNK_WIDTH, CHUNK_HEIGHT, (pos.y + 1) * CHUNK_WIDTH);
        
        if (frustum.isBoxInFrustum(min, max)) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), min);
            shader.setMat4("model", model);
            mesh->renderTransparent();
        }
    }
}

void ChunkProvider::loadAround(int centerX, int centerZ, int distance) {
    for (int x = centerX - distance; x <= centerX + distance; ++x) {
        for (int z = centerZ - distance; z <= centerZ + distance; ++z) {
            glm::ivec2 pos(x, z);
            
            if (m_world.getChunks().find(pos) == m_world.getChunks().end()) {
                m_world.loadChunk(x, z);
                m_generator.generateChunk(m_world.getChunks().at(pos).get());
                enqueueMesh(pos);
            }

            if (m_meshes.find(pos) == m_meshes.end()) {
                m_meshes[pos] = std::make_unique<ChunkMesh>();
                enqueueMesh(pos);
            } else if (m_world.getChunks().at(pos)->isDirty()) {
                enqueueMesh(pos);
            }
        }
    }
}

void ChunkProvider::unloadFar(int centerX, int centerZ, int distance) {
    auto it = m_meshes.begin();
    while (it != m_meshes.end()) {
        if (std::abs(it->first.x - centerX) > distance + 1 || std::abs(it->first.y - centerZ) > distance + 1) {
            m_world.unloadChunk(it->first.x, it->first.y);
            it = m_meshes.erase(it);
        } else {
            ++it;
        }
    }
}

void ChunkProvider::enqueueMesh(const glm::ivec2& pos) {
    if (std::find(m_meshQueue.begin(), m_meshQueue.end(), pos) != m_meshQueue.end()) return;
    m_meshQueue.push_back(pos);
}

void ChunkProvider::processMeshQueue(int budget) {
    while (budget > 0 && !m_meshQueue.empty()) {
        glm::ivec2 pos = m_meshQueue.front();
        m_meshQueue.pop_front();
        auto chunkIt = m_world.getChunks().find(pos);
        auto meshIt = m_meshes.find(pos);
        if (chunkIt != m_world.getChunks().end() && meshIt != m_meshes.end()) {
            meshIt->second->generate(*chunkIt->second, m_atlas);
            chunkIt->second->setDirty(false);
        }
        --budget;
    }
}
