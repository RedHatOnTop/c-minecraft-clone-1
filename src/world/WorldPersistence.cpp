#include "world/WorldPersistence.hpp"

#include "entity/ItemEntity.hpp"
#include "entity/Mob.hpp"
#include "world/ChestBlockEntity.hpp"
#include "world/FurnaceBlockEntity.hpp"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

namespace {
constexpr uint32_t CHUNK_MAGIC = 0x4B43434D;   // MCCK
constexpr uint32_t PLAYER_MAGIC = 0x4C50434D;  // MCPL
constexpr uint32_t ENTITY_MAGIC = 0x4E45434D;  // MCEN
constexpr uint32_t OLD_BE_MAGIC = 0x30454242;  // BBE0

struct FileHeader {
    uint32_t magic = 0;
    uint16_t version = 0;
    uint16_t flags = 0;
    uint32_t payloadLength = 0;
};

template <typename T>
bool writePod(std::ostream& out, const T& value) {
    out.write(reinterpret_cast<const char*>(&value), sizeof(T));
    return out.good();
}

template <typename T>
bool readPod(std::istream& in, T& value) {
    in.read(reinterpret_cast<char*>(&value), sizeof(T));
    return in.good();
}

bool writeHeader(std::ostream& out, uint32_t magic, uint16_t version, uint16_t flags, uint32_t payloadLength) {
    FileHeader header;
    header.magic = magic;
    header.version = version;
    header.flags = flags;
    header.payloadLength = payloadLength;
    return writePod(out, header.magic) && writePod(out, header.version) && writePod(out, header.flags) &&
           writePod(out, header.payloadLength);
}

bool readHeader(std::istream& in, FileHeader& header) {
    return readPod(in, header.magic) && readPod(in, header.version) && readPod(in, header.flags) &&
           readPod(in, header.payloadLength);
}

bool readBuffer(std::istream& in, std::vector<char>& buffer, size_t size) {
    buffer.resize(size);
    if (size == 0) return true;
    in.read(buffer.data(), static_cast<std::streamsize>(size));
    return in.good();
}

void writeItemStack(std::ostream& out, const ItemStack& st) {
    writePod(out, st.itemID);
    int32_t count = static_cast<int32_t>(st.count);
    int32_t meta = static_cast<int32_t>(st.metadata);
    writePod(out, count);
    writePod(out, meta);
}

bool readItemStack(std::istream& in, ItemStack& outStack) {
    ItemID id = 0;
    int32_t count = 0;
    int32_t meta = 0;
    if (!readPod(in, id) || !readPod(in, count) || !readPod(in, meta)) return false;
    outStack = ItemStack(id, static_cast<int>(count), static_cast<int>(meta));
    return true;
}

bool deserializeKnownBlockEntity(BlockEntityType type, Chunk& chunk, int x, int y, int z, std::istream& in) {
    BlockEntity* be = nullptr;
    if (type == BlockEntityType::Chest) {
        be = chunk.ensureBlockEntity(x, y, z, Blocks::CHEST);
    } else if (type == BlockEntityType::Furnace) {
        be = chunk.ensureBlockEntity(x, y, z, Blocks::FURNACE);
    }
    if (!be) return false;
    be->deserialize(in);
    return in.good();
}
}  // namespace

bool WorldPersistence::ensureWorldPath(const std::string& worldPath) {
    std::error_code ec;
    std::filesystem::create_directories(worldPath, ec);
    return !ec;
}

