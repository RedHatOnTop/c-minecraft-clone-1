#include "world/WorldGenerator.hpp"
#include <algorithm>

WorldGenerator::WorldGenerator(int seed) : m_seed(seed) {
    m_noise.SetSeed(m_seed);
    m_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_noise.SetFrequency(0.01f);
}

WorldGenerator::~WorldGenerator() {}

void WorldGenerator::generateChunk(Chunk* chunk) {
    generateTerrain(chunk);
    // Caves and structures would be added here
}

void WorldGenerator::generateTerrain(Chunk* chunk) {
    int chunkX = chunk->getPosition().x;
    int chunkZ = chunk->getPosition().y;

    FastNoiseLite bioNoise;
    bioNoise.SetSeed(m_seed + 1);
    bioNoise.SetFrequency(0.005f);

    for (int x = 0; x < CHUNK_WIDTH; ++x) {
        for (int z = 0; z < CHUNK_WIDTH; ++z) {
            float worldX = (float)(chunkX * CHUNK_WIDTH + x);
            float worldZ = (float)(chunkZ * CHUNK_WIDTH + z);
            
            float bioVal = bioNoise.GetNoise(worldX, worldZ);
            
            float frequency = 0.01f;
            float amplitude = 32.0f;
            int baseHeight = 64;

            if (bioVal > 0.3f) { // Mountains
                frequency = 0.02f;
                amplitude = 64.0f;
            } else if (bioVal < -0.3f) { // Plains
                frequency = 0.005f;
                amplitude = 16.0f;
            }

            m_noise.SetFrequency(frequency);
            float heightNoise = m_noise.GetNoise(worldX, worldZ);
            int height = (int)((heightNoise + 1.0f) * 0.5f * amplitude) + baseHeight;
            height = std::clamp(height, 0, CHUNK_HEIGHT - 1);

            for (int y = 0; y < CHUNK_HEIGHT; ++y) {
                if (y == 0) {
                    chunk->setBlock(x, y, z, Block(Blocks::BEDROCK));
                } else if (y < height - 4) {
                    chunk->setBlock(x, y, z, Block(Blocks::STONE));
                } else if (y < height - 1) {
                    chunk->setBlock(x, y, z, Block(Blocks::DIRT));
                } else if (y == height - 1) {
                    BlockID topBlock = (bioVal < -0.5f && height < 70) ? Blocks::SAND : Blocks::GRASS;
                    chunk->setBlock(x, y, z, Block(topBlock));
                } else if (y < 64) {
                    chunk->setBlock(x, y, z, Block(Blocks::WATER));
                } else {
                    chunk->setBlock(x, y, z, Block(Blocks::AIR));
                }
            }
        }
    }
}
