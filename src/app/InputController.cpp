#include "app/InputController.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "entity/Entity.hpp"
#include "entity/ItemEntity.hpp"
#include "entity/Mob.hpp"
#include "item/ItemStack.hpp"
#include "physics/AABB.hpp"
#include "physics/Raycast.hpp"
#include "util/SoundManager.hpp"
#include "world/BlockEntity.hpp"
#include "world/ChestBlockEntity.hpp"
#include "world/Chunk.hpp"
#include "world/FurnaceBlockEntity.hpp"
#include "world/WorldPersistence.hpp"

namespace {
int chunkLocalCoord(int worldCoord) {
    int v = worldCoord % CHUNK_WIDTH;
    if (v < 0) v += CHUNK_WIDTH;
    return v;
}

bool isGameplayInputEnabled(const GameContext& ctx) {
    return ctx.gameMode == GameMode::Playing && !ctx.inventoryOpen && !ctx.openedContainer.active;
}

void closeOpenedContainer(GameContext& ctx, GLFWwindow* window) {
    ctx.openedContainer = {};
    if (ctx.gameMode == GameMode::Playing && !ctx.inventoryOpen) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        ctx.firstMouse = true;
    }
}

BlockEntity* queryOpenedBlockEntity(GameContext& ctx) {
    if (!ctx.openedContainer.active) return nullptr;

    const glm::ivec3 pos = ctx.openedContainer.blockPos;
    const Block current = ctx.world.getBlock(pos.x, pos.y, pos.z);
    if (current.id != ctx.openedContainer.blockId) return nullptr;

    const int cx = (int)std::floor((float)pos.x / CHUNK_WIDTH);
    const int cz = (int)std::floor((float)pos.z / CHUNK_WIDTH);
    Chunk* chunk = ctx.world.getChunk(cx, cz);
    if (!chunk) return nullptr;

    const int bx = chunkLocalCoord(pos.x);
    const int bz = chunkLocalCoord(pos.z);
    return chunk->getBlockEntity(bx, pos.y, bz);
}

void logSaveResult(const char* label, const SaveResult& res) {
    if (!res.ok()) {
        std::cerr << "[Save] " << label << ": " << res.detail << "\n";
    }
}

void logLoadResult(const char* label, const LoadResult& res) {
    if (!res.ok()) {
        std::cerr << "[Load] " << label << ": " << res.detail << "\n";
        return;
    }
    if (res.migratedLegacy()) {
        std::cout << "[Load] " << label << ": " << res.detail << "\n";
    }
}

ItemID getDropItem(BlockID id) {
    switch (id) {
        case Blocks::GRASS:
            return (ItemID)Blocks::DIRT;
        case Blocks::STONE:
            return (ItemID)Blocks::STONE;
        case Blocks::DIRT:
            return (ItemID)Blocks::DIRT;
        case Blocks::LOG:
            return (ItemID)Blocks::LOG;
        case Blocks::LEAVES:
            return (ItemID)Blocks::LEAVES;
        default:
            return 0;
    }
}
}  // namespace

