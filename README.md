# Minecraft Java Edition 1.8.9 Clone (C++)

A high-fidelity Minecraft 1.8.9 clone built from scratch using C++20 and OpenGL 3.3.

## Features
- **Modern Rendering**: OpenGL 3.3 Core Profile with support for 1.8-style JSON block models.
- **Voxel Engine**: Optimized chunk-based system (16x256x16) with face culling.
- **World Generation**: Terrain generation using FastNoiseLite (Perlin/Simplex).
- **High-Fidelity UI**: Accurate font renderer (`ascii.png`) and viewport-aware GUI scaling.
- **Physics**: AABB-based collision detection and 1.8-style movement mechanics.
- **Item System**: Extensible `ItemRegistry` and `Inventory` management.
- **Persistence**: Versioned binary serialization with legacy migration support.

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
3. Configure and build:
   ```bash
   cmake -B build -DMC_OFFLINE_FETCHCONTENT=ON
   cmake --build build --config Release
   ```
4. If dependency caches are missing on a new machine, run one online configure first:
   ```bash
   cmake -B build -DMC_OFFLINE_FETCHCONTENT=OFF
   ```
   After that, use `MC_OFFLINE_FETCHCONTENT=ON` for reproducible offline configure/build.
5. The executable is generated in `build/bin/`.

## Asset Policy
- Required runtime assets are under `assets/minecraft/`.
- `assets/minecraft/shaders/` files are static assets and are not generated at runtime.
- `assets/minecraft/textures/blocks/` contains the minimum required baseline textures.
- `assets/sample-texturepack/` is optional and treated as an additional texture source when present.

## Repository Organization
- `src/`: Core implementation files.
- `include/`: Header files organized by module.
- `assets/`: Textures, shaders, and models.
- `cmake/`: Build and dependency helpers.
- `docs/quality/`: Defect ledger and quality notes.
- `docs/quality/parity-matrix-v1.md`: Parity item matrix and baseline item ratings.
- `docs/quality/scoring-handbook.md`: Scoring rules and item interpretation examples.
- `docs/quality/baseline-report-2026-03-08.md`: Published baseline score and interpretation.
- `docs/quality/top-20-gap-backlog-2026-03-08.md`: Locked top-20 parity gaps with phase/owner mapping.
- `docs/quality/subphase-0.1-qg-report.md`: Subphase 0.1 quality-gate verification.
- `docs/quality/subphase-0.2-qg-report.md`: Subphase 0.2 quality-gate verification.
- `docs/quality/subphase-0.3-qg-report.md`: Subphase 0.3 quality-gate verification.
- `docs/quality/baseline/`: Baseline scenario evidence package (logs, manifests, indexes).
- `docs/roadmap.md`: Project roadmap and milestone plan.
- `external/`: Third-party header-only libraries.

## License
This project is for educational purposes. All Minecraft assets belong to Mojang Studios.
