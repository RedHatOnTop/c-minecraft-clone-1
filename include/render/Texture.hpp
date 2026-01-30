#pragma once

#include <string>
#include <GL/gl3w.h>

class Texture {
public:
    unsigned int ID;
    int width, height, nrChannels;

    Texture(const char* path, bool flipY = true);
    ~Texture();

    void bind(unsigned int unit = 0) const;
};
