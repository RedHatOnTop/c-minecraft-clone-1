#include "world/World.hpp"
#include <cmath>

World::World() {}
World::~World() {}

void World::setBlock(int x, int y, int z, Block block) {
    int cx = std::floor((float)x / CHUNK_WIDTH);
    int cz = std::floor((float)z / CHUNK_WIDTH);
    
    glm::ivec2 chunkPos(cx, cz);
    if (m_chunks.find(chunkPos) != m_chunks.end()) {
        int bx = x % CHUNK_WIDTH;
        if (bx < 0) bx += CHUNK_WIDTH;
        int bz = z % CHUNK_WIDTH;
        if (bz < 0) bz += CHUNK_WIDTH;
        
        m_chunks[chunkPos]->setBlock(bx, y, bz, block);
    }
}

Block World::getBlock(int x, int y, int z) {
    int cx = std::floor((float)x / CHUNK_WIDTH);
    int cz = std::floor((float)z / CHUNK_WIDTH);
    
    glm::ivec2 chunkPos(cx, cz);
    if (m_chunks.find(chunkPos) != m_chunks.end()) {
        int bx = x % CHUNK_WIDTH;
        if (bx < 0) bx += CHUNK_WIDTH;
        int bz = z % CHUNK_WIDTH;
        if (bz < 0) bz += CHUNK_WIDTH;
        
        return m_chunks[chunkPos]->getBlock(bx, y, bz);
    }
    return Block(Blocks::AIR);
}

void World::loadChunk(int x, int z) {
    glm::ivec2 pos(x, z);
    if (m_chunks.find(pos) == m_chunks.end()) {
        m_chunks[pos] = std::make_shared<Chunk>(x, z);
    }
}

void World::unloadChunk(int x, int z) {
    m_chunks.erase(glm::ivec2(x, z));
}
