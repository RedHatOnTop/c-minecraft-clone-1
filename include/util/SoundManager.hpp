#pragma once

#include <string>
#include <map>
#include "miniaudio.h"

class SoundManager {
public:
    static bool init();
    static void cleanup();

    static void playSound(const std::string& name);
    static void loadSound(const std::string& name, const std::string& path);

private:
    static inline ma_engine m_engine;
    static inline std::map<std::string, std::string> m_sounds;
};
