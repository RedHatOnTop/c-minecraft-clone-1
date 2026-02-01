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
    FastNoiseLite m_caveNoise;

    void generateTerrain(Chunk* chunk);
    void generateCaves(Chunk* chunk);
    void generateDecorations(Chunk* chunk);
    void generateTree(Chunk* chunk, int x, int y, int z);
};
