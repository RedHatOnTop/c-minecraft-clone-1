#include "render/TextureAtlas.hpp"
#include "stb_image.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <map>

TextureAtlas::TextureAtlas(int tileSize) : m_tileSize(tileSize), m_atlasID(0), m_atlasWidth(0), m_atlasHeight(0) {}

TextureAtlas::~TextureAtlas() {
    if (m_atlasID) glDeleteTextures(1, &m_atlasID);
}

void TextureAtlas::addTexture(const std::string& name, const std::string& path) {
    m_texturePaths[name] = path;
}

static std::vector<unsigned char> makeCheckerboard(int size) {
    std::vector<unsigned char> out(size * size * 4, 255);
    const int cell = std::max(1, size / 4);
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            bool purple = ((x / cell) + (y / cell)) % 2 == 0;
            int idx = (y * size + x) * 4;
            if (purple) {
                out[idx + 0] = 170;
                out[idx + 1] = 0;
                out[idx + 2] = 170;
                out[idx + 3] = 255;
            } else {
                out[idx + 0] = 0;
                out[idx + 1] = 0;
                out[idx + 2] = 0;
                out[idx + 3] = 255;
            }
        }
    }
    return out;
}

static std::vector<unsigned char> resampleNearest(const unsigned char* src, int sw, int sh, int dstSize) {
    std::vector<unsigned char> dst(dstSize * dstSize * 4, 0);
    for (int y = 0; y < dstSize; ++y) {
        int sy = (int)((long long)y * sh / dstSize);
        for (int x = 0; x < dstSize; ++x) {
            int sx = (int)((long long)x * sw / dstSize);
            int sIdx = (sy * sw + sx) * 4;
            int dIdx = (y * dstSize + x) * 4;
            dst[dIdx + 0] = src[sIdx + 0];
            dst[dIdx + 1] = src[sIdx + 1];
            dst[dIdx + 2] = src[sIdx + 2];
            dst[dIdx + 3] = src[sIdx + 3];
        }
    }
    return dst;
}

void TextureAtlas::generate() {
    m_textureInfos.clear();
    if (m_texturePaths.empty()) return;

    struct LoadedTexture {
        std::string name;
        std::string path;
        int w;
        int h;
        bool ok;
        std::vector<unsigned char> rgba;
    };

    std::vector<LoadedTexture> loaded;
    loaded.reserve(m_texturePaths.size());

    int detectedTileSize = 0;
    for (auto const& [name, path] : m_texturePaths) {
        int w = 0, h = 0, ch = 0;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 4);
        LoadedTexture lt;
        lt.name = name;
        lt.path = path;
        lt.w = w;
        lt.h = h;
        lt.ok = data != nullptr && w > 0 && h > 0;
        if (lt.ok) {
            lt.rgba.assign(data, data + (w * h * 4));
            detectedTileSize = std::max(detectedTileSize, std::max(w, h));
            stbi_image_free(data);
        } else {
            if (data) stbi_image_free(data);
            std::cerr << "Atlas texture missing/unreadable: " << name << " -> " << path << std::endl;
        }
        loaded.push_back(std::move(lt));
    }

    int tileSize = detectedTileSize > 0 ? detectedTileSize : m_tileSize;
    tileSize = std::clamp(tileSize, 16, 256);
    if (tileSize != 16 && tileSize != 32 && tileSize != 64 && tileSize != 128 && tileSize != 256) {
        int candidates[] = {16, 32, 64, 128, 256};
        int best = 16;
        int bestDist = std::abs(tileSize - best);
        for (int c : candidates) {
            int d = std::abs(tileSize - c);
            if (d < bestDist) {
                best = c;
                bestDist = d;
            }
        }
        tileSize = best;
    }
    m_tileSize = tileSize;

    int numTextures = (int)loaded.size();
    int gridSide = (int)std::ceil(std::sqrt((float)numTextures));
    m_atlasWidth = gridSide * tileSize;
    m_atlasHeight = gridSide * tileSize;

    std::vector<unsigned char> atlasData(m_atlasWidth * m_atlasHeight * 4, 0);

    for (int i = 0; i < (int)loaded.size(); ++i) {
        const auto& lt = loaded[i];
        int gridX = i % gridSide;
        int gridY = i / gridSide;

        std::vector<unsigned char> tile;
        if (!lt.ok) {
            tile = makeCheckerboard(tileSize);
        } else if (lt.w != tileSize || lt.h != tileSize) {
            tile = resampleNearest(lt.rgba.data(), lt.w, lt.h, tileSize);
        } else {
            tile = lt.rgba;
        }

        for (int y = 0; y < tileSize; ++y) {
            for (int x = 0; x < tileSize; ++x) {
                int atlasIdx = ((gridY * tileSize + y) * m_atlasWidth + (gridX * tileSize + x)) * 4;
                int imgIdx = (y * tileSize + x) * 4;
                atlasData[atlasIdx + 0] = tile[imgIdx + 0];
                atlasData[atlasIdx + 1] = tile[imgIdx + 1];
                atlasData[atlasIdx + 2] = tile[imgIdx + 2];
                atlasData[atlasIdx + 3] = tile[imgIdx + 3];
            }
        }

        m_textureInfos[lt.name] = {
            glm::vec2((float)(gridX * tileSize) / m_atlasWidth, (float)(gridY * tileSize) / m_atlasHeight),
            glm::vec2((float)((gridX + 1) * tileSize) / m_atlasWidth, (float)((gridY + 1) * tileSize) / m_atlasHeight)
        };
    }

    if (!loaded.empty()) {
        const auto& first = loaded[0];
        m_textureInfos["__missing"] = m_textureInfos[first.name];
    } else {
        m_textureInfos["__missing"] = {glm::vec2(0.0f), glm::vec2(1.0f)};
    }

    glGenTextures(1, &m_atlasID);
    glBindTexture(GL_TEXTURE_2D, m_atlasID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_atlasWidth, m_atlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlasData.data());
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void TextureAtlas::bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_atlasID);
}

TextureInfo TextureAtlas::getTextureInfo(const std::string& name) const {
    auto it = m_textureInfos.find(name);
    if (it != m_textureInfos.end()) {
        return it->second;
    }

    static std::map<std::string, bool> missingLogged;
    if (!missingLogged[name]) {
        std::cerr << "Atlas texture key missing: " << name << " (using __missing UV)\n";
        missingLogged[name] = true;
    }

    auto missing = m_textureInfos.find("__missing");
    if (missing != m_textureInfos.end()) {
        return missing->second;
    }
    return {glm::vec2(0.0f), glm::vec2(1.0f)};
}
