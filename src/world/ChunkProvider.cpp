#include "world/ChunkProvider.hpp"
#include <glm/gtc/matrix_transform.hpp>

ChunkProvider::ChunkProvider(World& world, WorldGenerator& generator, TextureAtlas& atlas)
    : m_world(world), m_generator(generator), m_atlas(atlas) {}

ChunkProvider::~ChunkProvider() {}

void ChunkProvider::update(glm::vec3 playerPos, int renderDistance) {
    int px = std::floor(playerPos.x / CHUNK_WIDTH);
    int pz = std::floor(playerPos.z / CHUNK_WIDTH);

    loadAround(px, pz, renderDistance);
    unloadFar(px, pz, renderDistance);
}

void ChunkProvider::render(Shader& shader) {
    m_atlas.bind(0);
    for (auto const& [pos, mesh] : m_meshes) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x * CHUNK_WIDTH, 0, pos.y * CHUNK_WIDTH));
        shader.setMat4("model", model);
        mesh->render();
    }
}

void ChunkProvider::loadAround(int centerX, int centerZ, int distance) {
    for (int x = centerX - distance; x <= centerX + distance; ++x) {
        for (int z = centerZ - distance; z <= centerZ + distance; ++z) {
            glm::ivec2 pos(x, z);
            
            // 1. Ensure chunk data exists
            if (m_world.getChunks().find(pos) == m_world.getChunks().end()) {
                m_world.loadChunk(x, z);
                m_generator.generateChunk(m_world.getChunks().at(pos).get());
            }

            // 2. Ensure mesh exists and is up to date
            if (m_meshes.find(pos) == m_meshes.end()) {
                auto mesh = std::make_unique<ChunkMesh>();
                mesh->generate(*m_world.getChunks().at(pos), m_atlas);
                m_meshes[pos] = std::move(mesh);
            } else if (m_world.getChunks().at(pos)->isDirty()) {
                m_meshes[pos]->generate(*m_world.getChunks().at(pos), m_atlas);
                m_world.getChunks().at(pos)->setDirty(false);
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
