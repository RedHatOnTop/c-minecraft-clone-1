#pragma once

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "render/Texture.hpp"

struct TextureInfo {
    glm::vec2 uvMin;
    glm::vec2 uvMax;
};

class TextureAtlas {
public:
    TextureAtlas(int tileSize = 16);
    ~TextureAtlas();

    void addTexture(const std::string& name, const std::string& path);
    void generate();
    void bind(unsigned int unit = 0) const;

    TextureInfo getTextureInfo(const std::string& name) const;

private:
    int m_tileSize;
    std::map<std::string, std::string> m_texturePaths;
    std::map<std::string, TextureInfo> m_textureInfos;
    unsigned int m_atlasID;
    int m_atlasWidth, m_atlasHeight;
};
