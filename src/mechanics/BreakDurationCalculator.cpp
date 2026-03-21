#include "mechanics/BreakDurationCalculator.hpp"

// Stub — implementation in task 5.1
static BlockHardness s_defaultHardness{1.0f, false, ToolType::NONE};

float BreakDurationCalculator::calculate(BlockID /*blockId*/, ToolType /*heldTool*/, ToolTier /*heldTier*/) {
    return 1.0f;
}

const BlockHardness& BreakDurationCalculator::getHardness(BlockID /*blockId*/) {
    return s_defaultHardness;
}

float BreakDurationCalculator::getToolSpeed(ToolType /*type*/, ToolTier /*tier*/) {
    return 1.0f;
}
