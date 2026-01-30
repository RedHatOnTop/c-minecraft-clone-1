#pragma once

#include <map>
#include <memory>
#include <vector>
#include "world/World.hpp"
#include "world/WorldGenerator.hpp"
#include "render/ChunkMesh.hpp"
#include "render/TextureAtlas.hpp"
#include "render/Shader.hpp"

class ChunkProvider {
public:
    ChunkProvider(World& world, WorldGenerator& generator, TextureAtlas& atlas);
    ~ChunkProvider();

    void update(glm::vec3 playerPos, int renderDistance);
    void render(Shader& shader);

private:
    World& m_world;
    WorldGenerator& m_generator;
    TextureAtlas& m_atlas;

    std::map<glm::ivec2, std::unique_ptr<ChunkMesh>, IVec2Compare> m_meshes;

    void loadAround(int centerX, int centerZ, int distance);
    void unloadFar(int centerX, int centerZ, int distance);
};
