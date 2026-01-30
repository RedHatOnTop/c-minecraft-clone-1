#pragma once

#include <vector>
#include "item/ItemStack.hpp"

class Inventory {
public:
    Inventory(int size);
    ~Inventory();

    void setStack(int slot, const ItemStack& stack);
    ItemStack getStack(int slot) const;
    
    bool addItem(ItemStack& stack);
    void removeStack(int slot, int count);

private:
    std::vector<ItemStack> m_slots;
};
