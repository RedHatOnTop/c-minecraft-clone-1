#include "world/World.hpp"
#include "world/Lighting.hpp"
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
        Lighting::recalculateAll(*m_chunks[chunkPos]);
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

void World::updateLiquids() {
    // Simplified cellular automata for liquid flow
    // In a real implementation, we'd use a queue of 'scheduled' updates
    std::vector<std::pair<glm::ivec3, Block>> updates;

    for (auto const& [pos, chunk] : m_chunks) {
        for (int y = 1; y < CHUNK_HEIGHT; ++y) {
            for (int x = 0; x < CHUNK_WIDTH; ++x) {
                for (int z = 0; z < CHUNK_WIDTH; ++z) {
                    Block b = chunk->getBlock(x, y, z);
                    if (b.id == Blocks::WATER) {
                        int wx = pos.x * CHUNK_WIDTH + x;
                        int wz = pos.y * CHUNK_WIDTH + z;

                        // Flow down
                        if (getBlock(wx, y - 1, wz).id == Blocks::AIR) {
                            updates.push_back({{wx, y - 1, wz}, Block(Blocks::WATER)});
                        } 
                        // Flow sideways if on solid ground (simplified)
                        else if (getBlock(wx, y - 1, wz).id != Blocks::WATER) {
                            if (getBlock(wx + 1, y, wz).id == Blocks::AIR) updates.push_back({{wx + 1, y, wz}, Block(Blocks::WATER)});
                            if (getBlock(wx - 1, y, wz).id == Blocks::AIR) updates.push_back({{wx - 1, y, wz}, Block(Blocks::WATER)});
                            if (getBlock(wx, y, wz + 1).id == Blocks::AIR) updates.push_back({{wx, y, wz + 1}, Block(Blocks::WATER)});
                            if (getBlock(wx, y, wz - 1).id == Blocks::AIR) updates.push_back({{wx, y, wz - 1}, Block(Blocks::WATER)});
                        }
                    }
                }
            }
        }
    }

    for (auto const& update : updates) {
        setBlock(update.first.x, update.first.y, update.first.z, update.second);
    }
}
