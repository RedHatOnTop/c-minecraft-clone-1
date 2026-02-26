#include <vector>
#include <cmath>

#include "physics/Raycast.hpp"
#include "test_common.hpp"
#include "ui/FontRenderer.hpp"
#include "ui/GUIRenderer.hpp"
#include "ui/HUD.hpp"
#include "world/ChunkProvider.hpp"
#include "world/World.hpp"

void test_Raycast_ZeroDirectionComponent_NoCrash() {
    World world;
    world.loadChunk(0, 0);
    world.setBlock(2, 2, 2, Block(Blocks::STONE));

    RaycastResult hit = Raycaster::raycast(glm::vec3(0.5f, 2.5f, 2.5f), glm::vec3(1.0f, 0.0f, 0.0f), 10.0f, world);
    TEST_ASSERT(hit.hit);

    RaycastResult zero = Raycaster::raycast(glm::vec3(0.0f), glm::vec3(0.0f), 10.0f, world);
    TEST_ASSERT(!zero.hit);
}

void test_UI_ViewportResize_ProjectionUpdate() {
    glm::ivec2 guiViewport = GUIRenderer::normalizeViewport(-320, 0);
    TEST_ASSERT(guiViewport.x == 1);
    TEST_ASSERT(guiViewport.y == 1);

    glm::ivec2 fontViewport = FontRenderer::normalizeViewport(1920, 1080);
    TEST_ASSERT(fontViewport.x == 1920);
    TEST_ASSERT(fontViewport.y == 1080);

    glm::vec2 crosshairCenter = HUD::crosshairCenterForViewport(1366, 768);
    TEST_ASSERT(std::abs(crosshairCenter.x - 683.0f) < 0.0001f);
    TEST_ASSERT(std::abs(crosshairCenter.y - 384.0f) < 0.0001f);

    glm::vec2 hotbarOrigin = HUD::hotbarOriginForViewport(1366, 768);
    TEST_ASSERT(std::abs(hotbarOrigin.x - 501.0f) < 0.0001f);
    TEST_ASSERT(std::abs(hotbarOrigin.y - 722.0f) < 0.0001f);

    glm::vec2 clampedCenter = HUD::crosshairCenterForViewport(0, -100);
    TEST_ASSERT(std::abs(clampedCenter.x - 0.5f) < 0.0001f);
    TEST_ASSERT(std::abs(clampedCenter.y - 0.5f) < 0.0001f);
}

void test_ChunkMesh_TransparentSort_Stable() {
    const glm::vec3 cameraPos(0.0f, CHUNK_HEIGHT * 0.5f, 0.0f);
    std::vector<glm::ivec2> positions = {
        glm::ivec2(1, 0),
        glm::ivec2(0, 1),
        glm::ivec2(-1, 0),
        glm::ivec2(0, -1),
        glm::ivec2(4, 0)
    };

    std::vector<glm::ivec2> ordered = ChunkProvider::sortTransparentPositions(positions, cameraPos);
    TEST_ASSERT(ordered.size() == positions.size());

    auto chunkDistanceSq = [&](const glm::ivec2& pos) {
        glm::vec3 min(pos.x * CHUNK_WIDTH, 0.0f, pos.y * CHUNK_WIDTH);
        glm::vec3 max((pos.x + 1) * CHUNK_WIDTH, (float)CHUNK_HEIGHT, (pos.y + 1) * CHUNK_WIDTH);
        glm::vec3 center = 0.5f * (min + max);
        return glm::dot(center - cameraPos, center - cameraPos);
    };

    for (size_t i = 1; i < ordered.size(); ++i) {
        TEST_ASSERT(chunkDistanceSq(ordered[i - 1]) >= chunkDistanceSq(ordered[i]));
    }

    TEST_ASSERT(ordered[0] == glm::ivec2(4, 0));
    TEST_ASSERT(ordered[1] == glm::ivec2(0, 1));
    TEST_ASSERT(ordered[2] == glm::ivec2(1, 0));
}
