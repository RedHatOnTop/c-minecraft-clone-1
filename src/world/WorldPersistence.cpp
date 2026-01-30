#include "world/WorldPersistence.hpp"
#include <fstream>
#include <iostream>

bool WorldPersistence::saveChunk(const std::string& worldPath, const Chunk& chunk) {
    std::string path = getChunkPath(worldPath, chunk.getPosition().x, chunk.getPosition().y);
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) return false;

    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        for (int x = 0; x < CHUNK_WIDTH; ++x) {
            for (int z = 0; z < CHUNK_WIDTH; ++z) {
                Block b = chunk.getBlock(x, y, z);
                out.write((char*)&b.id, sizeof(b.id));
                out.write((char*)&b.metadata, sizeof(b.metadata));
            }
        }
    }
    out.close();
    return true;
}

bool WorldPersistence::loadChunk(const std::string& worldPath, Chunk& chunk) {
    std::string path = getChunkPath(worldPath, chunk.getPosition().x, chunk.getPosition().y);
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) return false;

    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        for (int x = 0; x < CHUNK_WIDTH; ++x) {
            for (int z = 0; z < CHUNK_WIDTH; ++z) {
                Block b;
                in.read((char*)&b.id, sizeof(b.id));
                in.read((char*)&b.metadata, sizeof(b.metadata));
                chunk.setBlock(x, y, z, b);
            }
        }
    }
    in.close();
    return true;
}

std::string WorldPersistence::getChunkPath(const std::string& worldPath, int x, int z) {
    return worldPath + "/chunk_" + std::to_string(x) + "_" + std::to_string(z) + ".dat";
}
