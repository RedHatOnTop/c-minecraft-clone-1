#include "test_common.hpp"
#include "mechanics/BreakDurationCalculator.hpp"

// Stub — property and unit tests added in task 5.2
void test_BreakDuration_StoneIronPickaxe() {
    // Placeholder: stone + iron pickaxe = 0.75s
    (void)BreakDurationCalculator::calculate(0, ToolType::PICKAXE, ToolTier::IRON);
}

void test_BreakDuration_BedrockUnbreakable() {
    // Placeholder: bedrock returns -1
    (void)BreakDurationCalculator::calculate(0, ToolType::NONE, ToolTier::HAND);
}
