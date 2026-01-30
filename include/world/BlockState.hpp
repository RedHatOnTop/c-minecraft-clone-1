#pragma once

#include <string>
#include <map>
#include <memory>
#include "render/BlockModel.hpp"

struct BlockState {
    std::string name;
    std::string modelPath;
    // Map of face to texture name used in the model
    std::map<Face, std::string> faceTextures;
};

class BlockStateManager {
public:
    static void registerBlockState(BlockID id, const BlockState& state) {
        m_blockStates[id] = state;
    }

    static const BlockState* getBlockState(BlockID id) {
        if (m_blockStates.find(id) != m_blockStates.end()) {
            return &m_blockStates.at(id);
        }
        return nullptr;
    }

private:
    static inline std::map<BlockID, BlockState> m_blockStates;
};
