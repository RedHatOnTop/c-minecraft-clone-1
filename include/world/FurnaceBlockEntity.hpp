#pragma once

#include "world/BlockEntity.hpp"
#include "item/Inventory.hpp"

class FurnaceBlockEntity : public BlockEntity {
public:
    Inventory inventory;
    float burnTime;
    float cookTime;

    FurnaceBlockEntity() : inventory(3), burnTime(0.0f), cookTime(0.0f) {}

    BlockEntityType type() const override { return BlockEntityType::Furnace; }

    void serialize(std::ostream& out) const override;
    void deserialize(std::istream& in) override;
};