SaveResult WorldPersistence::saveChunk(const std::string& worldPath, const Chunk& chunk) {
    if (!ensureWorldPath(worldPath)) {
        return {SaveStatusCode::IoError, "Failed to create world directory."};
    }

    std::ostringstream payload(std::ios::binary);
    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        for (int x = 0; x < CHUNK_WIDTH; ++x) {
            for (int z = 0; z < CHUNK_WIDTH; ++z) {
                const Block b = chunk.getBlock(x, y, z);
                writePod(payload, b.id);
                writePod(payload, b.metadata);
                writePod(payload, b.light);
            }
        }
    }

    uint32_t beCount = static_cast<uint32_t>(chunk.getBlockEntities().size());
    writePod(payload, beCount);
    for (const auto& [idx, be] : chunk.getBlockEntities()) {
        int32_t index = idx;
        uint8_t type = static_cast<uint8_t>(be->type());
        std::ostringstream bePayload(std::ios::binary);
        be->serialize(bePayload);
        const std::string bytes = bePayload.str();
        uint32_t payloadLength = static_cast<uint32_t>(bytes.size());

        writePod(payload, index);
        writePod(payload, type);
        writePod(payload, payloadLength);
        payload.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    }

    const std::string payloadBytes = payload.str();
    const uint32_t payloadLength = static_cast<uint32_t>(payloadBytes.size());

    std::ofstream out(getChunkPath(worldPath, chunk.getPosition().x, chunk.getPosition().y), std::ios::binary | std::ios::trunc);
    if (!out.is_open()) {
        return {SaveStatusCode::IoError, "Failed to open chunk file for write."};
    }
    if (!writeHeader(out, CHUNK_MAGIC, static_cast<uint16_t>(SaveFormatVersion::V2), 0, payloadLength)) {
        return {SaveStatusCode::IoError, "Failed to write chunk header."};
    }
    out.write(payloadBytes.data(), static_cast<std::streamsize>(payloadBytes.size()));
    if (!out.good()) {
        return {SaveStatusCode::IoError, "Failed to write chunk payload."};
    }
    return {};
}

LoadResult WorldPersistence::loadChunkV2(std::istream& in, const ChunkFileHeader& header, Chunk& chunk) {
    if (header.version != static_cast<uint16_t>(SaveFormatVersion::V2)) {
        return {LoadStatusCode::UnsupportedVersion, "Unsupported chunk save version."};
    }

    std::vector<char> payload;
    if (!readBuffer(in, payload, header.payloadLength)) {
        return {LoadStatusCode::InvalidData, "Chunk payload truncated."};
    }
    std::istringstream payloadStream(std::string(payload.data(), payload.size()), std::ios::binary);

    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        for (int x = 0; x < CHUNK_WIDTH; ++x) {
            for (int z = 0; z < CHUNK_WIDTH; ++z) {
                Block b;
                if (!readPod(payloadStream, b.id) || !readPod(payloadStream, b.metadata) || !readPod(payloadStream, b.light)) {
                    return {LoadStatusCode::InvalidData, "Block payload truncated."};
                }
                chunk.setBlock(x, y, z, b);
                if (b.id == Blocks::CHEST || b.id == Blocks::FURNACE) {
                    chunk.ensureBlockEntity(x, y, z, b.id);
                } else {
                    chunk.removeBlockEntity(x, y, z);
                }
            }
        }
    }

    uint32_t beCount = 0;
    if (!readPod(payloadStream, beCount)) {
        return {LoadStatusCode::InvalidData, "Missing block-entity count."};
    }

    for (uint32_t i = 0; i < beCount; ++i) {
        int32_t index = 0;
        uint8_t typeValue = 0;
        uint32_t bePayloadLength = 0;
        if (!readPod(payloadStream, index) || !readPod(payloadStream, typeValue) || !readPod(payloadStream, bePayloadLength)) {
            return {LoadStatusCode::InvalidData, "Block-entity header truncated."};
        }
        if (index < 0 || index >= CHUNK_SIZE) {
            return {LoadStatusCode::InvalidData, "Block-entity index out of range."};
        }

        std::vector<char> beBytes;
        if (!readBuffer(payloadStream, beBytes, bePayloadLength)) {
            return {LoadStatusCode::InvalidData, "Block-entity payload truncated."};
        }
        std::istringstream beStream(std::string(beBytes.data(), beBytes.size()), std::ios::binary);

        const int x = index % CHUNK_WIDTH;
        const int y = (index / CHUNK_WIDTH) % CHUNK_HEIGHT;
        const int z = index / (CHUNK_WIDTH * CHUNK_HEIGHT);

        const BlockEntityType type = static_cast<BlockEntityType>(typeValue);
        if (type == BlockEntityType::Chest || type == BlockEntityType::Furnace) {
            if (!deserializeKnownBlockEntity(type, chunk, x, y, z, beStream)) {
                return {LoadStatusCode::InvalidData, "Failed to deserialize known block entity."};
            }
        }
    }

    return {};
}

