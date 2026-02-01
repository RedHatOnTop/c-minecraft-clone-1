#pragma once

#include "world/Chunk.hpp"

class Lighting {
public:
    static void recalculateSkylight(Chunk& chunk);
    static void recalculateBlocklight(Chunk& chunk);
    static void recalculateAll(Chunk& chunk);
};

