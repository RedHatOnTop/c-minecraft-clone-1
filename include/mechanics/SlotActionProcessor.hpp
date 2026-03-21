#pragma once

#include "item/ItemStack.hpp"
#include "item/Inventory.hpp"

enum class ClickAction { LEFT_CLICK, RIGHT_CLICK, SHIFT_CLICK };

struct SlotActionResult {
    ItemStack cursorAfter;
    ItemStack slotAfter;
    bool changed;
};

class SlotActionProcessor {
public:
    static SlotActionResult processClick(
        ClickAction action,
        const ItemStack& cursor,
        const ItemStack& slot,
        int maxStackSize = 64
    );

    static bool processShiftClick(
        Inventory& source, int sourceSlot,
        Inventory& target, int targetStart, int targetEnd
    );
};
