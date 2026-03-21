#pragma once

#include "world/Block.hpp"
#include <cstdint>

enum class ToolType { NONE, PICKAXE, AXE, SHOVEL, SWORD };
enum class ToolTier { HAND, WOOD, STONE, IRON, DIAMOND };

struct BlockHardness {
    float hardness;
    bool requiresTool;
    ToolType preferredTool;
};

class BreakDurationCalculator {
public:
    static float calculate(BlockID blockId, ToolType heldTool, ToolTier heldTier);
    static const BlockHardness& getHardness(BlockID blockId);
    static float getToolSpeed(ToolType type, ToolTier tier);
};
