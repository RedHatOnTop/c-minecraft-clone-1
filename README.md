# Minecraft Java Edition 1.8.9 Clone (C++)

A high-fidelity Minecraft 1.8.9 clone built from scratch using C++20 and OpenGL 3.3.

## Features
- **Modern Rendering**: OpenGL 3.3 Core Profile with support for 1.8-style JSON block models.
- **Voxel Engine**: Optimized chunk-based system (16x256x16) with face culling.
- **World Generation**: Terrain generation using FastNoiseLite (Perlin/Simplex).
- **High-Fidelity UI**: Accurate Font Renderer (`ascii.png`) and GUI scaling system.
- **Physics**: AABB-based collision detection and 1.8-style movement mechanics.
- **Item System**: Extensible ItemRegistry and Inventory management.
- **Persistence**: Binary serialization for world saving and loading.

## Tech Stack
- **Language**: C++20
- **Graphics**: OpenGL 3.3
- **Windowing**: GLFW
- **Math**: GLM
- **JSON**: nlohmann/json
- **Noise**: FastNoiseLite
- **Image Loading**: stb_image

## Building the Project
1. Ensure you have CMake (3.16+) and a C++20 compatible compiler (MSVC, GCC, or Clang).
2. Clone the repository.
3. Run the following commands:
   ```bash
   cmake -B build
   cmake --build build --config Release
   ```
4. The executable will be located in `build/bin/`.

## Repository Organization
- `src/`: Core implementation files.
- `include/`: Header files organized by module.
- `assets/`: Minecraft-style textures, shaders, and models.
- `external/`: Third-party header-only libraries.

## License
This project is for educational purposes. All Minecraft assets belong to Mojang Studios.
