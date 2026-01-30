#pragma once

#include "world/Chunk.hpp"
#include "FastNoiseLite.h"

class WorldGenerator {
public:
    WorldGenerator(int seed);
    ~WorldGenerator();

    void generateChunk(Chunk* chunk);

private:
    int m_seed;
    FastNoiseLite m_noise;

    void generateTerrain(Chunk* chunk);
    void generateCaves(Chunk* chunk);
    void generateStructures(Chunk* chunk);
};
