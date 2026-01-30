#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "world/Block.hpp"

const int CHUNK_WIDTH = 16;
const int CHUNK_HEIGHT = 256;
const int CHUNK_SIZE = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT;

class Chunk {
public:
    Chunk(int x, int z);
    ~Chunk();

    void setBlock(int x, int y, int z, Block block);
    Block getBlock(int x, int y, int z) const;

    glm::ivec2 getPosition() const { return m_position; }

    bool isDirty() const { return m_dirty; }
    void setDirty(bool dirty) { m_dirty = dirty; }

private:
    glm::ivec2 m_position; // Chunk coordinates (not world coordinates)
    std::vector<Block> m_blocks;
    bool m_dirty;

    int getIndex(int x, int y, int z) const {
        return x + (y * CHUNK_WIDTH) + (z * CHUNK_WIDTH * CHUNK_HEIGHT);
    }
};
