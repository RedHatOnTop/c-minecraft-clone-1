#include "item/Inventory.hpp"
#include <algorithm>

Inventory::Inventory(int size) {
    m_slots.resize(size);
}

Inventory::~Inventory() {}

void Inventory::setStack(int slot, const ItemStack& stack) {
    if (slot >= 0 && slot < (int)m_slots.size()) {
        m_slots[slot] = stack;
    }
}

ItemStack Inventory::getStack(int slot) const {
    if (slot >= 0 && slot < (int)m_slots.size()) {
        return m_slots[slot];
    }
    return ItemStack();
}

bool Inventory::addItem(ItemStack& stack) {
    // First pass: try to stack with existing items
    for (auto& slot : m_slots) {
        if (!slot.isEmpty() && slot.itemID == stack.itemID && slot.metadata == stack.metadata) {
            Item* item = slot.getItem();
            int canAdd = item->maxStackSize - slot.count;
            int toAdd = std::min(canAdd, stack.count);
            slot.count += toAdd;
            stack.count -= toAdd;
            if (stack.count <= 0) return true;
        }
    }

    // Second pass: find empty slots
    for (auto& slot : m_slots) {
        if (slot.isEmpty()) {
            slot = stack;
            stack.count = 0;
            return true;
        }
    }

    return stack.count <= 0;
}

void Inventory::removeStack(int slot, int count) {
    if (slot >= 0 && slot < (int)m_slots.size()) {
        m_slots[slot].count -= count;
        if (m_slots[slot].count <= 0) {
            m_slots[slot] = ItemStack();
        }
    }
}
