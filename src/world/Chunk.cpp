#include "world/Chunk.hpp"
#include "world/ChestBlockEntity.hpp"
#include "world/FurnaceBlockEntity.hpp"

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

void Chunk::setLight(int x, int y, int z, uint8_t sky, uint8_t block) {
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_WIDTH) return;
    Block& b = m_blocks[getIndex(x, y, z)];
    setSkyLight(b, sky);
    setBlockLight(b, block);
}

BlockEntity* Chunk::getBlockEntity(int x, int y, int z) {
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_WIDTH) return nullptr;
    int idx = getIndex(x, y, z);
    auto it = m_blockEntities.find(idx);
    return it == m_blockEntities.end() ? nullptr : it->second.get();
}

const BlockEntity* Chunk::getBlockEntity(int x, int y, int z) const {
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_WIDTH) return nullptr;
    int idx = getIndex(x, y, z);
    auto it = m_blockEntities.find(idx);
    return it == m_blockEntities.end() ? nullptr : it->second.get();
}

BlockEntity* Chunk::ensureBlockEntity(int x, int y, int z, BlockID blockId) {
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_WIDTH) return nullptr;
    int idx = getIndex(x, y, z);
    auto& ptr = m_blockEntities[idx];
    if (!ptr) {
        if (blockId == Blocks::CHEST) ptr = std::make_unique<ChestBlockEntity>();
        else if (blockId == Blocks::FURNACE) ptr = std::make_unique<FurnaceBlockEntity>();
        else m_blockEntities.erase(idx);
    }
    return getBlockEntity(x, y, z);
}

void Chunk::removeBlockEntity(int x, int y, int z) {
    if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_WIDTH) return;
    int idx = getIndex(x, y, z);
    m_blockEntities.erase(idx);
}
