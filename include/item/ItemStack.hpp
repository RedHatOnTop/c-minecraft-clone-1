#pragma once

#include "item/Item.hpp"

struct ItemStack {
    ItemID itemID;
    int count;
    int metadata;

    ItemStack() : itemID(0), count(0), metadata(0) {}
    ItemStack(ItemID id, int count = 1, int meta = 0) : itemID(id), count(count), metadata(meta) {}

    bool isEmpty() const { return itemID == 0 || count <= 0; }
    
    Item* getItem() const {
        return ItemRegistry::getItem(itemID);
    }
};
