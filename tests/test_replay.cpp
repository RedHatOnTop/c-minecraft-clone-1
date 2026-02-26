#include "test_replay.hpp"

#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "entity/EntityManager.hpp"
#include "entity/ItemEntity.hpp"
#include "entity/Mob.hpp"
#include "item/Inventory.hpp"
#include "physics/Player.hpp"
#include "physics/Raycast.hpp"
#include "test_common.hpp"
#include "world/World.hpp"
#include "world/WorldPersistence.hpp"

namespace {
constexpr int kReplayRuns = 3;

using Trace = std::vector<std::string>;

std::string stackSignature(const ItemStack& stack) {
    std::ostringstream oss;
    oss << (int)stack.itemID << ":" << stack.count << ":" << (int)stack.metadata;
    return oss.str();
}

std::string inventorySignature(const Inventory& inventory, int slots) {
    std::ostringstream oss;
    for (int i = 0; i < slots; ++i) {
        if (i != 0) oss << "|";
        oss << stackSignature(inventory.getStack(i));
    }
    return oss.str();
}

std::string format2(float value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();
}

std::string makeReplayPath(const std::string& scenarioName, int runIndex) {
    std::filesystem::path root = std::filesystem::path("tests") / "out" / "replay_v1";
    std::filesystem::path path = root / (scenarioName + "_run_" + std::to_string(runIndex));
    std::error_code ec;
    std::filesystem::remove_all(path, ec);
    std::filesystem::create_directories(path, ec);
    TEST_ASSERT(!ec);
    return path.string();
}

void assertDeterministicTrace(const char* scenarioName, const Trace& expected, const Trace& actual) {
    TEST_ASSERT(actual.size() == expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        if (actual[i] != expected[i]) {
            std::ostringstream oss;
            oss << scenarioName << " trace mismatch at step " << i << ": expected `" << expected[i]
                << "` but got `" << actual[i] << "`";
            throw std::runtime_error(oss.str());
        }
    }
}

Trace runInventoryFlowScenario() {
    Inventory inventory(4);
    Trace trace;

    inventory.setStack(0, ItemStack((ItemID)Blocks::LOG, 2, 0));
    inventory.setStack(1, ItemStack((ItemID)Blocks::COBBLESTONE, 3, 0));
    trace.push_back("seed:" + inventorySignature(inventory, 4));

    inventory.removeStack(0, 1);
    inventory.setStack(2, ItemStack((ItemID)Blocks::PLANKS, 4, 0));
    trace.push_back("craft_log_to_planks:" + inventorySignature(inventory, 4));

    inventory.removeStack(1, 2);
    trace.push_back("consume_cobble:" + inventorySignature(inventory, 4));

    return trace;
}

std::string raycastSignature(World& world) {
    RaycastResult res = Raycaster::raycast(glm::vec3(0.5f, 70.5f, 2.5f), glm::vec3(1.0f, 0.0f, 0.0f), 10.0f, world);
    if (!res.hit) return "none";

    Block block = world.getBlock(res.blockPos.x, res.blockPos.y, res.blockPos.z);
    std::ostringstream oss;
    oss << (int)block.id << "@" << res.blockPos.x << "," << res.blockPos.y << "," << res.blockPos.z;
    return oss.str();
}

Trace runBlockRaycastFlowScenario() {
    World world;
    world.loadChunk(0, 0);

    Trace trace;

    world.setBlock(2, 70, 2, Block(Blocks::STONE));
    trace.push_back("place_stone:" + raycastSignature(world));

    world.setBlock(4, 70, 2, Block(Blocks::DIRT));
    trace.push_back("place_dirt_behind_stone:" + raycastSignature(world));

    world.setBlock(2, 70, 2, Block(Blocks::AIR));
    trace.push_back("break_stone:" + raycastSignature(world));

    return trace;
}

std::string persistenceSignature(const Player& player, const EntityManager& entities) {
    int mobCount = 0;
    int itemEntityCount = 0;
    float mobHealthSum = 0.0f;

    for (const auto& entity : entities.getEntities()) {
        if (const auto* mob = dynamic_cast<const Mob*>(entity.get())) {
            mobCount++;
            mobHealthSum += mob->health;
        } else if (dynamic_cast<const ItemEntity*>(entity.get())) {
            itemEntityCount++;
        }
    }

    std::ostringstream oss;
    oss << "slot=" << player.selectedSlot << "|hp=" << format2(player.health) << "|slot0="
        << stackSignature(player.inventory.getStack(0)) << "|mobs=" << mobCount << "|mob_hp=" << format2(mobHealthSum)
        << "|items=" << itemEntityCount;
    return oss.str();
}

Trace runPersistenceFlowScenario(int runIndex) {
    const std::string worldPath = makeReplayPath("persistence_flow", runIndex);

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

    Trace trace;

    Player loaded1(glm::vec3(0.0f));
    EntityManager loadedEntities1;
    TEST_ASSERT(WorldPersistence::loadPlayerData(worldPath, loaded1).ok());
    TEST_ASSERT(WorldPersistence::loadEntities(worldPath, loadedEntities1).ok());
    trace.push_back("load_1:" + persistenceSignature(loaded1, loadedEntities1));

    for (const auto& entity : loadedEntities1.getEntities()) {
        if (auto* loadedMob = dynamic_cast<Mob*>(entity.get())) {
            loadedMob->health -= 2.0f;
            break;
        }
    }
    loaded1.inventory.removeStack(0, 2);
    TEST_ASSERT(WorldPersistence::savePlayerData(worldPath, loaded1).ok());
    TEST_ASSERT(WorldPersistence::saveEntities(worldPath, loadedEntities1).ok());

    Player loaded2(glm::vec3(0.0f));
    EntityManager loadedEntities2;
    TEST_ASSERT(WorldPersistence::loadPlayerData(worldPath, loaded2).ok());
    TEST_ASSERT(WorldPersistence::loadEntities(worldPath, loadedEntities2).ok());
    trace.push_back("load_2:" + persistenceSignature(loaded2, loadedEntities2));

    loaded2.selectedSlot = 1;
    TEST_ASSERT(WorldPersistence::savePlayerData(worldPath, loaded2).ok());
    Player loaded3(glm::vec3(0.0f));
    EntityManager loadedEntities3;
    TEST_ASSERT(WorldPersistence::loadPlayerData(worldPath, loaded3).ok());
    TEST_ASSERT(WorldPersistence::loadEntities(worldPath, loadedEntities3).ok());
    trace.push_back("load_3:" + persistenceSignature(loaded3, loadedEntities3));

    return trace;
}
}  // namespace

