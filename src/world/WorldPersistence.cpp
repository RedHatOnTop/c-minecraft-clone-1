#include "world/WorldPersistence.hpp"
#include "entity/Mob.hpp"
#include "entity/ItemEntity.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cstdint>

bool WorldPersistence::saveChunk(const std::string& worldPath, const Chunk& chunk) {
    std::string path = getChunkPath(worldPath, chunk.getPosition().x, chunk.getPosition().y);
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) return false;

    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        for (int x = 0; x < CHUNK_WIDTH; ++x) {
            for (int z = 0; z < CHUNK_WIDTH; ++z) {
                Block b = chunk.getBlock(x, y, z);
                out.write((char*)&b.id, sizeof(b.id));
                out.write((char*)&b.metadata, sizeof(b.metadata));
            }
        }
    }
    out.close();
    return true;
}

bool WorldPersistence::loadChunk(const std::string& worldPath, Chunk& chunk) {
    std::string path = getChunkPath(worldPath, chunk.getPosition().x, chunk.getPosition().y);
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) return false;

    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        for (int x = 0; x < CHUNK_WIDTH; ++x) {
            for (int z = 0; z < CHUNK_WIDTH; ++z) {
                Block b;
                in.read((char*)&b.id, sizeof(b.id));
                in.read((char*)&b.metadata, sizeof(b.metadata));
                chunk.setBlock(x, y, z, b);
            }
        }
    }
    in.close();
    return true;
}

bool WorldPersistence::savePlayerData(const std::string& worldPath, const Player& player) {
    std::string path = getPlayerPath(worldPath);
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) return false;

    out.write((char*)&player.position, sizeof(player.position));
    out.write((char*)&player.selectedSlot, sizeof(player.selectedSlot));
    out.write((char*)&player.health, sizeof(player.health));
    int32_t slotCount = (int32_t)player.inventory.size();
    out.write((char*)&slotCount, sizeof(slotCount));
    for (int i = 0; i < slotCount; ++i) {
        ItemStack st = player.inventory.getStack(i);
        out.write((char*)&st.itemID, sizeof(st.itemID));
        int32_t count = (int32_t)st.count;
        int32_t meta = (int32_t)st.metadata;
        out.write((char*)&count, sizeof(count));
        out.write((char*)&meta, sizeof(meta));
    }
    out.close();
    return true;
}

bool WorldPersistence::loadPlayerData(const std::string& worldPath, Player& player) {
    std::string path = getPlayerPath(worldPath);
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) return false;

    in.read((char*)&player.position, sizeof(player.position));
    in.read((char*)&player.selectedSlot, sizeof(player.selectedSlot));
    in.read((char*)&player.health, sizeof(player.health));
    int32_t slotCount = 0;
    in.read((char*)&slotCount, sizeof(slotCount));
    slotCount = std::max<int32_t>(0, slotCount);
    for (int i = 0; i < slotCount && i < player.inventory.size(); ++i) {
        ItemID id = 0;
        int32_t count = 0;
        int32_t meta = 0;
        in.read((char*)&id, sizeof(id));
        in.read((char*)&count, sizeof(count));
        in.read((char*)&meta, sizeof(meta));
        player.inventory.setStack(i, ItemStack(id, (int)count, (int)meta));
    }
    in.close();
    return true;
}

bool WorldPersistence::saveEntities(const std::string& worldPath, const EntityManager& entities) {
    std::string path = getEntitiesPath(worldPath);
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) return false;

    int32_t count = 0;
    for (auto& e : entities.getEntities()) {
        if (e->removed) continue;
        if (dynamic_cast<Mob*>(e.get()) || dynamic_cast<ItemEntity*>(e.get())) count++;
    }
    out.write((char*)&count, sizeof(count));

    for (auto& e : entities.getEntities()) {
        if (e->removed) continue;

        if (auto* mob = dynamic_cast<Mob*>(e.get())) {
            uint8_t type = 1;
            out.write((char*)&type, sizeof(type));
            out.write((char*)&mob->position, sizeof(mob->position));
            out.write((char*)&mob->health, sizeof(mob->health));
            uint32_t len = (uint32_t)mob->type.size();
            out.write((char*)&len, sizeof(len));
            out.write(mob->type.data(), len);
        } else if (auto* item = dynamic_cast<ItemEntity*>(e.get())) {
            uint8_t type = 2;
            out.write((char*)&type, sizeof(type));
            out.write((char*)&item->position, sizeof(item->position));
            out.write((char*)&item->stack.itemID, sizeof(item->stack.itemID));
            int32_t c = (int32_t)item->stack.count;
            int32_t m = (int32_t)item->stack.metadata;
            out.write((char*)&c, sizeof(c));
            out.write((char*)&m, sizeof(m));
        }
    }

    out.close();
    return true;
}

bool WorldPersistence::loadEntities(const std::string& worldPath, EntityManager& entities) {
    std::string path = getEntitiesPath(worldPath);
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) return false;

    int32_t count = 0;
    in.read((char*)&count, sizeof(count));
    count = std::max<int32_t>(0, count);

    for (int32_t i = 0; i < count; ++i) {
        uint8_t type = 0;
        in.read((char*)&type, sizeof(type));
        if (type == 1) {
            glm::vec3 pos;
            float health;
            in.read((char*)&pos, sizeof(pos));
            in.read((char*)&health, sizeof(health));
            uint32_t len = 0;
            in.read((char*)&len, sizeof(len));
            std::string mobType;
            mobType.resize(len);
            in.read(mobType.data(), len);
            auto mob = std::make_shared<Mob>(pos, mobType);
            mob->health = health;
            entities.addEntity(mob);
        } else if (type == 2) {
            glm::vec3 pos;
            ItemID id;
            int32_t c, m;
            in.read((char*)&pos, sizeof(pos));
            in.read((char*)&id, sizeof(id));
            in.read((char*)&c, sizeof(c));
            in.read((char*)&m, sizeof(m));
            entities.addEntity(std::make_shared<ItemEntity>(pos, ItemStack(id, (int)c, (int)m)));
        }
    }

    in.close();
    return true;
}

std::string WorldPersistence::getChunkPath(const std::string& worldPath, int x, int z) {
    return worldPath + "/chunk_" + std::to_string(x) + "_" + std::to_string(z) + ".dat";
}

std::string WorldPersistence::getPlayerPath(const std::string& worldPath) {
    return worldPath + "/player.dat";
}

std::string WorldPersistence::getEntitiesPath(const std::string& worldPath) {
    return worldPath + "/entities.dat";
}
