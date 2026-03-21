#include "mechanics/SlotActionProcessor.hpp"

// Stub — implementation in task 7.1
SlotActionResult SlotActionProcessor::processClick(
    ClickAction /*action*/,
    const ItemStack& cursor,
    const ItemStack& slot,
    int /*maxStackSize*/
) {
    return {cursor, slot, false};
}

bool SlotActionProcessor::processShiftClick(
    Inventory& /*source*/, int /*sourceSlot*/,
    Inventory& /*target*/, int /*targetStart*/, int /*targetEnd*/
) {
    return false;
}
