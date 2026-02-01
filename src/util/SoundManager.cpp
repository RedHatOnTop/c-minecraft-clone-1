#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "util/SoundManager.hpp"
#include <iostream>

bool SoundManager::init() {
    ma_result result = ma_engine_init(NULL, &m_engine);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio engine." << std::endl;
        return false;
    }
    return true;
}

void SoundManager::cleanup() {
    ma_engine_uninit(&m_engine);
}

void SoundManager::loadSound(const std::string& name, const std::string& path) {
    m_sounds[name] = path;
}

void SoundManager::playSound(const std::string& name) {
    if (m_sounds.count(name)) {
        ma_engine_play_sound(&m_engine, m_sounds[name].c_str(), NULL);
    }
}