namespace app {
bool isUIOpen(const GameContext& ctx) {
    return ctx.gameMode != GameMode::Playing || ctx.inventoryOpen || ctx.openedContainer.active;
}

void ensureOpenedContainerValid(GameContext& ctx, GLFWwindow* window) {
    if (!ctx.openedContainer.active) return;
    BlockEntity* be = queryOpenedBlockEntity(ctx);
    if (!be) {
        closeOpenedContainer(ctx, window);
        return;
    }

    if (ctx.openedContainer.blockId == Blocks::CHEST && !dynamic_cast<ChestBlockEntity*>(be)) {
        closeOpenedContainer(ctx, window);
        return;
    }
    if (ctx.openedContainer.blockId == Blocks::FURNACE && !dynamic_cast<FurnaceBlockEntity*>(be)) {
        closeOpenedContainer(ctx, window);
    }
}

ChestBlockEntity* getOpenChest(GameContext& ctx) {
    if (!ctx.openedContainer.active || ctx.openedContainer.blockId != Blocks::CHEST) return nullptr;
    return dynamic_cast<ChestBlockEntity*>(queryOpenedBlockEntity(ctx));
}

FurnaceBlockEntity* getOpenFurnace(GameContext& ctx) {
    if (!ctx.openedContainer.active || ctx.openedContainer.blockId != Blocks::FURNACE) return nullptr;
    return dynamic_cast<FurnaceBlockEntity*>(queryOpenedBlockEntity(ctx));
}

void processInput(GameContext& ctx, GLFWwindow* window) {
    ensureOpenedContainerValid(ctx, window);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (!ctx.escDown) {
            if (ctx.inventoryOpen || ctx.openedContainer.active) {
                ctx.inventoryOpen = false;
                closeOpenedContainer(ctx, window);
            } else if (ctx.gameMode == GameMode::Playing) {
                ctx.gameMode = GameMode::Paused;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            } else if (ctx.gameMode == GameMode::Paused) {
                ctx.gameMode = GameMode::MainMenu;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            } else {
                glfwSetWindowShouldClose(window, true);
            }
            ctx.escDown = true;
        }
    } else {
        ctx.escDown = false;
    }

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (!ctx.enterDown) {
            if (ctx.gameMode == GameMode::MainMenu || ctx.gameMode == GameMode::Paused) {
                ctx.gameMode = GameMode::Playing;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                ctx.firstMouse = true;
            }
            ctx.enterDown = true;
        }
    } else {
        ctx.enterDown = false;
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (!ctx.eDown) {
            if (ctx.gameMode == GameMode::Playing) {
                if (ctx.openedContainer.active) {
                    closeOpenedContainer(ctx, window);
                } else {
                    ctx.inventoryOpen = !ctx.inventoryOpen;
                    glfwSetInputMode(window, GLFW_CURSOR, ctx.inventoryOpen ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
                    if (!ctx.inventoryOpen) ctx.firstMouse = true;
                }
            }
            ctx.eDown = true;
        }
    } else {
        ctx.eDown = false;
    }

    if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
        if (!ctx.f5Down) {
            for (const auto& [pos, chunk] : ctx.world.getChunks()) {
                (void)pos;
                logSaveResult("chunk", WorldPersistence::saveChunk(ctx.worldPath, *chunk));
            }
            logSaveResult("player", WorldPersistence::savePlayerData(ctx.worldPath, *ctx.player));
            logSaveResult("entities", WorldPersistence::saveEntities(ctx.worldPath, ctx.entityManager));
            ctx.f5Down = true;
        }
    } else {
        ctx.f5Down = false;
    }

    if (glfwGetKey(window, GLFW_KEY_F9) == GLFW_PRESS) {
        if (!ctx.f9Down) {
            logLoadResult("player", WorldPersistence::loadPlayerData(ctx.worldPath, *ctx.player));
            ctx.entityManager.clear();
            logLoadResult("entities", WorldPersistence::loadEntities(ctx.worldPath, ctx.entityManager));
            ctx.f9Down = true;
        }
    } else {
        ctx.f9Down = false;
    }

    if (glfwGetKey(window, GLFW_KEY_F6) == GLFW_PRESS) {
        if (!ctx.f6Down) {
            ItemStack chest((ItemID)Blocks::CHEST, 1, 0);
            ItemStack furnace((ItemID)Blocks::FURNACE, 1, 0);
            ItemStack cobble((ItemID)Blocks::COBBLESTONE, 16, 0);
            ctx.player->addToInventory(chest);
            ctx.player->addToInventory(furnace);
            ctx.player->addToInventory(cobble);
            ctx.f6Down = true;
        }
    } else {
        ctx.f6Down = false;
    }
}

