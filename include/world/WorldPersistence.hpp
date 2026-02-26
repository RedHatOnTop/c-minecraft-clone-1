#pragma once

#include <string>
#include <cstdint>
#include <istream>
#include "world/Chunk.hpp"
#include "physics/Player.hpp"
#include "entity/EntityManager.hpp"

enum class SaveFormatVersion : uint16_t {
    V2 = 2
};

struct ChunkFileHeader {
    uint32_t magic;
    uint16_t version;
    uint16_t flags;
    uint32_t payloadLength;
};

enum class SaveStatusCode : uint8_t {
    Ok = 0,
    NotFound,
    IoError,
    InvalidData,
    UnsupportedVersion
};

enum class LoadStatusCode : uint8_t {
    Ok = 0,
    NotFound,
    IoError,
    InvalidData,
    UnsupportedVersion,
    MigratedLegacy
};

struct SaveResult {
    SaveStatusCode code = SaveStatusCode::Ok;
    std::string detail;

    bool ok() const { return code == SaveStatusCode::Ok; }
};

struct LoadResult {
    LoadStatusCode code = LoadStatusCode::Ok;
    std::string detail;

    bool ok() const {
        return code == LoadStatusCode::Ok || code == LoadStatusCode::MigratedLegacy;
    }

    bool migratedLegacy() const {
        return code == LoadStatusCode::MigratedLegacy;
    }
};

class WorldPersistence {
public:
    static SaveResult saveChunk(const std::string& worldPath, const Chunk& chunk);
    static LoadResult loadChunk(const std::string& worldPath, Chunk& chunk);

    static SaveResult savePlayerData(const std::string& worldPath, const Player& player);
    static LoadResult loadPlayerData(const std::string& worldPath, Player& player);

    static SaveResult saveEntities(const std::string& worldPath, const EntityManager& entities);
    static LoadResult loadEntities(const std::string& worldPath, EntityManager& entities);

private:
    static bool ensureWorldPath(const std::string& worldPath);
    static LoadResult loadChunkLegacy(std::istream& in, std::streamoff fileSize, Chunk& chunk);
    static LoadResult loadChunkV2(std::istream& in, const ChunkFileHeader& header, Chunk& chunk);

    static std::string getChunkPath(const std::string& worldPath, int x, int z);
    static std::string getPlayerPath(const std::string& worldPath);
    static std::string getEntitiesPath(const std::string& worldPath);
};
