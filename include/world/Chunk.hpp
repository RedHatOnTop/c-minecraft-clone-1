#pragma once

#include <map>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "world/Block.hpp"
#include "world/BlockEntity.hpp"

const int CHUNK_WIDTH = 16;
const int CHUNK_HEIGHT = 256;
const int CHUNK_SIZE = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT;

class Chunk {
public:
    Chunk(int x, int z);
    ~Chunk();

    void setBlock(int x, int y, int z, Block block);
    Block getBlock(int x, int y, int z) const;
    void setLight(int x, int y, int z, uint8_t sky, uint8_t block);

    glm::ivec2 getPosition() const { return m_position; }

    bool isDirty() const { return m_dirty; }
    void setDirty(bool dirty) { m_dirty = dirty; }

    BlockEntity* getBlockEntity(int x, int y, int z);
    const BlockEntity* getBlockEntity(int x, int y, int z) const;
    BlockEntity* ensureBlockEntity(int x, int y, int z, BlockID blockId);
    void removeBlockEntity(int x, int y, int z);

    const std::map<int, std::unique_ptr<BlockEntity>>& getBlockEntities() const { return m_blockEntities; }

private:
    glm::ivec2 m_position; // Chunk coordinates (not world coordinates)
    std::vector<Block> m_blocks;
    bool m_dirty;
    std::map<int, std::unique_ptr<BlockEntity>> m_blockEntities;

    int getIndex(int x, int y, int z) const {
        return x + (y * CHUNK_WIDTH) + (z * CHUNK_WIDTH * CHUNK_HEIGHT);
    }
};
