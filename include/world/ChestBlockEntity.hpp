#pragma once

#include "world/BlockEntity.hpp"
#include "item/Inventory.hpp"

class ChestBlockEntity : public BlockEntity {
public:
    Inventory inventory;

    ChestBlockEntity() : inventory(27) {}

    BlockEntityType type() const override { return BlockEntityType::Chest; }

    void serialize(std::ostream& out) const override;
    void deserialize(std::istream& in) override;
};

