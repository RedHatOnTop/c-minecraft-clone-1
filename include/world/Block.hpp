#pragma once

#include <string>
#include <cstdint>

typedef uint8_t BlockID;

namespace Blocks {
    const BlockID AIR = 0;
    const BlockID STONE = 1;
    const BlockID GRASS = 2;
    const BlockID DIRT = 3;
    const BlockID COBBLESTONE = 4;
    const BlockID PLANKS = 5;
    const BlockID BEDROCK = 7;
    const BlockID WATER = 8;
    const BlockID LAVA = 10;
    const BlockID SAND = 12;
    const BlockID GRAVEL = 13;
    const BlockID LOG = 17;
    const BlockID LEAVES = 18;
}

struct Block {
    BlockID id;
    uint8_t metadata;
    uint8_t light; // 4 bits sky, 4 bits block

    Block() : id(Blocks::AIR), metadata(0), light(0) {}
    Block(BlockID id, uint8_t meta = 0) : id(id), metadata(meta), light(0) {}
};
