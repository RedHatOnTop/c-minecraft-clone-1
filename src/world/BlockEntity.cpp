#include "world/ChestBlockEntity.hpp"
#include "world/FurnaceBlockEntity.hpp"
#include <cstdint>
#include <algorithm>

static void writeItemStack(std::ostream& out, const ItemStack& st) {
    out.write((const char*)&st.itemID, sizeof(st.itemID));
    int32_t count = (int32_t)st.count;
    int32_t meta = (int32_t)st.metadata;
    out.write((const char*)&count, sizeof(count));
    out.write((const char*)&meta, sizeof(meta));
}

static ItemStack readItemStack(std::istream& in) {
    ItemID id = 0;
    int32_t count = 0;
    int32_t meta = 0;
    in.read((char*)&id, sizeof(id));
    in.read((char*)&count, sizeof(count));
    in.read((char*)&meta, sizeof(meta));
    return ItemStack(id, (int)count, (int)meta);
}

void ChestBlockEntity::serialize(std::ostream& out) const {
    int32_t slotCount = (int32_t)inventory.size();
    out.write((const char*)&slotCount, sizeof(slotCount));
    for (int i = 0; i < slotCount; ++i) {
        writeItemStack(out, inventory.getStack(i));
    }
}

void ChestBlockEntity::deserialize(std::istream& in) {
    int32_t slotCount = 0;
    in.read((char*)&slotCount, sizeof(slotCount));
    slotCount = std::max<int32_t>(0, slotCount);
    for (int i = 0; i < slotCount && i < inventory.size(); ++i) {
        inventory.setStack(i, readItemStack(in));
    }
}

void FurnaceBlockEntity::serialize(std::ostream& out) const {
    int32_t slotCount = (int32_t)inventory.size();
    out.write((const char*)&slotCount, sizeof(slotCount));
    for (int i = 0; i < slotCount; ++i) {
        writeItemStack(out, inventory.getStack(i));
    }
    out.write((const char*)&burnTime, sizeof(burnTime));
    out.write((const char*)&cookTime, sizeof(cookTime));
}

void FurnaceBlockEntity::deserialize(std::istream& in) {
    int32_t slotCount = 0;
    in.read((char*)&slotCount, sizeof(slotCount));
    slotCount = std::max<int32_t>(0, slotCount);
    for (int i = 0; i < slotCount && i < inventory.size(); ++i) {
        inventory.setStack(i, readItemStack(in));
    }
    in.read((char*)&burnTime, sizeof(burnTime));
    in.read((char*)&cookTime, sizeof(cookTime));
}
