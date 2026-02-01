#pragma once

#include <string>
#include "world/Chunk.hpp"
#include "physics/Player.hpp"
#include "entity/EntityManager.hpp"

class WorldPersistence {
public:
    static bool saveChunk(const std::string& worldPath, const Chunk& chunk);
    static bool loadChunk(const std::string& worldPath, Chunk& chunk);

    static bool savePlayerData(const std::string& worldPath, const Player& player);
    static bool loadPlayerData(const std::string& worldPath, Player& player);

    static bool saveEntities(const std::string& worldPath, const EntityManager& entities);
    static bool loadEntities(const std::string& worldPath, EntityManager& entities);

private:
    static std::string getChunkPath(const std::string& worldPath, int x, int z);
    static std::string getPlayerPath(const std::string& worldPath);
    static std::string getEntitiesPath(const std::string& worldPath);
};
