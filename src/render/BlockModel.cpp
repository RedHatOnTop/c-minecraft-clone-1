#include "render/BlockModel.hpp"
#include <fstream>
#include <iostream>

BlockModel::BlockModel() {}
BlockModel::~BlockModel() {}

void BlockModel::loadFromFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "Failed to open block model file: " << path << std::endl;
        return;
    }

    nlohmann::json data;
    try {
        f >> data;
        parseJson(data);
    } catch (const std::exception& e) {
        std::cerr << "Error parsing block model " << path << ": " << e.what() << std::endl;
    }
}

void BlockModel::parseJson(const nlohmann::json& j) {
    if (j.contains("parent")) {
        m_parent = j["parent"];
        // In a full implementation, we would recursively load the parent here
    }

    if (j.contains("textures")) {
        for (auto it = j["textures"].begin(); it != j["textures"].end(); ++it) {
            m_textures[it.key()] = it.value();
        }
    }

    if (j.contains("elements")) {
        for (const auto& el : j["elements"]) {
            ModelElement element;
            element.from = glm::vec3(el["from"][0], el["from"][1], el["from"][2]);
            element.to = glm::vec3(el["to"][0], el["to"][1], el["to"][2]);

            if (el.contains("faces")) {
                for (auto it = el["faces"].begin(); it != el["faces"].end(); ++it) {
                    Face f = stringToFace(it.key());
                    ModelFace face;
                    face.texture = it.value()["texture"];
                    if (it.value().contains("uv")) {
                        face.uv = glm::vec4(it.value()["uv"][0], it.value()["uv"][1], it.value()["uv"][2], it.value()["uv"][3]);
                    } else {
                        face.uv = glm::vec4(0, 0, 16, 16); // Default UV
                    }
                    face.rotation = it.value().value("rotation", 0);
                    element.faces[f] = face;
                }
            }
            m_elements.push_back(element);
        }
    }
}

Face BlockModel::stringToFace(const std::string& s) {
    if (s == "down") return Face::DOWN;
    if (s == "up") return Face::UP;
    if (s == "north") return Face::NORTH;
    if (s == "south") return Face::SOUTH;
    if (s == "west") return Face::WEST;
    if (s == "east") return Face::EAST;
    return Face::UP; // Default
}
