#include "world/Lighting.hpp"

void Lighting::recalculateSkylight(Chunk& chunk) {
    for (int x = 0; x < CHUNK_WIDTH; ++x) {
        for (int z = 0; z < CHUNK_WIDTH; ++z) {
            uint8_t sky = 15;
            for (int y = CHUNK_HEIGHT - 1; y >= 0; --y) {
                Block b = chunk.getBlock(x, y, z);
                if (sky > 0 && isBlockOpaque(b.id)) {
                    sky = 0;
                }
                chunk.setLight(x, y, z, sky, getBlockLight(b));
            }
        }
    }
}

void Lighting::recalculateBlocklight(Chunk& chunk) {
    for (int x = 0; x < CHUNK_WIDTH; ++x) {
        for (int z = 0; z < CHUNK_WIDTH; ++z) {
            for (int y = 0; y < CHUNK_HEIGHT; ++y) {
                Block b = chunk.getBlock(x, y, z);
                uint8_t bl = 0;
                if (b.id == Blocks::LAVA) bl = 15;
                chunk.setLight(x, y, z, getSkyLight(b), bl);
            }
        }
    }
}

void Lighting::recalculateAll(Chunk& chunk) {
    recalculateBlocklight(chunk);
    recalculateSkylight(chunk);
}

