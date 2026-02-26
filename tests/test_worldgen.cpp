#include <algorithm>
#include <array>
#include <numeric>

#include "test_common.hpp"
#include "world/Chunk.hpp"
#include "world/WorldGenerator.hpp"

namespace {
int surfaceHeight(const Chunk& chunk, int x, int z) {
    for (int y = CHUNK_HEIGHT - 1; y >= 0; --y) {
        Block b = chunk.getBlock(x, y, z);
        if (b.id != Blocks::AIR && b.id != Blocks::WATER) return y + 1;
    }
    return 0;
}
}  // namespace

void test_WorldGen_BiomeDistribution_FunctionalBaseline() {
    WorldGenerator generator(12345);
    std::array<int, 3> categories = {0, 0, 0};  // plains, mid, mountain
    int columns = 0;

    for (int cx = -2; cx <= 2; ++cx) {
        for (int cz = -2; cz <= 2; ++cz) {
            Chunk chunk(cx, cz);
            generator.generateChunk(&chunk);
            for (int x = 0; x < CHUNK_WIDTH; ++x) {
                for (int z = 0; z < CHUNK_WIDTH; ++z) {
                    const int h = surfaceHeight(chunk, x, z);
                    if (h < 70) {
                        categories[0]++;
                    } else if (h < 95) {
                        categories[1]++;
                    } else {
                        categories[2]++;
                    }
                    columns++;
                }
            }
        }
    }

    TEST_ASSERT(columns > 0);
    // Require all three terrain bands to appear in sampled chunks.
    TEST_ASSERT(categories[0] > 0);
    TEST_ASSERT(categories[1] > 0);
    TEST_ASSERT(categories[2] > 0);
}

void test_WorldGen_HeightAndDecoration_Profile() {
    WorldGenerator generator(12345);
    std::vector<int> heights;
    heights.reserve(CHUNK_WIDTH * CHUNK_WIDTH * 9);
    int logBlocks = 0;
    int leafBlocks = 0;

    for (int cx = -1; cx <= 1; ++cx) {
        for (int cz = -1; cz <= 1; ++cz) {
            Chunk chunk(cx, cz);
            generator.generateChunk(&chunk);
            for (int x = 0; x < CHUNK_WIDTH; ++x) {
                for (int z = 0; z < CHUNK_WIDTH; ++z) {
                    heights.push_back(surfaceHeight(chunk, x, z));
                    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
                        Block b = chunk.getBlock(x, y, z);
                        if (b.id == Blocks::LOG) logBlocks++;
                        if (b.id == Blocks::LEAVES) leafBlocks++;
                    }
                }
            }
        }
    }

    TEST_ASSERT(!heights.empty());
    const int minH = *std::min_element(heights.begin(), heights.end());
    const int maxH = *std::max_element(heights.begin(), heights.end());
    const float meanH = (float)std::accumulate(heights.begin(), heights.end(), 0.0) / (float)heights.size();

    TEST_ASSERT(minH >= 45);
    TEST_ASSERT(maxH <= 140);
    TEST_ASSERT((maxH - minH) >= 25);
    TEST_ASSERT(meanH >= 60.0f);
    TEST_ASSERT(meanH <= 95.0f);

    // Decoration generation should produce at least some trees in this 3x3 sample.
    TEST_ASSERT(logBlocks > 0);
    TEST_ASSERT(leafBlocks > 0);
}
