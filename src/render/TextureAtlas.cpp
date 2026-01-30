#include "render/TextureAtlas.hpp"
#include "stb_image.h"
#include <iostream>
#include <cmath>
#include <algorithm>

TextureAtlas::TextureAtlas(int tileSize) : m_tileSize(tileSize), m_atlasID(0), m_atlasWidth(0), m_atlasHeight(0) {}

TextureAtlas::~TextureAtlas() {
    if (m_atlasID) glDeleteTextures(1, &m_atlasID);
}

void TextureAtlas::addTexture(const std::string& name, const std::string& path) {
    m_texturePaths[name] = path;
}

void TextureAtlas::generate() {
    if (m_texturePaths.empty()) return;

    int numTextures = m_texturePaths.size();
    int gridSide = std::ceil(std::sqrt(numTextures));
    m_atlasWidth = gridSide * m_tileSize;
    m_atlasHeight = gridSide * m_tileSize;

    std::vector<unsigned char> atlasData(m_atlasWidth * m_atlasHeight * 4, 0);

    int count = 0;
    for (auto const& [name, path] : m_texturePaths) {
        int w, h, ch;
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 4);
        
        if (data) {
            int gridX = count % gridSide;
            int gridY = count / gridSide;

            for (int y = 0; y < h; ++y) {
                for (int x = 0; x < w; ++x) {
                    int atlasIdx = ((gridY * m_tileSize + y) * m_atlasWidth + (gridX * m_tileSize + x)) * 4;
                    int imgIdx = (y * w + x) * 4;
                    atlasData[atlasIdx + 0] = data[imgIdx + 0];
                    atlasData[atlasIdx + 1] = data[imgIdx + 1];
                    atlasData[atlasIdx + 2] = data[imgIdx + 2];
                    atlasData[atlasIdx + 3] = data[imgIdx + 3];
                }
            }

            m_textureInfos[name] = {
                glm::vec2((float)(gridX * m_tileSize) / m_atlasWidth, (float)(gridY * m_tileSize) / m_atlasHeight),
                glm::vec2((float)((gridX + 1) * m_tileSize) / m_atlasWidth, (float)((gridY + 1) * m_tileSize) / m_atlasHeight)
            };

            stbi_image_free(data);
            count++;
        } else {
            std::cerr << "Failed to load texture for atlas: " << path << std::endl;
        }
    }

    glGenTextures(1, &m_atlasID);
    glBindTexture(GL_TEXTURE_2D, m_atlasID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_atlasWidth, m_atlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlasData.data());
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void TextureAtlas::bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_atlasID);
}

TextureInfo TextureAtlas::getTextureInfo(const std::string& name) const {
    if (m_textureInfos.find(name) != m_textureInfos.end()) {
        return m_textureInfos.at(name);
    }
    return { glm::vec2(0), glm::vec2(0) };
}