LoadResult WorldPersistence::loadChunkLegacy(std::istream& in, std::streamoff fileSize, Chunk& chunk) {
    constexpr std::streamoff LEGACY_BLOCKS_V0_BYTES = static_cast<std::streamoff>(CHUNK_SIZE) * 2;
    constexpr std::streamoff LEGACY_BLOCKS_V1_BYTES = static_cast<std::streamoff>(CHUNK_SIZE) * 3;

    if (fileSize < LEGACY_BLOCKS_V0_BYTES) {
        return {LoadStatusCode::InvalidData, "Legacy chunk file is too small."};
    }

    const bool hasLight = fileSize >= LEGACY_BLOCKS_V1_BYTES;
    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        for (int x = 0; x < CHUNK_WIDTH; ++x) {
            for (int z = 0; z < CHUNK_WIDTH; ++z) {
                Block b;
                if (!readPod(in, b.id) || !readPod(in, b.metadata)) {
                    return {LoadStatusCode::InvalidData, "Legacy chunk block stream truncated."};
                }
                if (hasLight) {
                    if (!readPod(in, b.light)) {
                        return {LoadStatusCode::InvalidData, "Legacy chunk light stream truncated."};
                    }
                } else {
                    b.light = 0;
                }
                chunk.setBlock(x, y, z, b);
                if (b.id == Blocks::CHEST || b.id == Blocks::FURNACE) {
                    chunk.ensureBlockEntity(x, y, z, b.id);
                }
            }
        }
    }

    if (hasLight) {
        uint32_t maybeMagic = 0;
        if (readPod(in, maybeMagic) && maybeMagic == OLD_BE_MAGIC) {
            int32_t beCount = 0;
            if (!readPod(in, beCount)) {
                return {LoadStatusCode::InvalidData, "Legacy block-entity count missing."};
            }
            beCount = std::max<int32_t>(0, beCount);
            for (int32_t i = 0; i < beCount; ++i) {
                int32_t index = 0;
                uint8_t typeValue = 0;
                if (!readPod(in, index) || !readPod(in, typeValue)) {
                    return {LoadStatusCode::InvalidData, "Legacy block-entity header truncated."};
                }
                if (index < 0 || index >= CHUNK_SIZE) {
                    return {LoadStatusCode::InvalidData, "Legacy block-entity index out of range."};
                }
                const int x = index % CHUNK_WIDTH;
                const int y = (index / CHUNK_WIDTH) % CHUNK_HEIGHT;
                const int z = index / (CHUNK_WIDTH * CHUNK_HEIGHT);

                const BlockEntityType type = static_cast<BlockEntityType>(typeValue);
                if (type == BlockEntityType::Chest || type == BlockEntityType::Furnace) {
                    if (!deserializeKnownBlockEntity(type, chunk, x, y, z, in)) {
                        return {LoadStatusCode::InvalidData, "Legacy block-entity payload truncated."};
                    }
                } else {
                    return {LoadStatusCode::InvalidData, "Legacy block-entity type is unknown and cannot be skipped."};
                }
            }
        }
    }

    return {LoadStatusCode::MigratedLegacy, "Legacy chunk data loaded and marked for migration."};
}

LoadResult WorldPersistence::loadChunk(const std::string& worldPath, Chunk& chunk) {
    std::ifstream in(getChunkPath(worldPath, chunk.getPosition().x, chunk.getPosition().y), std::ios::binary);
    if (!in.is_open()) {
        return {LoadStatusCode::NotFound, "Chunk file does not exist."};
    }

    in.seekg(0, std::ios::end);
    const std::streamoff fileSize = in.tellg();
    in.seekg(0, std::ios::beg);

    ChunkFileHeader header{};
    if (readPod(in, header.magic) && readPod(in, header.version) && readPod(in, header.flags) && readPod(in, header.payloadLength) &&
        header.magic == CHUNK_MAGIC) {
        return loadChunkV2(in, header, chunk);
    }

    in.clear();
    in.seekg(0, std::ios::beg);
    return loadChunkLegacy(in, fileSize, chunk);
}

