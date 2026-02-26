#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>

#include "entity/EntityManager.hpp"
#include "entity/ItemEntity.hpp"
#include "entity/Mob.hpp"
#include "physics/Player.hpp"
#include "test_common.hpp"
#include "world/ChestBlockEntity.hpp"
#include "world/Chunk.hpp"
#include "world/WorldPersistence.hpp"

static std::string makeWorldPath(const std::string& name) {
    std::filesystem::path p = std::filesystem::path("tests") / "out" / name;
    std::error_code ec;
    std::filesystem::remove_all(p, ec);
    std::filesystem::create_directories(p, ec);
    TEST_ASSERT(!ec);
    return p.string();
}

static void writeLegacyChunkV0(const std::string& worldPath, const Chunk& chunk) {
    std::filesystem::create_directories(worldPath);
    const std::string path = worldPath + "/chunk_" + std::to_string(chunk.getPosition().x) + "_" +
                             std::to_string(chunk.getPosition().y) + ".dat";
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    TEST_ASSERT(out.is_open());
    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        for (int x = 0; x < CHUNK_WIDTH; ++x) {
            for (int z = 0; z < CHUNK_WIDTH; ++z) {
                Block b = chunk.getBlock(x, y, z);
                out.write((const char*)&b.id, sizeof(b.id));
                out.write((const char*)&b.metadata, sizeof(b.metadata));
            }
        }
    }
}

void test_DataMigration_ChunkLegacyToV2_RoundTrip() {
    const std::string worldPath = makeWorldPath("chunk_migration");

    Chunk legacy(0, 0);
    legacy.setBlock(1, 40, 1, Block(Blocks::CHEST, 2));
    legacy.setBlock(2, 40, 1, Block(Blocks::STONE, 0));
    writeLegacyChunkV0(worldPath, legacy);

    Chunk loaded(0, 0);
    LoadResult lr = WorldPersistence::loadChunk(worldPath, loaded);
    TEST_ASSERT(lr.ok());
    TEST_ASSERT(lr.migratedLegacy());
    TEST_ASSERT(loaded.getBlock(1, 40, 1).id == Blocks::CHEST);

    SaveResult sr = WorldPersistence::saveChunk(worldPath, loaded);
    TEST_ASSERT(sr.ok());

    Chunk reloaded(0, 0);
    LoadResult reloadedResult = WorldPersistence::loadChunk(worldPath, reloaded);
    TEST_ASSERT(reloadedResult.ok());
    TEST_ASSERT(!reloadedResult.migratedLegacy());
    TEST_ASSERT(reloaded.getBlock(1, 40, 1).id == Blocks::CHEST);
    TEST_ASSERT(reloaded.getBlock(2, 40, 1).id == Blocks::STONE);
}

void test_DataMigration_PlayerEntity_RoundTrip() {
    const std::string worldPath = makeWorldPath("player_entity_roundtrip");

    Player player(glm::vec3(1.0f, 80.0f, -3.0f));
    player.selectedSlot = 3;
    player.health = 13.5f;
    player.inventory.setStack(0, ItemStack((ItemID)Blocks::STONE, 12, 0));

    EntityManager entities;
    auto mob = std::make_shared<Mob>(glm::vec3(3.0f, 80.0f, 3.0f), "zombie");
    mob->health = 7.25f;
    entities.addEntity(mob);
    entities.addEntity(std::make_shared<ItemEntity>(glm::vec3(2.0f, 81.0f, 2.0f), ItemStack((ItemID)Blocks::DIRT, 5, 0)));

    TEST_ASSERT(WorldPersistence::savePlayerData(worldPath, player).ok());
    TEST_ASSERT(WorldPersistence::saveEntities(worldPath, entities).ok());

    Player loadedPlayer(glm::vec3(0.0f));
    EntityManager loadedEntities;
    LoadResult lp = WorldPersistence::loadPlayerData(worldPath, loadedPlayer);
    LoadResult le = WorldPersistence::loadEntities(worldPath, loadedEntities);
    TEST_ASSERT(lp.ok());
    TEST_ASSERT(le.ok());

    TEST_ASSERT(loadedPlayer.selectedSlot == 3);
    TEST_ASSERT(std::abs(loadedPlayer.health - 13.5f) < 0.0001f);
    TEST_ASSERT(loadedPlayer.inventory.getStack(0).itemID == (ItemID)Blocks::STONE);
    TEST_ASSERT(loadedPlayer.inventory.getStack(0).count == 12);

    TEST_ASSERT(loadedEntities.getEntities().size() == 2);
}

void test_Entity_MobHealthPersistence() {
    const std::string worldPath = makeWorldPath("mob_health");

    EntityManager entities;
    auto mob = std::make_shared<Mob>(glm::vec3(7.0f, 70.0f, 7.0f), "zombie");
    mob->health = 4.5f;
    entities.addEntity(mob);

    TEST_ASSERT(WorldPersistence::saveEntities(worldPath, entities).ok());

    EntityManager loaded;
    LoadResult lr = WorldPersistence::loadEntities(worldPath, loaded);
    TEST_ASSERT(lr.ok());

    bool found = false;
    for (const auto& e : loaded.getEntities()) {
        if (auto* loadedMob = dynamic_cast<Mob*>(e.get())) {
            found = true;
            TEST_ASSERT(std::abs(loadedMob->health - 4.5f) < 0.0001f);
        }
    }
    TEST_ASSERT(found);
}