void handleBlockInteraction(GameContext& ctx, GLFWwindow* window) {
    if (!isUIOpen(ctx) && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!ctx.leftMouseDown) {
            auto rayIntersectsAABB = [](const glm::vec3& ro, const glm::vec3& rd, const AABB& box, float& tOut) -> bool {
                float tmin = 0.0f;
                float tmax = 5.0f;
                for (int axis = 0; axis < 3; ++axis) {
                    float origin = ro[axis];
                    float dir = rd[axis];
                    float minB = box.min[axis];
                    float maxB = box.max[axis];
                    if (std::abs(dir) < 1e-6f) {
                        if (origin < minB || origin > maxB) return false;
                    } else {
                        float ood = 1.0f / dir;
                        float t1 = (minB - origin) * ood;
                        float t2 = (maxB - origin) * ood;
                        if (t1 > t2) std::swap(t1, t2);
                        tmin = std::max(tmin, t1);
                        tmax = std::min(tmax, t2);
                        if (tmin > tmax) return false;
                    }
                }
                tOut = tmin;
                return true;
            };

            float closestT = 9999.0f;
            std::shared_ptr<Entity> closestEntity;
            for (auto& e : ctx.entityManager.getEntities()) {
                if (e->removed) continue;
                Mob* mob = dynamic_cast<Mob*>(e.get());
                if (!mob) continue;
                float tHit = 0.0f;
                if (rayIntersectsAABB(ctx.player->camera.Position, glm::normalize(ctx.player->camera.Front), mob->getAABB(), tHit)) {
                    if (tHit < closestT) {
                        closestT = tHit;
                        closestEntity = e;
                    }
                }
            }

            if (closestEntity) {
                closestEntity->health -= 4.0f;
                if (closestEntity->health <= 0.0f) closestEntity->markRemoved();
            } else {
                RaycastResult res = Raycaster::raycast(ctx.player->camera.Position, ctx.player->camera.Front, 5.0f, ctx.world);
                if (res.hit) {
                    Block hitBlock = ctx.world.getBlock(res.blockPos.x, res.blockPos.y, res.blockPos.z);
                    ItemID drop = getDropItem(hitBlock.id);
                    if (drop != 0) {
                        ctx.entityManager.addEntity(std::make_shared<ItemEntity>(
                            glm::vec3(res.blockPos) + glm::vec3(0.5f, 0.5f, 0.5f), ItemStack(drop, 1, 0)));
                    }
                    ctx.world.setBlock(res.blockPos.x, res.blockPos.y, res.blockPos.z, Block(Blocks::AIR));
                    SoundManager::playSound("break");
                }
            }
            ctx.leftMouseDown = true;
        }
    } else {
        ctx.leftMouseDown = false;
    }

    if (!isUIOpen(ctx) && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (!ctx.rightMouseDown) {
            RaycastResult res = Raycaster::raycast(ctx.player->camera.Position, ctx.player->camera.Front, 5.0f, ctx.world);
            if (res.hit) {
                Block hitBlock = ctx.world.getBlock(res.blockPos.x, res.blockPos.y, res.blockPos.z);
                bool consumed = false;

                if (hitBlock.id == Blocks::CHEST || hitBlock.id == Blocks::FURNACE) {
                    ctx.openedContainer.active = true;
                    ctx.openedContainer.blockPos = res.blockPos;
                    ctx.openedContainer.blockId = hitBlock.id;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    consumed = true;
                }

                if (!consumed) {
                    glm::ivec3 placePos = res.blockPos + res.faceNormal;
                    AABB playerAABB = ctx.player->getAABB();
                    AABB blockAABB(glm::vec3(placePos), glm::vec3(placePos) + glm::vec3(1.0f));
                    if (!playerAABB.intersects(blockAABB)) {
                        ItemStack held = ctx.player->inventory.getStack(ctx.player->selectedSlot);
                        BlockID placeId = Blocks::STONE;
                        if (!held.isEmpty()) {
                            placeId = (BlockID)held.itemID;
                            if (placeId != Blocks::STONE && placeId != Blocks::DIRT && placeId != Blocks::GRASS &&
                                placeId != Blocks::LOG && placeId != Blocks::LEAVES && placeId != Blocks::CHEST &&
                                placeId != Blocks::FURNACE && placeId != Blocks::COBBLESTONE && placeId != Blocks::PLANKS) {
                                placeId = Blocks::STONE;
                            }
                        }

                        uint8_t meta = 0;
                        if (placeId == Blocks::CHEST || placeId == Blocks::FURNACE) {
                            float yaw = ctx.player->camera.Yaw + 90.0f;
                            while (yaw < 0.0f) yaw += 360.0f;
                            while (yaw >= 360.0f) yaw -= 360.0f;
                            meta = (uint8_t)(((int)std::floor((yaw + 45.0f) / 90.0f)) & 3);
                        }

                        ctx.world.setBlock(placePos.x, placePos.y, placePos.z, Block(placeId, meta));
                        if (!held.isEmpty() && placeId == (BlockID)held.itemID) {
                            ctx.player->inventory.removeStack(ctx.player->selectedSlot, 1);
                        }
                        SoundManager::playSound("place");
                    }
                }
            }
            ctx.rightMouseDown = true;
        }
    } else {
        ctx.rightMouseDown = false;
    }
}

