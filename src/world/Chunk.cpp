#include "world/Chunk.hpp"

Chunk::Chunk(int x, int z) : m_position(x, z), m_dirty(true) {
    m_blocks.resize(CHUNK_SIZE);
}

Chunk::~Chunk() {}

void Chunk::setBlock(int x, int y, int z, Block block) {
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_WIDTH) return;
    m_blocks[getIndex(x, y, z)] = block;
    m_dirty = true;
}

Block Chunk::getBlock(int x, int y, int z) const {
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_WIDTH) return Block(Blocks::AIR);
    return m_blocks[getIndex(x, y, z)];
}
