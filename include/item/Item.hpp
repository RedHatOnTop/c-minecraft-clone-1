#pragma once

#include <string>
#include <map>
#include <memory>

typedef uint16_t ItemID;

class Item {
public:
    ItemID id;
    std::string unlocalizedName;
    int maxStackSize;

    Item(ItemID id, const std::string& name, int maxStack = 64) 
        : id(id), unlocalizedName(name), maxStackSize(maxStack) {}
    virtual ~Item() = default;
};

class ItemRegistry {
public:
    static void registerItem(std::unique_ptr<Item> item) {
        m_items[item->id] = std::move(item);
    }
    
    static Item* getItem(ItemID id) {
        return m_items.count(id) ? m_items[id].get() : nullptr;
    }

private:
    static inline std::map<ItemID, std::unique_ptr<Item>> m_items;
};
