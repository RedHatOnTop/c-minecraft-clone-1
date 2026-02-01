#pragma once

#include <map>
#include <memory>
#include <glm/glm.hpp>
#include "world/Chunk.hpp"

// Comparator for ivec2 to use in map
struct IVec2Compare {
    bool operator()(const glm::ivec2& a, const glm::ivec2& b) const {
        if (a.x != b.x) return a.x < b.x;
        return a.y < b.y;
    }
};

class World {
public:
    World();
    ~World();

    void setBlock(int x, int y, int z, Block block);
    Block getBlock(int x, int y, int z);

    void loadChunk(int x, int z);
    void unloadChunk(int x, int z);

    void updateLiquids();

    const std::map<glm::ivec2, std::shared_ptr<Chunk>, IVec2Compare>& getChunks() const { return m_chunks; }

private:
    std::map<glm::ivec2, std::shared_ptr<Chunk>, IVec2Compare> m_chunks;
};
