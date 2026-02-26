cmake_minimum_required(VERSION 3.16)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(MinecraftClone189 VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    add_definitions(-DNOMINMAX)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(cmake/Dependencies.cmake)

# Include directories
include_directories(include)
include_directories(external)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.hpp" "include/*.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE glfw nlohmann_json::nlohmann_json gl3w glm::glm)

# If using glad2 FetchContent, we need to generate it
if(TARGET glad)
    # This depends on how glad is structured. Glad2 can generate a library.
    # For now, I'll assume we want GL 3.3 Core.
    # If this fails, I'll switch to a pre-generated glad in external/.
endif()

# Platform specific libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE GL)
endif()

# Copy assets to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)

include(CTest)
if(BUILD_TESTING)
    set(TEST_SHARED_SOURCES
        src/world/WorldPersistence.cpp
        src/world/Chunk.cpp
        src/world/World.cpp
        src/world/Lighting.cpp
        src/world/BlockEntity.cpp
        src/physics/Raycast.cpp
        src/physics/Player.cpp
        src/render/Camera.cpp
        src/item/Inventory.cpp
        src/util/SoundManager.cpp
    )

    function(add_mc_test_target target_name)
        add_executable(${target_name}
            ${ARGN}
            ${TEST_SHARED_SOURCES}
        )
        target_include_directories(${target_name} PRIVATE include external tests)
        target_link_libraries(${target_name} PRIVATE glfw gl3w nlohmann_json::nlohmann_json glm::glm)
        if(WIN32)
            target_link_libraries(${target_name} PRIVATE opengl32)
        else()
            target_link_libraries(${target_name} PRIVATE GL)
        endif()
        add_test(NAME ${target_name} COMMAND ${target_name})
        set_tests_properties(${target_name} PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endfunction()

    add_mc_test_target(tests_data
        tests/test_main_data.cpp
        tests/test_persistence.cpp
    )

    add_mc_test_target(tests_runtime
        tests/test_main_runtime.cpp
        tests/test_inventory.cpp
    )

    add_mc_test_target(tests_render
        tests/test_main_render.cpp
        tests/test_raycast.cpp
    )

    add_mc_test_target(tests_gameplay_smoke
        tests/test_main_gameplay_smoke.cpp
        tests/test_replay.cpp
    )
endif()
