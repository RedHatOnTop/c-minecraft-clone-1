#pragma once

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

enum class Face {
    DOWN, UP, NORTH, SOUTH, WEST, EAST
};

struct ModelFace {
    std::string texture;
    glm::vec4 uv; // x1, y1, x2, y2
    int rotation;
};

struct ModelElement {
    glm::vec3 from;
    glm::vec3 to;
    std::map<Face, ModelFace> faces;
};

class BlockModel {
public:
    BlockModel();
    ~BlockModel();

    void loadFromFile(const std::string& path);
    
    const std::vector<ModelElement>& getElements() const { return m_elements; }
    const std::map<std::string, std::string>& getTextures() const { return m_textures; }

private:
    std::string m_parent;
    std::map<std::string, std::string> m_textures;
    std::vector<ModelElement> m_elements;

    void parseJson(const nlohmann::json& j);
    Face stringToFace(const std::string& s);
};
