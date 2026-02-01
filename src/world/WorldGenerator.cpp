#include "world/WorldGenerator.hpp"
#include "world/Lighting.hpp"
#include <algorithm>

WorldGenerator::WorldGenerator(int seed) : m_seed(seed) {
    m_noise.SetSeed(m_seed);
    m_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_noise.SetFrequency(0.01f);

    m_caveNoise.SetSeed(m_seed + 2);
    m_caveNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_caveNoise.SetFrequency(0.05f);
}

WorldGenerator::~WorldGenerator() {}

void WorldGenerator::generateChunk(Chunk* chunk) {
    generateTerrain(chunk);
    generateCaves(chunk);
    generateDecorations(chunk);
    Lighting::recalculateAll(*chunk);
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

void WorldGenerator::generateCaves(Chunk* chunk) {
    int chunkX = chunk->getPosition().x;
    int chunkZ = chunk->getPosition().y;

    for (int y = 1; y < 128; ++y) {
        for (int x = 0; x < CHUNK_WIDTH; ++x) {
            for (int z = 0; z < CHUNK_WIDTH; ++z) {
                float worldX = (float)(chunkX * CHUNK_WIDTH + x);
                float worldY = (float)y;
                float worldZ = (float)(chunkZ * CHUNK_WIDTH + z);

                float noise = m_caveNoise.GetNoise(worldX, worldY, worldZ);
                if (noise > 0.5f) {
                    chunk->setBlock(x, y, z, Block(Blocks::AIR));
                }
            }
        }
    }
}

void WorldGenerator::generateDecorations(Chunk* chunk) {
    int chunkX = chunk->getPosition().x;
    int chunkZ = chunk->getPosition().y;

    for (int x = 2; x < CHUNK_WIDTH - 2; ++x) {
        for (int z = 2; z < CHUNK_WIDTH - 2; ++z) {
            // Simple deterministic check for tree placement
            unsigned int seed = m_seed ^ (chunkX * CHUNK_WIDTH + x) ^ (chunkZ * CHUNK_WIDTH + z);
            seed = (seed * 1103515245 + 12345) & 0x7fffffff;
            
            if (seed % 100 == 0) { // 1% chance per column
                // Find surface height
                for (int y = CHUNK_HEIGHT - 1; y > 0; --y) {
                    if (chunk->getBlock(x, y, z).id == Blocks::GRASS) {
                        generateTree(chunk, x, y + 1, z);
                        break;
                    }
                }
            }
        }
    }
}

void WorldGenerator::generateTree(Chunk* chunk, int x, int y, int z) {
    // Trunk
    int trunkHeight = 5;
    for (int i = 0; i < trunkHeight; ++i) {
        if (y + i < CHUNK_HEIGHT)
            chunk->setBlock(x, y + i, z, Block(Blocks::LOG));
    }

    // Leaves
    for (int ly = -2; ly <= 2; ++ly) {
        for (int lx = -2; lx <= 2; ++lx) {
            for (int lz = -2; lz <= 2; ++lz) {
                if (lx * lx + ly * ly + lz * lz <= 5) { // Simple sphere-ish shape
                    int leafX = x + lx;
                    int leafY = y + trunkHeight + ly - 1;
                    int leafZ = z + lz;
                    if (leafX >= 0 && leafX < CHUNK_WIDTH && leafY >= 0 && leafY < CHUNK_HEIGHT && leafZ >= 0 && leafZ < CHUNK_WIDTH) {
                        if (chunk->getBlock(leafX, leafY, leafZ).id == Blocks::AIR) {
                            chunk->setBlock(leafX, leafY, leafZ, Block(Blocks::LEAVES));
                        }
                    }
                }
            }
        }
    }
}
