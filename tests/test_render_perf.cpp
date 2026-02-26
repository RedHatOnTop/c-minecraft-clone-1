#include <algorithm>
#include <chrono>
#include <vector>

#include "test_common.hpp"
#include "world/ChunkProvider.hpp"

void test_RenderPerf_TransparentSort_P95Budget() {
    std::vector<glm::ivec2> positions;
    positions.reserve(4096);
    for (int x = -32; x < 32; ++x) {
        for (int z = -32; z < 32; ++z) {
            positions.push_back(glm::ivec2(x, z));
        }
    }

    const glm::vec3 cameraPos(4.0f, CHUNK_HEIGHT * 0.5f, -7.0f);
    std::vector<double> elapsedMs;
    elapsedMs.reserve(120);

    for (int i = 0; i < 120; ++i) {
        const auto start = std::chrono::steady_clock::now();
        std::vector<glm::ivec2> sorted = ChunkProvider::sortTransparentPositions(positions, cameraPos);
        const auto end = std::chrono::steady_clock::now();
        TEST_ASSERT(sorted.size() == positions.size());
        elapsedMs.push_back(std::chrono::duration<double, std::milli>(end - start).count());
    }

    std::sort(elapsedMs.begin(), elapsedMs.end());
    const size_t p95Index = static_cast<size_t>(elapsedMs.size() * 0.95);
    const double p95 = elapsedMs[std::min(p95Index, elapsedMs.size() - 1)];

    // Integration-stage performance proxy budget for a hot render-adjacent path.
    TEST_ASSERT(p95 < 6.0);
}