void handleContainerAndCraftShortcuts(GameContext& ctx, GLFWwindow* window) {
    if (ctx.inventoryOpen && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!ctx.cDown) {
            ItemStack in = ctx.player->inventory.getStack(0);
            if (!in.isEmpty() && in.itemID == (ItemID)Blocks::LOG && in.count >= 1) {
                ctx.player->inventory.removeStack(0, 1);
                ItemStack out((ItemID)Blocks::PLANKS, 4, 0);
                ctx.player->addToInventory(out);
            }
            ctx.cDown = true;
        }
    } else {
        ctx.cDown = false;
    }

    ChestBlockEntity* openChest = getOpenChest(ctx);
    if (openChest && glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        if (!ctx.tDown) {
            ItemStack from = ctx.player->inventory.getStack(0);
            if (!from.isEmpty()) {
                ItemStack moved = from;
                if (moved.count > 1) moved.count = 1;
                bool ok = openChest->inventory.addItem(moved);
                if (ok) ctx.player->inventory.removeStack(0, 1);
            }
            ctx.tDown = true;
        }
    } else {
        ctx.tDown = false;
    }

    FurnaceBlockEntity* openFurnace = getOpenFurnace(ctx);
    if (openFurnace && glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        if (!ctx.yDown) {
            ItemStack in0 = openFurnace->inventory.getStack(0);
            if (!in0.isEmpty() && in0.itemID == (ItemID)Blocks::COBBLESTONE && in0.count >= 1) {
                ItemStack outSlot = openFurnace->inventory.getStack(2);
                if (outSlot.isEmpty() || (outSlot.itemID == (ItemID)Blocks::STONE && outSlot.count < 64)) {
                    openFurnace->inventory.removeStack(0, 1);
                    if (outSlot.isEmpty()) {
                        openFurnace->inventory.setStack(2, ItemStack((ItemID)Blocks::STONE, 1, 0));
                    } else {
                        outSlot.count += 1;
                        openFurnace->inventory.setStack(2, outSlot);
                    }
                }
            }
            ctx.yDown = true;
        }
    } else {
        ctx.yDown = false;
    }

    if (openFurnace && glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        if (!ctx.uDown) {
            ItemStack from = ctx.player->inventory.getStack(0);
            if (!from.isEmpty()) {
                ItemStack moved = from;
                if (moved.count > 1) moved.count = 1;
                ItemStack cur = openFurnace->inventory.getStack(0);
                if (cur.isEmpty()) {
                    openFurnace->inventory.setStack(0, moved);
                    ctx.player->inventory.removeStack(0, 1);
                } else if (cur.itemID == moved.itemID && cur.count < 64) {
                    cur.count += 1;
                    openFurnace->inventory.setStack(0, cur);
                    ctx.player->inventory.removeStack(0, 1);
                }
            }
            ctx.uDown = true;
        }
    } else {
        ctx.uDown = false;
    }
}
}  // namespace app