SaveResult WorldPersistence::savePlayerData(const std::string& worldPath, const Player& player) {
    if (!ensureWorldPath(worldPath)) {
        return {SaveStatusCode::IoError, "Failed to create world directory."};
    }

    std::ostringstream payload(std::ios::binary);
    writePod(payload, player.position);
    writePod(payload, player.selectedSlot);
    writePod(payload, player.health);

    int32_t slotCount = static_cast<int32_t>(player.inventory.size());
    writePod(payload, slotCount);
    for (int i = 0; i < slotCount; ++i) {
        writeItemStack(payload, player.inventory.getStack(i));
    }

    const std::string payloadBytes = payload.str();
    std::ofstream out(getPlayerPath(worldPath), std::ios::binary | std::ios::trunc);
    if (!out.is_open()) {
        return {SaveStatusCode::IoError, "Failed to open player file for write."};
    }
    if (!writeHeader(out, PLAYER_MAGIC, static_cast<uint16_t>(SaveFormatVersion::V2), 0,
                     static_cast<uint32_t>(payloadBytes.size()))) {
        return {SaveStatusCode::IoError, "Failed to write player header."};
    }
    out.write(payloadBytes.data(), static_cast<std::streamsize>(payloadBytes.size()));
    if (!out.good()) {
        return {SaveStatusCode::IoError, "Failed to write player payload."};
    }
    return {};
}

LoadResult WorldPersistence::loadPlayerData(const std::string& worldPath, Player& player) {
    std::ifstream in(getPlayerPath(worldPath), std::ios::binary);
    if (!in.is_open()) {
        return {LoadStatusCode::NotFound, "Player file does not exist."};
    }

    FileHeader header{};
    if (readHeader(in, header) && header.magic == PLAYER_MAGIC && header.version == static_cast<uint16_t>(SaveFormatVersion::V2)) {
        std::vector<char> payload;
        if (!readBuffer(in, payload, header.payloadLength)) {
            return {LoadStatusCode::InvalidData, "Player payload truncated."};
        }
        std::istringstream data(std::string(payload.data(), payload.size()), std::ios::binary);
        if (!readPod(data, player.position) || !readPod(data, player.selectedSlot) || !readPod(data, player.health)) {
            return {LoadStatusCode::InvalidData, "Player core payload truncated."};
        }
        int32_t slotCount = 0;
        if (!readPod(data, slotCount)) {
            return {LoadStatusCode::InvalidData, "Player slot count missing."};
        }
        slotCount = std::max<int32_t>(0, slotCount);
        for (int i = 0; i < slotCount && i < player.inventory.size(); ++i) {
            ItemStack st;
            if (!readItemStack(data, st)) {
                return {LoadStatusCode::InvalidData, "Player inventory payload truncated."};
            }
            player.inventory.setStack(i, st);
        }
        return {};
    }

    // Legacy fallback
    in.clear();
    in.seekg(0, std::ios::beg);

    if (!readPod(in, player.position) || !readPod(in, player.selectedSlot) || !readPod(in, player.health)) {
        return {LoadStatusCode::InvalidData, "Legacy player payload truncated."};
    }
    int32_t slotCount = 0;
    if (!readPod(in, slotCount)) {
        return {LoadStatusCode::InvalidData, "Legacy player slot count missing."};
    }
    slotCount = std::max<int32_t>(0, slotCount);
    for (int i = 0; i < slotCount && i < player.inventory.size(); ++i) {
        ItemStack st;
        if (!readItemStack(in, st)) {
            return {LoadStatusCode::InvalidData, "Legacy player inventory payload truncated."};
        }
        player.inventory.setStack(i, st);
    }
    return {LoadStatusCode::MigratedLegacy, "Legacy player data loaded and marked for migration."};
}

