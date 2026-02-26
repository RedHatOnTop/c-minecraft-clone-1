#pragma once

#include <cstdint>
#include <istream>
#include <ostream>

enum class BlockEntityType : uint8_t {
    Chest = 1,
    Furnace = 2
};

class BlockEntity {
public:
    virtual ~BlockEntity() = default;
    virtual BlockEntityType type() const = 0;
    virtual void serialize(std::ostream& out) const = 0;
    virtual void deserialize(std::istream& in) = 0;
};