void test_ReplayHarness_InventoryFlow_Deterministic() {
    const Trace expected = {"seed:17:2:0|4:3:0|0:0:0|0:0:0", "craft_log_to_planks:17:1:0|4:3:0|5:4:0|0:0:0",
                            "consume_cobble:17:1:0|4:1:0|5:4:0|0:0:0"};

    for (int run = 0; run < kReplayRuns; ++run) {
        Trace actual = runInventoryFlowScenario();
        assertDeterministicTrace("inventory_flow", expected, actual);
    }
}

void test_ReplayHarness_BlockRaycastFlow_Deterministic() {
    const Trace expected = {"place_stone:1@2,70,2", "place_dirt_behind_stone:1@2,70,2", "break_stone:3@4,70,2"};

    for (int run = 0; run < kReplayRuns; ++run) {
        Trace actual = runBlockRaycastFlowScenario();
        assertDeterministicTrace("block_raycast_flow", expected, actual);
    }
}

void test_ReplayHarness_PersistenceFlow_Deterministic() {
    const Trace expected = {"load_1:slot=3|hp=13.50|slot0=1:12:0|mobs=1|mob_hp=7.25|items=1",
                            "load_2:slot=3|hp=13.50|slot0=1:10:0|mobs=1|mob_hp=5.25|items=1",
                            "load_3:slot=1|hp=13.50|slot0=1:10:0|mobs=1|mob_hp=5.25|items=1"};

    for (int run = 0; run < kReplayRuns; ++run) {
        Trace actual = runPersistenceFlowScenario(run);
        assertDeterministicTrace("persistence_flow", expected, actual);
    }
}