SaveResult WorldPersistence::saveEntities(const std::string& worldPath, const EntityManager& entities) {
    if (!ensureWorldPath(worldPath)) {
        return {SaveStatusCode::IoError, "Failed to create world directory."};
    }

    std::ostringstream payload(std::ios::binary);
    int32_t count = 0;
    for (const auto& e : entities.getEntities()) {
        if (e->removed) continue;
        if (dynamic_cast<Mob*>(e.get()) || dynamic_cast<ItemEntity*>(e.get())) ++count;
    }
    writePod(payload, count);

    for (const auto& e : entities.getEntities()) {
        if (e->removed) continue;

        if (const auto* mob = dynamic_cast<Mob*>(e.get())) {
            uint8_t type = 1;
            writePod(payload, type);
            writePod(payload, mob->position);
            writePod(payload, mob->health);
            uint32_t len = static_cast<uint32_t>(mob->type.size());
            writePod(payload, len);
            payload.write(mob->type.data(), static_cast<std::streamsize>(len));
        } else if (const auto* item = dynamic_cast<ItemEntity*>(e.get())) {
            uint8_t type = 2;
            writePod(payload, type);
            writePod(payload, item->position);
            writePod(payload, item->stack.itemID);
            int32_t c = static_cast<int32_t>(item->stack.count);
            int32_t m = static_cast<int32_t>(item->stack.metadata);
            writePod(payload, c);
            writePod(payload, m);
        }
    }

    const std::string payloadBytes = payload.str();
    std::ofstream out(getEntitiesPath(worldPath), std::ios::binary | std::ios::trunc);
    if (!out.is_open()) {
        return {SaveStatusCode::IoError, "Failed to open entities file for write."};
    }
    if (!writeHeader(out, ENTITY_MAGIC, static_cast<uint16_t>(SaveFormatVersion::V2), 0,
                     static_cast<uint32_t>(payloadBytes.size()))) {
        return {SaveStatusCode::IoError, "Failed to write entity header."};
    }
    out.write(payloadBytes.data(), static_cast<std::streamsize>(payloadBytes.size()));
    if (!out.good()) {
        return {SaveStatusCode::IoError, "Failed to write entity payload."};
    }
    return {};
}

LoadResult WorldPersistence::loadEntities(const std::string& worldPath, EntityManager& entities) {
    std::ifstream in(getEntitiesPath(worldPath), std::ios::binary);
    if (!in.is_open()) {
        return {LoadStatusCode::NotFound, "Entity file does not exist."};
    }

    auto loadPayload = [&](std::istream& stream) -> LoadResult {
        int32_t count = 0;
        if (!readPod(stream, count)) {
            return {LoadStatusCode::InvalidData, "Entity count missing."};
        }
        count = std::max<int32_t>(0, count);

        for (int32_t i = 0; i < count; ++i) {
            uint8_t type = 0;
            if (!readPod(stream, type)) {
                return {LoadStatusCode::InvalidData, "Entity type missing."};
            }
            if (type == 1) {
                glm::vec3 pos{};
                float health = 20.0f;
                uint32_t len = 0;
                if (!readPod(stream, pos) || !readPod(stream, health) || !readPod(stream, len)) {
                    return {LoadStatusCode::InvalidData, "Mob payload truncated."};
                }
                std::string mobType;
                mobType.resize(len);
                stream.read(mobType.data(), static_cast<std::streamsize>(len));
                if (!stream.good()) {
                    return {LoadStatusCode::InvalidData, "Mob type payload truncated."};
                }
                auto mob = std::make_shared<Mob>(pos, mobType);
                mob->health = health;
                entities.addEntity(mob);
            } else if (type == 2) {
                glm::vec3 pos{};
                ItemID id = 0;
                int32_t c = 0;
                int32_t m = 0;
                if (!readPod(stream, pos) || !readPod(stream, id) || !readPod(stream, c) || !readPod(stream, m)) {
                    return {LoadStatusCode::InvalidData, "Item entity payload truncated."};
                }
                entities.addEntity(std::make_shared<ItemEntity>(pos, ItemStack(id, static_cast<int>(c), static_cast<int>(m))));
            } else {
                return {LoadStatusCode::InvalidData, "Unknown entity type in payload."};
            }
        }
        return {};
    };

    FileHeader header{};
    if (readHeader(in, header) && header.magic == ENTITY_MAGIC && header.version == static_cast<uint16_t>(SaveFormatVersion::V2)) {
        std::vector<char> payload;
        if (!readBuffer(in, payload, header.payloadLength)) {
            return {LoadStatusCode::InvalidData, "Entity payload truncated."};
        }
        std::istringstream payloadStream(std::string(payload.data(), payload.size()), std::ios::binary);
        return loadPayload(payloadStream);
    }

    in.clear();
    in.seekg(0, std::ios::beg);
    LoadResult legacy = loadPayload(in);
    if (!legacy.ok()) return legacy;
    legacy.code = LoadStatusCode::MigratedLegacy;
    legacy.detail = "Legacy entity data loaded and marked for migration.";
    return legacy;
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
