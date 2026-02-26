#pragma once

#include <algorithm>
#include <cmath>
#include <map>
#include <deque>
#include <memory>
#include <string>
#include <vector>
#include "world/World.hpp"
#include "world/WorldGenerator.hpp"
#include "render/ChunkMesh.hpp"
#include "render/TextureAtlas.hpp"
#include "render/Shader.hpp"
#include "render/Frustum.hpp"

class ChunkProvider {
public:
    ChunkProvider(World& world, WorldGenerator& generator, TextureAtlas& atlas, const std::string& worldPath);
    ~ChunkProvider();

    void update(glm::vec3 playerPos, int renderDistance);
    void render(Shader& shader, const Frustum& frustum);
    void renderTransparent(Shader& shader, const Frustum& frustum, const glm::vec3& cameraPos);
    static inline std::vector<glm::ivec2> sortTransparentPositions(const std::vector<glm::ivec2>& positions,
                                                                   const glm::vec3& cameraPos) {
        struct TransparentEntry {
            float distanceSq;
            glm::ivec2 pos;
        };

        std::vector<TransparentEntry> entries;
        entries.reserve(positions.size());
        for (const glm::ivec2& pos : positions) {
            glm::vec3 min(pos.x * CHUNK_WIDTH, 0, pos.y * CHUNK_WIDTH);
            glm::vec3 max((pos.x + 1) * CHUNK_WIDTH, CHUNK_HEIGHT, (pos.y + 1) * CHUNK_WIDTH);
            glm::vec3 center = 0.5f * (min + max);
            entries.push_back({glm::dot(center - cameraPos, center - cameraPos), pos});
        }

        std::sort(entries.begin(), entries.end(), [](const TransparentEntry& a, const TransparentEntry& b) {
            float distanceDiff = std::abs(a.distanceSq - b.distanceSq);
            if (distanceDiff > 1e-5f) return a.distanceSq > b.distanceSq;
            if (a.pos.x != b.pos.x) return a.pos.x < b.pos.x;
            return a.pos.y < b.pos.y;
        });

        std::vector<glm::ivec2> sorted;
        sorted.reserve(entries.size());
        for (const TransparentEntry& entry : entries) {
            sorted.push_back(entry.pos);
        }
        return sorted;
    }

private:
    World& m_world;
    WorldGenerator& m_generator;
    TextureAtlas& m_atlas;
    std::string m_worldPath;

    std::map<glm::ivec2, std::unique_ptr<ChunkMesh>, IVec2Compare> m_meshes;
    std::deque<glm::ivec2> m_meshQueue;

    void loadAround(int centerX, int centerZ, int distance);
    void unloadFar(int centerX, int centerZ, int distance);
    void enqueueMesh(const glm::ivec2& pos);
    void processMeshQueue(int budget);
};
