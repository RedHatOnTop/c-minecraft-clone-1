#include <algorithm>
#include <cstdint>

#include "test_common.hpp"
#include "world/Chunk.hpp"
#include "world/WorldGenerator.hpp"

#if defined(_WIN32)
#include <windows.h>
#include <psapi.h>
#endif

namespace {
uint64_t currentProcessWorkingSet() {
#if defined(_WIN32)
    PROCESS_MEMORY_COUNTERS counters{};
    if (GetProcessMemoryInfo(GetCurrentProcess(), &counters, sizeof(counters))) {
        return static_cast<uint64_t>(counters.WorkingSetSize);
    }
#endif
    return 0;
}
}  // namespace

void test_Runtime_MemoryGrowth_SoakProfile() {
    const uint64_t start = currentProcessWorkingSet();

    for (int iteration = 0; iteration < 150; ++iteration) {
        WorldGenerator generator(12345 + iteration);
        for (int cx = -1; cx <= 1; ++cx) {
            for (int cz = -1; cz <= 1; ++cz) {
                Chunk chunk(cx, cz);
                generator.generateChunk(&chunk);
            }
        }
    }

    const uint64_t end = currentProcessWorkingSet();
    if (start == 0 || end == 0) {
        // If the platform memory API is unavailable, this check is not enforceable.
        return;
    }

    const uint64_t allowedGrowth = (start / 3) + (32ull * 1024ull * 1024ull);  // 33% + 32 MiB cushion
    TEST_ASSERT(end <= start + allowedGrowth);
}
