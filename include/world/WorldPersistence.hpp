#pragma once

#include <string>
#include "world/Chunk.hpp"

class WorldPersistence {
public:
    static bool saveChunk(const std::string& worldPath, const Chunk& chunk);
    static bool loadChunk(const std::string& worldPath, Chunk& chunk);

private:
    static std::string getChunkPath(const std::string& worldPath, int x, int z);
};
